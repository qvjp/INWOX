/** MIT License
 *
 * Copyright (c) 2020 - 2021 Qv Junping
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * kernel/include/inwox/kernel/syscall.h
 * 系统调用函数声明
 */

#ifndef KERNEL_SYSCALL_H_
#define KERNEL_SYSCALL_H_

#define __need_ssize_t
#define __need_mode_t
#define __need_pid_t
#include <sys/types.h>
#include <sys/utsname.h>
#include <inwox/fork.h>
#include <inwox/syscall.h>
#include <inwox/timespec.h>

struct __mmapRequest;
namespace Syscall {
void pad(void);
__attribute__((__noreturn__)) void exit(int status);
ssize_t read(int fd, void *buffer, size_t size);
ssize_t write(int fd, const void *buffer, size_t size);
void *mmap(__mmapRequest *request);
int munmap(void *addr, size_t size);
int openat(int fd, const char *path, int flags, mode_t mode);
int close(int fd);
pid_t regfork(int flags, struct regfork *registers);
int execve(const char *path, char *const argv[], char *const envp[]);
pid_t waitpid(pid_t pid, int *status, int flags);
int fstat(int fd, struct stat *result);
int fstatat(int fd, const char *__restrict path, struct stat *__restrict result, int flags);
ssize_t readdir(int fd, unsigned long offset, void *buffer, size_t size);
int nanosleep(const struct timespec *request, struct timespec *remaining);
int tcgetattr(int fd, struct termios *result);
int tcsetattr(int fd, int flags, const struct termios *termio);
int fchdirat(int dirfd, const char *path);
int uname(struct utsname *uname);
void badSyscall();
} /* namespace Syscall */

#endif /* KERNEL_SYSCALL_H_ */
