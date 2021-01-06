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
 * kernel/src/process.cpp
 * 系统调用
 */

#include <errno.h>
#include <inwox/fcntl.h>
#include <inwox/kernel/print.h>
#include <inwox/kernel/process.h>
#include <inwox/kernel/syscall.h>
#include <inwox/kernel/timer.h>

/**
 * 系统调用表
 */
static const void *syscallList[NUM_SYSCALLS] = {
    (void *)Syscall::pad,

    (void*) Syscall::exit,
    (void*) Syscall::write,
    (void*) Syscall::read,
    (void*) Syscall::mmap,
    (void*) Syscall::munmap,
    (void*) Syscall::openat,
    (void*) Syscall::close,
    (void*) Syscall::regfork,
    (void*) Syscall::execve,
    (void*) Syscall::waitpid,
    (void*) Syscall::fstatat,
    (void*) Syscall::readdir,
    (void*) Syscall::nanosleep,
    (void*) Syscall::tcgetattr,
    (void*) Syscall::tcsetattr,
};

/**
 * 通过系统调用号获取系统调用具体入口地址
 */
extern "C" const void *getSyscallHandler(unsigned interruptNum)
{
    if (interruptNum >= NUM_SYSCALLS)
        return (void *)Syscall::badSyscall;
    else
        return syscallList[interruptNum];
}

static FileDescription *getRootFd(int fd, const char *__restrict path)
{
    if (path[0] == '/') {
        return Process::current->rootFd;
    } else if (fd == AT_FDCWD) {
        return Process::current->cwdFd;
    } else {
        return Process::current->fd[fd];
    }
}

/**
 * 系统调用：pad()
 * 保留的0号系统调用
 */
void Syscall::pad()
{
    const char *INWOX = "INWOX";
    int num = 0;
    for (int i = 0; i < 5; i++) {
        num += INWOX[i] - 'A' + 1;
    }
    Print::printf("Pad: %d\n", num);
}

/**
 * 系统调用：exit()
 * 退出当前进程，通过调用当前进程的exit方法，并发起49号中断（调度中断）
 *
 * 设置为__attribute__((__noreturn__))说明此函数不会返回，控制权
 * 不会再返回调用者，而最后的__builtin_unreachable()则告诉GCC程序
 * 不会执行到那里，否则声明了没有返回的函数GCC会发出警告，要注意的是函数
 * 返回值void不是不返回，是没有返回“值”，还是会返回给调用者。
 */
__attribute__((__noreturn__)) void Syscall::exit(int status)
{
    Process::current->exit(status);
    /* 退出后，调用int $49进行进程调度 */
    __asm__ __volatile__("int $49");
    __builtin_unreachable();
}

ssize_t Syscall::read(int fd, void *buffer, size_t size)
{
    FileDescription *descr = Process::current->fd[fd];
    return descr->read(buffer, size);
}

ssize_t Syscall::write(int fd, const void *buffer, size_t size)
{
    FileDescription *descr = Process::current->fd[fd];
    return descr->write(buffer, size);
}

int Syscall::openat(int fd, const char *path, int flags, mode_t mode)
{
    FileDescription *descr = getRootFd(fd, path);
    FileDescription *result = descr->openat(path, flags, mode);
    if (!result) {
        return -1;
    }
    return Process::current->registerFileDescriptor(result);
}

int Syscall::close(int fd)
{
    FileDescription *fDescr = Process::current->fd[fd];
    if (!fDescr) {
        errno = EBADF;
        return -1;
    }
    delete fDescr;
    Process::current->fd[fd] = nullptr;
    return 0;
}

pid_t Syscall::regfork(int flags, struct regfork *registers)
{
    if (!((flags & RFPROC) && (flags & RFFDG))) {
        errno = EINVAL;
        return -1;
    }
    Process *newProcess = Process::current->regfork(flags, registers);
    return newProcess->pid;
}

int Syscall::execve(const char *path, char *const argv[], char *const envp[])
{
    if (path == NULL || path[0] == '\0') {
        return -1;
    }
    FileDescription *descr = Process::current->rootFd->openat(path, 0, 0);
    if (!descr || Process::current->execute(descr, argv, envp) == -1) {
        return -1;
    }
    __asm__ __volatile__("int $49");
    __builtin_unreachable();
}

pid_t Syscall::waitpid(pid_t pid, int *status, int flags)
{
    Process *process = Process::current->waitpid(pid, flags);
    if (!process) {
        return -1;
    }
    *status = process->status;
    delete process;
    return pid;
}

int Syscall::fstatat(int fd, const char *__restrict path, struct stat *__restrict result, int flags)
{
    (void)flags;
    FileDescription *descr = getRootFd(fd, path);
    Vnode *vnode = descr->vnode->openat(path, 0, 0);
    if (!vnode) {
        return -1;
    }
    return vnode->stat(result);
}

ssize_t Syscall::readdir(int fd, unsigned long offset, void *buffer, size_t size)
{
    FileDescription *descr = Process::current->fd[fd];
    return descr->readdir(offset, buffer, size);
}

int Syscall::tcgetattr(int fd, struct termios *result)
{
    FileDescription *descr = Process::current->fd[fd];
    return descr->tcgetattr(result);
}

int Syscall::tcsetattr(int fd, int flags, const struct termios *termio)
{
    FileDescription *descr = Process::current->fd[fd];
    return descr->tcsetattr(flags, termio);
}

/**
 * INWOX不能处理的系统调用
 */
void Syscall::badSyscall()
{
    Print::printf("Syscall::badSyscall was called\n");
}
