/** MIT License
 *
 * Copyright (c) 2020 Qv Junping
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
 * libc/include/unistd.h
 * 定义POSIX函数
 */

#ifndef UNISTD_H
#define UNISTD_H

#define __need_pid_t
#define __need_ssize_t
#define __need_size_t
#define __need_FILE
#include <sys/types.h>
#include <inwox/fork.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief access的第二个参数
 *
 * 以下权限可以进行或操作
 */
#define F_OK 0        // 测试存在
#define R_OK (1 << 0) // 测试读权限
#define W_OK (1 << 1) // 测试写权限
#define X_OK (1 << 2) // 测试执行权限

__attribute__((__noreturn__)) void _exit(int);
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
int close(int);
int access(const char *, int);

pid_t fork(void);
pid_t rfork(int);
int execl(const char *, const char *, ...);
int execv(const char *, char *const[]);
int execve(const char *, char *const[], char *const[]);
int execvp(const char *, char *const[]);

unsigned int sleep(unsigned int);
#ifdef __cplusplus
}
#endif

#endif /* UNISTD_H */
