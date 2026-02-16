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
	OutputParser output_parser(pid, fd_read);
	output_parser.ParseOutput();

	return 0;
}