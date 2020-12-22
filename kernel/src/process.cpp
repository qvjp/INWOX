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
 * 实现INWOX中的进程
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h> /* malloc */
#include <string.h> /* memset memcpy */
#include <sys/stat.h>
#include <inwox/kernel/elf.h>            /* ElfHeader ProgramHeader */
#include <inwox/kernel/file.h>
#include <inwox/kernel/physicalmemory.h> /* popPageFrame */
#include <inwox/kernel/print.h>          /* printf */
#include <inwox/kernel/process.h>
#include <inwox/kernel/terminal.h>

Process *Process::current;
static Process *firstProcess;
static Process *idleProcess;
static pid_t nextPid = 0;
/**
 * 这里的进程我们只指一个程序的基本可执行实体，并不代表线程的容器（区别于现代面向线程设计的系统）。
 * 当前的INWOX中进程控制块比较简单，包括一个独立的地址空间、运行上下文、指向下一个进程的指针和内核堆栈、用户堆栈
 */
Process::Process()
{
    addressSpace = nullptr;
    interruptContext = nullptr;
    next = nullptr;
    prev = nullptr;
    kstack = nullptr;
    memset(fd, 0, sizeof(fd));
    rootFd = nullptr;
    cwdFd = nullptr;
    pid = nextPid++;
    contextChanged = false;
    fdInitialized = false;
    terminated = false;
    parent = nullptr;
    children = nullptr;
    numChildren = 0;
    status = 0;
}

Process::~Process()
{
    assert(terminated);
    kernelSpace->unmapMemory((inwox_vir_addr_t) kstack, 0x1000);
    free(children);
}

/**
 * 进程初始化
 * 创建空闲进程（没有其他任务执行时执行的进程）
 */
void Process::initialize(FileDescription *rootFd)
{
    idleProcess = new Process();
    idleProcess->addressSpace = kernelSpace;
    idleProcess->rootFd = rootFd;
    idleProcess->interruptContext = new regs();
    current = idleProcess;
    firstProcess = nullptr;
}

void Process::addProcess(Process *process)
{
    process->next = firstProcess;
    if (process->next) {
        process->next->prev = process;
    }
    firstProcess = process;
}

uintptr_t Process::loadELF(uintptr_t elf)
{
    struct ElfHeader *header = (struct ElfHeader *)elf;
    if (check_elf_magic(header)) {
        errno = EACCES;
        return -1;
    }
    struct ProgramHeader *programHeader = (struct ProgramHeader *)(elf + header->e_phoff);

    addressSpace = new AddressSpace();
    for (size_t i = 0; i < header->e_phnum; i++) {
        if (programHeader[i].p_type != PT_LOAD) {
            continue;
        }
        inwox_vir_addr_t loadAddressAligned = programHeader[i].p_paddr & ~0xFFF;
        ptrdiff_t offset = programHeader[i].p_paddr - loadAddressAligned;
        const void *src = (void *)(elf + programHeader[i].p_offset);
        size_t size = ALIGN_UP(programHeader[i].p_memsz + offset, 0x1000);
        /* 将申请到的物理内存映射到连续虚拟内存 */
        addressSpace->mapMemory(loadAddressAligned, size, PROT_READ | PROT_WRITE | PROT_EXEC);
        inwox_vir_addr_t dest = kernelSpace->mapFromOtherAddressSpace(addressSpace, loadAddressAligned, size, PROT_WRITE);
        /* 将申请到的虚拟内存保存的内容全部设为0 */
        memset((void *)(dest + offset), 0, programHeader[i].p_memsz);
        /* 将p_offset开始，长度为p_filesz的内容复制到目标内存 */
        memcpy((void *)(dest + offset), src, programHeader[i].p_filesz);
        /* 取消映射 */
        kernelSpace->unmapPhysical(dest, size);
    }
    return (uintptr_t) header->e_entry;
}

/**
 * 进程调度函数
 *
 * 若当前正在执行的进程next域有其他进程，那么返回这个进程
 * 若没有其他进程，执行空闲进程
 */
