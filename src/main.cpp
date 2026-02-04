#include <project_info.h>
#include <unistd.h>
#include <argument_handler.h>
#include <iostream>
#include <format>

int main(int argc, char *argv[])
{
	auto sperf_arguments = ArgumentHandler::HandleArguments(argc, argv);

	// Check error message
	if (!sperf_arguments.error_message.empty())
	{
		std::cerr << "Error: " << sperf_arguments.error_message << "\n";
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

	std::cout << std::format("Executable name: {}\n", sperf_arguments.executable_name);
	std::cout << "Executable arguments: ";
	for (const auto &arg : sperf_arguments.executable_arguments)
	{
		std::cout << arg << " ";
	}
	std::cout << "\n";

	return 0;

	char *exec_argv[] = {
		"strace",
		"ls",
		NULL,
	};
	char *exec_envp[] = {
		"PATH=/bin",
		NULL,
	};
	execve("strace", exec_argv, exec_envp);
	execve("/bin/strace", exec_argv, exec_envp);
	execve("/usr/bin/strace", exec_argv, exec_envp);
	perror(argv[0]);
	exit(EXIT_FAILURE);
}