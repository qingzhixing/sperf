#pragma once
#ifndef OUTPUT_PARSER_H
#define OUTPUT_PARSER_H

#include <string>
#include <iostream>
#include <pidfd_open.hpp>
#include <map>
#include <chrono>

class OutputParser
{
public:
	/**
	 * @brief 解析 strace 输出
	 *
	 * @param pid 子进程 pid
	 * @param fd_read 管道读口 fd
	 */
	static void ParseOutput(const int child_pid, const int fd_read)
	{
		int status;
		char buffer[4096];

		while (true)
		{
			// 非阻塞监测子进程是否退出
			auto result = waitpid(child_pid, &status, WNOHANG);
			if (result < 0)
			{
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
			if (result == child_pid)
			{
				// 子进程已退出，处理剩余数据后跳出循环
				// 在退出前，需要将管道中可能残留的数据读完
				ssize_t n = read(fd_read, buffer, sizeof(buffer));
				if (n > 0)
				{
					// 解析并处理最后这批数据
					ParseAndAccumulate(buffer, n);
				}
				// 输出最后一次统计信息
				PrintSyscallTimes();
				break;
			}

			// 子进程未结束时读取数据
			// 设置超时
			fd_set readfds;
			// 清空fd_set
			FD_ZERO(&readfds);
			// 添加fd_read到fd_set
			FD_SET(fd_read, &readfds);

			struct timeval timeout = {0, 100000}; // 0 s + 1e5 us = 100 ms
			auto activity = select(fd_read + 1, &readfds, NULL, NULL, &timeout);
			if (activity < 0)
			{
				perror("select");
				exit(EXIT_FAILURE);
			}
			// timeout
			if (activity == 0)
			{
				continue;
			}
			// 有数据可读
			if (FD_ISSET(fd_read, &readfds))
			{
				ssize_t n = read(fd_read, buffer, sizeof(buffer));
				if (n > 0)
				{
					ParseAndAccumulate(buffer, n);
				}

				if (IsTimeToPrint())
				{
					PrintSyscallTimes();
					fflush(stdout);
				}
			}
		}
	}

	/**
	 * @brief 解析 strace 输出并累加 syscall 时间
	 *
	 * @param buffer  strace 输出的数据
	 * @param n       数据字节数
	 */
	static void ParseAndAccumulate(char *buffer, ssize_t n)
	{
		// 维护一个行缓冲，用于解析每行数据
		static std::string line_buffer;
		line_buffer.append(buffer, n);
		// 查找换行符，将行缓冲中的数据解析并累加
		size_t pos;
		while ((pos = line_buffer.find('\n')) != std::string::npos)
		{
			auto line = line_buffer.substr(0, pos);
			line_buffer.erase(0, pos + 1);
			// 解析 line 并累加 syscall 时间
			auto [syscall_name, cost_time] = ParseSingleLine(line);
			if (!syscall_name.empty())
			{
				syscall_time_map[syscall_name] += cost_time;
			}
		}
	}

	/**
	 * @brief 解析单行 strace 输出并累加 syscall 时间
	 *
	 * @param line 单行 strace 输出
	 * @return std::pair<std::string, double>  syscall_name 和 cost_time
	 */
	static std::pair<std::string, double> auto(const std::string &line)
	{
		// 每行格式为 "syscall_name(...) = return_value <cost_time>"
		// 提取 syscall_name 并累加时间
		auto name_start = line.find_first_not_of(" \t");
		if (name_start == std::string::npos)
		{
			return;
		}
		auto name_end = line.find("(", name_start);
		if (name_end == std::string::npos)
		{
			return;
		}
		std::string syscall_name = line.substr(name_start, name_end - name_start);

		// 提取 cost_time
		auto cost_start = line.find_last_of("<") + 1;
		auto cost_end = line.find_last_of(">");
		if (cost_start == std::string::npos || cost_end == std::string::npos)
		{
			return;
		}
		std::string cost_time_str = line.substr(cost_start, cost_end - cost_start);
		try
		{
			double cost_time = std::stod(cost_time_str);
			return {syscall_name, cost_time};
		}
		catch (const std::invalid_argument &e)
		{
			// 忽略无法转换为 double 的 cost_time
		}
	}

	static void PrintSyscallTimes()
	{
		// clear screen
		std::cout << "\033[2J\033[1;1H";

		std::cout << "🕰️ 系统调用耗时统计（单位：秒）:\n";
		for (const auto &[syscall, time] : syscall_time_map)
		{
			std::cout << std::format("{:<20} {:>10.6f}\n", syscall, time);
		}
	}

	static bool IsTimeToPrint()
	{
		// 100ms打印一次
		static auto last_print_time = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_print_time);
		if (duration.count() >= 100)
		{
			last_print_time = now;
			return true;
		}
		return false;
	}

private:
	inline static std::map<std::string, double> syscall_time_map;
};
#endif //! OUTPUT_PARSER_H