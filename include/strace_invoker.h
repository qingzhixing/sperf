#pragma once
#ifndef STRACE_INVOKER_H
#define STRACE_INVOKER_H

#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <project_info.h>
#include <fcntl.h>

class StraceInvoker
{
public:
	/**
	 * @brief 调用 strace 命令
	 *
	 * @param strace_args strace 命令参数
	 * @param executable_name 可执行文件名
	 * @param executable_arguments 可执行文件参数
	 * @return int 子进程 pid
	 */
	static int Invoke(const std::vector<std::string> &strace_args,
					  const std::string &executable_name,
					  const std::vector<std::string> &executable_arguments)
	{
		char **exec_argv = ConstructExecArgv(strace_args, executable_name, executable_arguments);

		char *exec_envp[] = {
			strdup("PATH=/bin"),
			NULL,
		};

		// 创建管道，用于将子进程的stderr重定向到父进程stdin
		int pipefd[2]; // pipefd[0]用于读，pipefd[1]用于写
		pipe(pipefd);

		auto pid = fork();

		// Child Process
		if (pid == 0)
		{
			// 关闭pipefd[0]，因为子进程只需要写
			close(pipefd[0]);

			// 屏蔽stdout
			int dev_null = open("/dev/null", O_WRONLY);
			if (dev_null == -1)
			{
				perror("open /dev/null");
				exit(EXIT_FAILURE);
			}
			dup2(dev_null, STDOUT_FILENO);
			close(dev_null);

			// 重定向stderr到pipefd[1]
			dup2(pipefd[1], STDERR_FILENO);
			close(pipefd[1]);

			execve("strace", exec_argv, exec_envp);
			execve("/bin/strace", exec_argv, exec_envp);
			execve("/usr/bin/strace", exec_argv, exec_envp);
			perror(ProjectInfo::project_name.c_str());
			exit(EXIT_FAILURE);
		}
		// Parent Process
		close(pipefd[1]); // 关闭pipefd[1]，因为父进程只需要读
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);

		// free exec_argv
		for (size_t i = 0; exec_argv[i] != NULL; i++)
		{
			free(exec_argv[i]);
		}
		delete[] exec_argv;
		// free exec_envp
		for (size_t i = 0; exec_envp[i] != NULL; i++)
		{
			free(exec_envp[i]);
		}

		return pid;
	}

private:
	static char **ConstructExecArgv(const std::vector<std::string> &strace_args,
									const std::string &executable_name,
									const std::vector<std::string> &executable_arguments)
	{
		char **exec_argv = new char *[strace_args.size() + executable_arguments.size() + 3];
		// strace
		exec_argv[0] = strdup("strace");
		// strace args
		for (size_t i = 0; i < strace_args.size(); i++)
		{
			exec_argv[i + 1] = strdup(strace_args[i].c_str());
		}
		// command
		exec_argv[strace_args.size() + 1] = strdup(executable_name.c_str());
		// command args
		for (size_t i = 0; i < executable_arguments.size(); i++)
		{
			exec_argv[i + strace_args.size() + 2] = strdup(executable_arguments[i].c_str());
		}
		// NULL
		exec_argv[strace_args.size() + executable_arguments.size() + 2] = NULL;

		return exec_argv;
	}
};

#endif //! STRACE_INVOKER_H