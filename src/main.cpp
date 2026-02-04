#include <project_info.h>
#include <argument_handler.h>
#include <strace_invoker.h>
#include <iostream>
#include <format>
#include <string>
#include <vector>

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
	StraceInvoker strace_invoker;
	auto pid = strace_invoker.Invoke({"-T", "-f"}, executable_name, executable_arguments);
	// Wait for strace to finish
	waitpid(pid, NULL, 0);
	return 0;
}