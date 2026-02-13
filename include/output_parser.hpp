#pragma once
#ifndef OUTPUT_PARSER_H
#define OUTPUT_PARSER_H

#include <string>
#include <iostream>
#include <pidfd_open.hpp>

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
	}
};
#endif //! OUTPUT_PARSER_H