#pragma once
#ifndef OUTPUT_PARSER_HPP
#define OUTPUT_PARSER_HPP

#include <string>
#include <iostream>
#include <pidfd_open.hpp>
#include <map>
#include <chrono>
#include <sys/wait.h>
#include <format>

class OutputParser
{

public:
	OutputParser(int pid, int fd_read) : child_pid(pid), fd_read(fd_read), syscalls_time_s(0.0) {}

public:
	/**
	 * @brief 解析 strace 输出
	 *
	 * @param pid 子进程 pid
	 * @param fd_read 管道读口 fd
	 */
	void ParseOutput()
	{
		// 记录子进程启动时间，用于计算相对时间
		sub_process_start_time = std::chrono::steady_clock::now();
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
	void ParseAndAccumulate(char *buffer, ssize_t n)
	{
		// 维护一个行缓冲，用于解析每行数据
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
				syscalls_time_s += cost_time;
			}
		}
	}

	/**
	 * @brief 解析单行 strace 输出并累加 syscall 时间
	 *
	 * @param line 单行 strace 输出
	 * @return std::pair<std::string, double>  syscall_name 和 cost_time
	 */
	static std::pair<std::string, double> ParseSingleLine(const std::string &line)
	{
		// 每行格式为 "syscall_name(...) = return_value <cost_time>"
		// 提取 syscall_name 并累加时间
		auto name_start = line.find_first_not_of(" \t");
		if (name_start == std::string::npos)
		{
			return {};
		}
		auto name_end = line.find("(", name_start);
		if (name_end == std::string::npos)
		{
			return {};
		}
		std::string syscall_name = line.substr(name_start, name_end - name_start);

		// 提取 cost_time
		auto cost_start = line.find_last_of("<") + 1;
		auto cost_end = line.find_last_of(">");
		if (cost_start == std::string::npos || cost_end == std::string::npos)
		{
			return {};
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
			return {};
		}
	}

	/**
	 * @brief 格式化时间显示，智能选择合适的时间单位
	 *
	 * @param time_seconds 时间（秒）
	 * @return std::pair<double, std::string> 格式化后的数值和单位
	 */
	static std::pair<double, std::string> FormatTime(double time_seconds)
	{
		if (time_seconds >= 1.0)
		{
			// 大于1秒，使用秒为单位
			return {time_seconds, "s"};
		}
		else
		{
			// 1毫秒到1秒之间，使用毫秒为单位
			return {time_seconds * 1000.0, "ms"};
		}
	}

	/**
	 * @brief 输出系统调用耗时统计信息
	 */
	void PrintSyscallTimes()
	{
		// 计算子进程运行时间
		auto now = std::chrono::steady_clock::now();
		auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now - sub_process_start_time);
		double sub_process_time_s = duration_ns.count() / 1e9;

		// clear screen
		std::cout << "\033[2J\033[1;1H";

		// 智能选择时间单位
		auto [sub_process_time, sub_process_unit] = FormatTime(sub_process_time_s);
		auto [syscalls_time, syscalls_unit] = FormatTime(syscalls_time_s);

		std::cout << std::format("📊 子进程运行时间: {:.3f} {}\n", sub_process_time, sub_process_unit);
		std::cout << std::format("🔧 系统调用总耗时: {:.3f} {}\n", syscalls_time, syscalls_unit);
		std::cout << std::format("📈 系统调用占比: {:.2f}%\n\n", (syscalls_time_s / sub_process_time_s) * 100);

		std::cout << "🕰️ 系统调用耗时统计:\n";
		// 按耗时排序输出
		std::vector<std::pair<std::string, double>> sorted_syscalls(syscall_time_map.begin(), syscall_time_map.end());
		std::sort(sorted_syscalls.begin(), sorted_syscalls.end(),
				  [](const auto &a, const auto &b)
				  { return a.second > b.second; });

		// 打印表头
		std::cout << std::format("{:<20} {:>7} {:>8}\n",
								 "Syscall Name", "Time", "(%)");
		std::cout << std::string(45, '-') << "\n";

		for (const auto &[syscall, time] : sorted_syscalls)
		{
			auto [formatted_time, unit] = FormatTime(time);
			std::cout << std::format("{:<20} {:>8.3f} {:>2} ({:>6.2f}%)\n",
									 syscall, formatted_time, unit, time / syscalls_time_s * 100);
		}
	}

	/**
	 * @brief 判断是否需要打印统计信息，每 500 ms 打印一次
	 */
	bool IsTimeToPrint()
	{
		auto now = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_print_time);
		if (duration.count() >= 500)
		{
			last_print_time = now;
			return true;
		}
		return false;
	}

private:
	std::chrono::steady_clock::time_point last_print_time;
	std::chrono::steady_clock::time_point sub_process_start_time;
	double syscalls_time_s;
	std::map<std::string, double> syscall_time_map;
	std::string line_buffer;
	int child_pid;
	int fd_read;
};
#endif //! OUTPUT_PARSER_HPP