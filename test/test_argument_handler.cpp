#include <testkit.h>
#include <argument_handler.hpp>
#include <string.h>

UnitTest(Test_HelpArgument_Long)
{
	char *argv[] = {strdup("sperf"), strdup("--help")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(sperf_args.print_help);
}

UnitTest(Test_HelpArgument_Short)
{
	char *argv[] = {strdup("sperf"), strdup("-h")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(sperf_args.print_help);
}

UnitTest(Test_VersionArgument_Long)
{
	char *argv[] = {strdup("sperf"), strdup("--version")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(sperf_args.print_version);
}

UnitTest(Test_VersionArgument_Short)
{
	char *argv[] = {strdup("sperf"), strdup("-v")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(sperf_args.print_version);
}

UnitTest(Test_NoArgument)
{
	char *argv[] = {strdup("sperf")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(!sperf_args.print_help);
	assert(!sperf_args.print_version);
	assert(!sperf_args.error_message.empty());
}

UnitTest(Test_InvalidArgument)
{
	char *argv[] = {strdup("sperf"), strdup("--invalid")};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	assert(!sperf_args.print_help);
	assert(!sperf_args.print_version);
	assert(!sperf_args.error_message.empty());
}

UnitTest(Test_ValidArgument)
{
	char *argv[] = {
		strdup("sperf"),
		strdup("ls"),
		strdup("-l"),
		strdup("."),
	};
	int argc = sizeof(argv) / sizeof(argv[0]);
	auto sperf_args = ArgumentHandler::HandleArguments(argc, argv);
	// Check other arguments
	assert(!sperf_args.print_help);
	assert(!sperf_args.print_version);
	assert(sperf_args.error_message.empty());
	// Check executable name
	assert(sperf_args.executable_name == "ls");
	// Check executable arguments
	assert(sperf_args.executable_arguments.size() == 2);
	assert(sperf_args.executable_arguments[0] == "-l");
	assert(sperf_args.executable_arguments[1] == ".");
}