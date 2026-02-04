#pragma once
#ifndef ARGUMENT_HANDLER_H
#define ARGUMENT_HANDLER_H

#include <string>
#include <vector>
#include <getopt.h>

struct SperfArguments
{
	bool print_help = false;
	bool print_version = false;
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

		int opt;

		// long options
		static struct option long_options[] = {
			{"version", no_argument, nullptr, 'v'},
			{"help", no_argument, nullptr, 'h'},
			{nullptr, 0, nullptr, 0}};

		const char *optstring = "+hv";

		while ((opt = getopt_long(argc, argv, optstring, long_options, nullptr)) != -1)
		{
			switch (opt)
			{
			case 'v':
				sperf_args.print_version = true;
				break;

			case 'h':
				sperf_args.print_help = true;
				break;

			default:
				// Invalid options
				return {.print_help = true};
			}
		}

		// Parse COMMAND & ARGS
		if (optind >= argc)
		{
			return {.print_help = true};
		}

		sperf_args.executable_name = argv[optind];

		for (int i = optind + 1; i < argc; i++)
		{
			sperf_args.executable_arguments.push_back(argv[i]);
		}

		return sperf_args;
	}
};

#endif //! ARGUMENT_HANDLER_H