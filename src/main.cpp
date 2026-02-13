#include <project_info.hpp>
#include <argument_handler.hpp>
#include <strace_invoker.hpp>
#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <output_parser.hpp>

int main(int argc, char *argv[])
{
	auto sperf_arguments = ArgumentHandler::HandleArguments(argc, argv);

	// Check error message
	if (!sperf_arguments.error_message.empty())
	{
		std::cerr << std::format("Error: {}\n", sperf_arguments.error_message);
		ProjectInfo::PrintHelp();
		return 1;
	}

	// Print help message
	if (sperf_arguments.print_help)
	{
		ProjectInfo::PrintHelp();
		return 0;
	}

	// Print version information
	if (sperf_arguments.print_version)
	{
		ProjectInfo::PrintVersion();
		return 0;
	}

	const auto &executable_name = sperf_arguments.executable_name;
	const auto &executable_arguments = sperf_arguments.executable_arguments;

	// Invoke strace
	auto [pid, fd_read] = StraceInvoker::Invoke({"-T", "-f"}, executable_name, executable_arguments);

	// Parse strace output
	OutputParser::ParseOutput(fd_read);

	// Wait for strace to finish
	int status;
	waitpid(pid, &status, 0);
	// 判断是否异常退出，并获取错误码
	if (WIFEXITED(status))
	{
		int error_code = WEXITSTATUS(status);
		if (error_code != 0)
		{
			std::cout << std::format("❌ 子进程 {} 退出, 错误码: {}\n", pid, error_code);
			return 1;
		}
	}
	else if (WIFSIGNALED(status))
	{
		std::cout << std::format("❌ 子进程 {} 被信号 {} 终止, Status: {}\n", pid, WTERMSIG(status), status);
		return 1;
	}
	else
	{
		std::cout << std::format("❌ 子进程 {} 异常退出, Status: {}\n", pid, status);
		return 1;
	}
	return 0;
}