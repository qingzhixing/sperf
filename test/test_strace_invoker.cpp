#include <strace_invoker.hpp>
#include <testkit.h>

class TestStraceInvoker
{
public:
	static char **ConstructExecArgv(const std::vector<std::string> &strace_args,
									const std::string &executable_name,
									const std::vector<std::string> &executable_arguments)
	{
		return StraceInvoker::ConstructExecArgv(strace_args, executable_name, executable_arguments);
	}
};

UnitTest(Test_ConstructExecArgv)
{
	auto exec_argv = TestStraceInvoker::ConstructExecArgv({"-T"}, "ls", {"-l", "./"});
	assert(exec_argv != nullptr);
	assert(strcmp(exec_argv[0], "strace") == 0);
	assert(strcmp(exec_argv[1], "-T") == 0);
	assert(strcmp(exec_argv[2], "ls") == 0);
	assert(strcmp(exec_argv[3], "-l") == 0);
	assert(strcmp(exec_argv[4], "./") == 0);
	// free exec_argv
	for (size_t i = 0; exec_argv[i] != NULL; i++)
	{
		free(exec_argv[i]);
	}
	delete[] exec_argv;
}