#include <project_info.h>
#include <unistd.h>
#include <argument_handler.h>
#include <iostream>
#include <format>
#include <string.h>
#include <string>
#include <vector>
#include <sys/wait.h>

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

	const auto &executable_name = sperf_arguments.executable_name;
	const auto &executable_arguments = sperf_arguments.executable_arguments;

	// 构造调用参数
	std::vector<std::string> strace_with_args = {"strace", "-T", "-f"};
	// strace & strace args
	char *exec_argv[strace_with_args.size() + executable_arguments.size() + 2];
	for (size_t i = 0; i < strace_with_args.size(); i++)
	{
		exec_argv[i] = strdup(strace_with_args[i].c_str());
	}
	// command
	exec_argv[strace_with_args.size()] = strdup(executable_name.c_str());
	// command args
	for (size_t i = 0; i < executable_arguments.size(); i++)
	{
		exec_argv[i + strace_with_args.size() + 1] = strdup(executable_arguments[i].c_str());
	}
	// NULL
	exec_argv[strace_with_args.size() + executable_arguments.size() + 1] = NULL;

	char *exec_envp[] = {
		strdup("PATH=/bin"),
		NULL,
	};

	auto pid = fork();

	// Child Process
	if (pid == 0)
	{
		std::cout << "❤️‍🔥Child process is running\n";
		execve("strace", exec_argv, exec_envp);
		execve("/bin/strace", exec_argv, exec_envp);
		execve("/usr/bin/strace", exec_argv, exec_envp);
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	// Parent Process
	int status;
	waitpid(pid, &status, 0); // 等待子进程

	return 0;
}