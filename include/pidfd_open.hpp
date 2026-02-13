#pragma once
#ifndef PIDFD_OPEN_H
#define PIDFD_OPEN_H

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

// 我的glibc 没有 pidfd_open 函数，所以我需要自己实现一个
inline int pidfd_open(__pid_t pid, unsigned int flags)
{
	return syscall(__NR_pidfd_open, pid, flags);
}

#endif //! PIDFD_OPEN_H
