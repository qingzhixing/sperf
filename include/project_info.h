#pragma once
#ifndef PROJECT_INFO_H
#define PROJECT_INFO_H

#include <string>
#include <format>
#include <iostream>

class ProjectInfo
{
public:
	static const inline std::string project_name = "sperf";
	static const inline std::string description = "一个命令行工具，它能启动另一个程序，并统计该程序中各个系统调用的占用时间。";
	static inline const std::string ascii_art =
		R"(
███████╗██████╗ ███████╗██████╗ ███████╗
██╔════╝██╔══██╗██╔════╝██╔══██╗██╔════╝
███████╗██████╔╝█████╗  ██████╔╝█████╗  
╚════██║██╔═══╝ ██╔══╝  ██╔══██╗██╔══╝  
███████║██║     ███████╗██║  ██║██║     
╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝╚═╝                                               
)";
	static inline const std::string author = "qingzhixing";
	static inline const std::string mail = "qzsqqmail@qq.com";
	static inline const std::string license = "MIT License";
	static inline const std::string github_link = "https://github.com/qingzhixing/sperf";

public:
	static void PrintVersion()
	{
		std::cout << std::format("🎃 {}: {}\n", project_name, description);
		std::cout << ascii_art << std::endl;
		std::cout << std::format("⌨️ Author: {}\n", author);
		std::cout << std::format("💌 Mail: {}\n", mail);
		std::cout << std::format("👾 License: {}\n", license);
		std::cout << std::format("📎 GitHub: {}\n", github_link);
	}

	static void PrintHelp()
	{
		std::cout << std::format("Usage: {} [OPTIONS] COMMAND [ARG]...\n", project_name);
		std::cout << "Options:\n";
		std::cout << "  -h, --help    Show this help message.\n";
		std::cout << "  -v, --version Show version information.\n";
		std::cout << "  COMMAND		  The command you want to analyze.\n";
		std::cout << "  ARG           Arguments for the command.\n";
		std::cout << "Example:\n";
		std::cout << std::format("  {} ls -l\n", project_name);
		std::cout << std::format("More information: {}\n", R"(https://jyywiki.cn/OS/2025/labs/M3.md)");
	}
};

#endif //! PROJECT_INFO_H