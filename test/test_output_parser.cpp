#include <testkit.h>
#include <output_parser.hpp>

UnitTest(Test_ParseSingleLine)
{
	auto [name, time] = OutputParser::ParseSingleLine("read(3, \"Hello, World!\", 13) = 13 <0.000123>");
	assert(name == "read");
	assert(time == 0.000123);

	auto [name2, time2] = OutputParser::ParseSingleLine("write(1, \"Hello, World!\", 13) = 13 <0.000456>");
	assert(name2 == "write");
	assert(time2 == 0.000456);
}