struct regs *Process::schedule(struct regs *context)
{
    if (likely(!current->contextChanged)) {
        current->interruptContext = context;
    } else {
        current->contextChanged = false;
    }
    if (current->next) {
        current = current->next;
    } else {
        if (firstProcess) {
            current = firstProcess;
        } else {
            current = idleProcess;
        }
    }
    setKernelStack((uintptr_t)current->kstack + 0x1000);
    current->addressSpace->activate();
    return current->interruptContext;
}

int Process::copyArguments(char *const argv[], char *const envp[], char **&newArgv, char **&newEnvp)
{
    int argc = 0;
    int envc = 0;
    size_t stringSizes = 0;
    for (argc = 0; argv[argc]; argc++) {
        stringSizes += strlen(argv[argc]) + 1;
    }
    for (envc = 0; envp[envc]; envc++) {
        stringSizes += strlen(envp[envc]) + 1;
    }
    stringSizes = ALIGN_UP(stringSizes, alignof(char*));
    size_t size = ALIGN_UP(stringSizes + (argc + envc + 2) * sizeof(char *), 0x1000);
    inwox_vir_addr_t page = addressSpace->mapMemory(size, PROT_READ | PROT_WRITE);
    inwox_vir_addr_t pageMapped = kernelSpace->mapFromOtherAddressSpace(addressSpace, page, size, PROT_WRITE);
    char *nextString = (char *) pageMapped;
    char **argvMapped = (char **) (pageMapped + stringSizes);
    char **envpMapped = argvMapped + argc + 1;

    for (int i = 0; i < argc; i++) {
        argvMapped[i] = nextString - pageMapped + page;
        nextString = stpcpy(nextString, argv[i]) + 1;
    }
    for (int i = 0; i < envc; i++) {
        envpMapped[i] = nextString;
        nextString = stpcpy(nextString, envp[i]) + 1;
    }
    argvMapped[argc] = nullptr;
    envpMapped[envc] = nullptr;
    kernelSpace->unmapPhysical(pageMapped, size);

    newArgv = (char **) (page + stringSizes);
    newEnvp = (char **) ((char*)newArgv + (argc + 1) * sizeof(char *));
    return argc;
}

int Process::execute(FileDescription *descr, char *const argv[], char *const envp[])
{
    AddressSpace *oldAddressSpace = addressSpace;
    FileVnode *file = (FileVnode *) descr->vnode;
    if (!S_ISREG(descr->vnode->mode)) {
        errno = EACCES;
        return -1;
    }
    uintptr_t entry = loadELF((uintptr_t) file->data);
    if ((int)entry == -1) {
        errno = ENOEXEC;
        return -1;
    }
    /**
     * 分配两个栈，内核栈用来保存上下文信息
     * 用户栈处理其他
     */
    kstack = (void*)kernelSpace->mapMemory(0x1000, PROT_READ | PROT_WRITE);
    inwox_vir_addr_t stack = addressSpace->mapMemory(0x1000, PROT_READ | PROT_WRITE);

    interruptContext = (struct regs *)((uintptr_t)kstack + 0x1000 - sizeof(struct regs));
    memset(interruptContext, 0, sizeof(struct regs));
    char **newArgv;
    char **newEnvp;
    int argc = copyArguments(argv, envp, newArgv, newEnvp);
    interruptContext->eax = argc;
    interruptContext->ebx = (uint32_t) newArgv;
    interruptContext->ecx = (uint32_t) newEnvp;
    interruptContext->eip = (uint32_t)entry;
    interruptContext->cs = 0x1B;      /* 用户代码段是0x18 因为在ring3，
                                                * 按Intel规定，要使用(0x18 | 0x3)
                                                * 下边作为段选择子偏移ss的0x23也是
                                                * 一样道理。
                                                */
    interruptContext->eflags = 0x200; /* 开启中断 */
    interruptContext->useresp = stack + 0x1000;
    interruptContext->ss = 0x23; /* 用户数据段 */
    if (!fdInitialized) {
        fd[0] = new FileDescription(&terminal); /* 文件描述符0指向 stdin */
        fd[1] = new FileDescription(&terminal); /* 文件描述符1指向 stdout */
        fd[2] = new FileDescription(&terminal); /* 文件描述符2指向 stderr */
        rootFd = new FileDescription(*idleProcess->rootFd);
        cwdFd = new FileDescription(*rootFd);
        fdInitialized = true;
    }
    if (this == current) {
        contextChanged = true;
        kernelSpace->activate();
    }
    if (oldAddressSpace) {
        delete oldAddressSpace;
    }
    return 0;
}

