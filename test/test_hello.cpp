#include <testkit.h>
#include <iostream>
#include <format>
#include <project_info.hpp>

UnitTest(Test_Hello)
{
	std::cout << std::format("👾 Your computer is hacked by @{}!\n", ProjectInfo::author);
}