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
 * kernel/src/process.cpp
 * 系统调用
 */

#include <errno.h>
#include <sched.h>
#include <sys/stat.h>
#include <inwox/fcntl.h>
#include <inwox/kernel/addressspace.h>
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
    (void*) Syscall::fchdirat,
    (void*) Syscall::uname,
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

/**
 * @brief 获取文件句柄
 * 
 * @param fd 若path不以'/'开头，则获取文件描述符fd对应的文件句柄
 * @param path 若以'/'开头，获取系统根目录文件句柄
 * @return FileDescription* 文件句柄
 */
static FileDescription *getRootFd(int fd, const char *path)
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
    sched_yield();
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
    FileDescription *result = descr->openat(path, flags, mode & ~Process::current->umask);
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
    FileDescription *descr = getRootFd(AT_FDCWD, path);
    Vnode *vnode = resolvePath(descr->vnode, path);
    if (!vnode || Process::current->execute(vnode, argv, envp) == -1) {
        return -1;
    }
    sched_yield();
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
    Vnode *vnode = resolvePath(descr->vnode, path);
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
 * @brief 修改调用进程的工作路径
 * 
 * @param dirfd 同其他*at函数一致，表明相对此文件描述符dirfd解析后边的path
 * @param path 修改后的路径
 * @return int 返回0成功，-1失败
 */
int Syscall::fchdirat(int dirfd, const char *path)
{
    FileDescription *descr = getRootFd(dirfd, path);
    FileDescription *newCwd = descr->openat(path, 0, 0);
    if (!newCwd) {
        return -1;
    }
    if (!S_ISDIR(newCwd->vnode->mode)) {
        errno = ENOTDIR;
        return -1;
    }
    delete Process::current->cwdFd;
    Process::current->cwdFd = newCwd;
    return 0;
}


/**
 * @brief 系统调用mmap实现函数
 * 
 * @param addr 如果为NULL，由内核选择新映射所在的虚拟地址位置（页面对齐），若不为NULL，则将addr作为参考，在其附近（页面对齐）处找合适位置开始映射
 * @param size 要映射区域的长度（必须大于0）
 * @param protection 描述了映射的内存保护模式（不能与打开的文件冲突）可以指定为如下几个权限或它们或运算的结果
 *            PROT_EXEC  页可以被执行
 *            PROT_READ  也可以被读取
 *            PROT_WRITE 页可以被写入
 *            PROT_NONE  页无权访问
 * @param flags 确定对此段内存的操作是否对映射同一区域的其他进程可见、或其他控制操作
 *            MAP_PRIVATE 创建一个私有的、写时复制的映射，映射的更新对映射同一文件的其他进程不可见
 *            MAP_SHARED 共享映射，映射的修改对其他映射相同文件的进程可见
 *            MAP_ANONYMOUS 没有提供待映射的文件，分配的内存将初始化为0，参数fd、offset忽略
 * @param fd 如果指定fd（否则flags为MAP_ANONYMOUS），将使用fd表示的文件的offset偏移处开始的长度为size的内容映射到内存
 * @param offset 同fd一起使用，文件偏移位置，页面对齐
 * @return void* 新映射的虚拟地址
 */
static void *mmapImplementation(void * /*addr*/, size_t size, int protection, int flags, int /*fd*/, off_t /*offset*/)
{
    // 目前只能分配私有内存
    if (size <= 0 || !(flags & MAP_PRIVATE)) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    // 对于匿名映射，在当前进程的地址空间分配一块指定大小、保护模式的内存
    if (flags & MAP_ANONYMOUS) {
        AddressSpace *addressSpace = Process::current->addressSpace;
        return (void *)addressSpace->mapMemory(size, protection);
    }

    // 实现其他flags
    errno = ENOTSUP;
    return MAP_FAILED;
}

/**
 * @brief 系统调用mmap
 * 
 * mmap即把文件的连续一段映射为一段连续内存，文件是通用概念，甚至也可以不指定，具体见实现函数mmapImplementation
 * 
 * @param request 参数见mmapImplementation描述
 * @return void* 返回新映射的虚拟地址
 */
void *Syscall::mmap(__mmapRequest *request)
{
    return mmapImplementation(request->_addr, request->_size, request->_protection, request->_flags, request->_fd,
                              request->_offset);
}

/**
 * @brief 系统调用munmap
 * 
 * 取消对一块内存的映射，再次访问该地址会产生错误。进程终止时会自动取消映射，关闭文件描述符不会取消
 * 
 * @param addr 开始地址，必须是页面的整数倍
 * @param size 待取消映射的内存长度，不需要页对齐
 * @return int 成功返回0，失败-1
 */
int Syscall::munmap(void *addr, size_t size)
{
    if (size == 0 || (inwox_vir_addr_t)addr & 0xFFF) {
        errno = EINVAL;
        return -1;
    }

    AddressSpace *addressSpace = Process::current->addressSpace;
    /* TODO: The userspace process could unmap kernel pages! */
    addressSpace->unmapMemory((inwox_vir_addr_t)addr, size);
    return 0;
}

/**
 * INWOX不能处理的系统调用
 */
void Syscall::badSyscall()
{
    Print::printf("Syscall::badSyscall was called\n");
}