/**
 * 进程退出
 * TODO：回收资源
 */
void Process::exit(int status)
{
    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }
    if (this == firstProcess) {
        firstProcess = next;
    }

    delete addressSpace;
    for (size_t i = 0; i < OPEN_MAX; i++) {
        if (fd[i]) {
            delete fd[i];
        }
    }
    delete rootFd;
    delete cwdFd;
    terminated = true;
    this->status = status;

    Print::printf("Process %u exited with status: %d\n", pid, status);
}

/**
 * 进程的fork
 * 创建新进程，分配新的内核栈，在新内核栈底部保存父进程寄存器信息，
 * 然后复制父进程地址空间，复制文件描述符，并将新进程加入进程列表等待调度
 */
Process *Process::regfork(int flags, struct regfork *registers)
{
    (void) flags;
    Process *process = new Process();
    process->parent = this;
    children = (Process**) realloc(children, ++numChildren * sizeof(Process*));
    children[numChildren - 1] = process;

    // fork 寄存器
    process->kstack = (void*) kernelSpace->mapMemory(0x1000, PROT_READ | PROT_WRITE);
    process->interruptContext = (struct regs*)((uintptr_t) process->kstack + 0x1000 - sizeof(struct regs));
    process->interruptContext->eax = registers->rf_eax;
    process->interruptContext->ebx = registers->rf_ebx;
    process->interruptContext->ecx = registers->rf_ecx;
    process->interruptContext->edx = registers->rf_edx;
    process->interruptContext->esi = registers->rf_esi;
    process->interruptContext->edi = registers->rf_edi;
    process->interruptContext->ebp = registers->rf_ebp;
    process->interruptContext->eip = registers->rf_eip;
    process->interruptContext->esp = registers->rf_esp;

    process->interruptContext->int_no = 0;
    process->interruptContext->err_code = 0;
    process->interruptContext->cs = 0x1B;
    process->interruptContext->eflags = 0x200;
    process->interruptContext->ss = 0x23;

    // fork地址空间
    process->addressSpace = addressSpace->fork();

    // fork文件描述符
    for (size_t i = 0; i < OPEN_MAX; i++) {
        if (fd[i]) {
            process->fd[i] = new FileDescription(*fd[i]);
        }
    }
    process->rootFd = new FileDescription(*rootFd);
    process->cwdFd = new FileDescription(*cwdFd);
    process->fdInitialized = true;

    // 将进程加入进程链表
    addProcess(process);
    return process;
}

/**
 * 将文件注册给进程
 * 交给最小的可用文件描述符表示
 */
int Process::registerFileDescriptor(FileDescription *descr)
{
    for (int i = 0; i < 20; i++) {
        if (fd[i] == nullptr) {
            fd[i] = descr;
            return i;
        }
    }

    errno = EMFILE;
    return -1;
}

Process *Process::waitpid(pid_t pid, int flags)
{
    if(flags != 0) {
        errno = EINVAL;
        return nullptr;
    }

    for (size_t i = 0; i < numChildren; i++) {
        if (children[i]->pid == pid) {
            Process *result = children[i];
            // 等待进程停止
            while (!result->terminated) {
                __asm__ __volatile__("int $49");
                __sync_synchronize();
            }
            if (i < numChildren - 1) {
                children[i] = children[numChildren - 1];
            }
            children = (Process**) realloc(children, --numChildren * sizeof(Process*));
            return result;
        }
    }
    errno = ECHILD;
    return nullptr;
}
