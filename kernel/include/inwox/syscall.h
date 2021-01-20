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
 * kernel/include/inwox/syscall.h
 * 系统调用号
 */

#ifndef INWOX_SYSCALL_H_
#define INWOX_SYSCALL_H_

#define SYSCALL_PAD       0
#define SYSCALL_EXIT      1
#define SYSCALL_WRITE     2
#define SYSCALL_READ      3
#define SYSCALL_MMAP      4
#define SYSCALL_MUNMAP    5
#define SYSCALL_OPENAT    6
#define SYSCALL_CLOSE     7
#define SYSCALL_REGFORK   8
#define SYSCALL_EXECVE    9
#define SYSCALL_WAITPID   10
#define SYSCALL_FSTATAT   11
#define SYSCALL_READDIR   12
#define SYSCALL_NANOSLEEP 13
#define SYSCALL_TCGETATTR 14
#define SYSCALL_TCSETATTR 15
#define SYSCALL_FCHDIRAT  16

#define NUM_SYSCALLS 17

#endif /* INWOX_SYSCALL_H_ */
