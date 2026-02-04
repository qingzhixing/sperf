#!/usr/bin/env python3
import os
import subprocess
import sys
import argparse


def main():
    # 打印脚本运行信息
    print("✨ 测试脚本开始运行...")
    
    # 构建项目
    if subprocess.run("xmake f -m debug && xmake", shell=True).returncode != 0:
        print("✖️ 构建失败")
        sys.exit(1)
    print("✅ 构建成功")
    
    
    print("✨ 运行Testkit...")
    
    # 设置环境变量
    env = os.environ.copy()
    env["TK_VERBOSE"] = "1"
    # 运行测试,运行目录: ./build
    subprocess.run(["./sperf"], env=env, cwd="./build", check=False)
    
    print("✅ Testkit运行结束")
    
    
    print("✨ 运行valgrind...")
    subprocess.run(["valgrind", "--tool=memcheck",
                    "--show-leak-kinds=all", "--leak-check=full",
                    "-q",
                    "./sperf","ls","./"], cwd="./build", check=False)
    
    print("✅ valgrind运行结束")

    print("✨ 脚本运行完毕!")


if __name__ == "__main__":
    main()
