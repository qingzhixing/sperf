#pragma once
#ifndef ARGUMENT_HANDLER_HPP
#define ARGUMENT_HANDLER_HPP

#include <string>
#include <vector>

struct SperfArguments
{
	bool print_help = false;
	bool print_version = false;
	std::string error_message;
	std::string executable_name;
	std::vector<std::string> executable_arguments;
};

class ArgumentHandler
{
public:
	/**
	 * @brief HandleArguments Handle arguments passed to the program.
	 * @param argc Argument count.
	 * @param argv Argument vector.
	 * @return SperfArguments
	 *
	 * Usage: sperf [OPTIONS] COMMAND [ARG]...
	 * Options:
	 *   -h, --help    Show this help message.
	 *   -v, --version Show version information.
	 * 	 COMMAND                 The command you want to analyze.
	 *   ARG           Arguments for the command.
	 * Example:
	 *   sperf ls -l
	 */
	static SperfArguments HandleArguments(int argc, char *argv[])
	{
		SperfArguments sperf_args;

		if (argc == 1)
		{
			return {.error_message = "No arguments provided."};
		}

		// 先处理所有以 '-' 开头的选项（sperf 自己的选项）
		int arg_idx = 1;
		while (arg_idx < argc && argv[arg_idx][0] == '-')
		{
			std::string arg = argv[arg_idx];

			if (arg == "-h" || arg == "--help")
			{
				sperf_args.print_help = true;
			}
			else if (arg == "-v" || arg == "--version")
			{
				sperf_args.print_version = true;
			}
			else
			{
				// 未知选项
				return {.error_message = "Unknown option: " + arg};
			}

			arg_idx++;
		}

		// 剩下的部分是命令和参数
		if (arg_idx >= argc)
		{
			// 只有选项，没有命令
			if (!sperf_args.print_help && !sperf_args.print_version)
			{
				return {.error_message = "No command provided."};
			}
			return sperf_args;
		}

		// 第一个非选项参数是命令名
		sperf_args.executable_name = argv[arg_idx];
		arg_idx++;

		// 剩下的都是命令参数
		for (; arg_idx < argc; arg_idx++)
		{
			sperf_args.executable_arguments.push_back(argv[arg_idx]);
		}

		// 帮助/版本信息优先
		if (sperf_args.print_help || sperf_args.print_version)
		{
			sperf_args.executable_name.clear();
			sperf_args.executable_arguments.clear();
		}

		return sperf_args;
	}
};

#endif //! ARGUMENT_HANDLER_HPP