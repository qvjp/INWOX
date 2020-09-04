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

#include <errno.h>
#include <stdlib.h> /* malloc */
#include <string.h> /* memset memcpy */
#include <inwox/kernel/elf.h>            /* ElfHeader ProgramHeader */
#include <inwox/kernel/physicalmemory.h> /* popPageFrame */
#include <inwox/kernel/print.h>          /* printf */
#include <inwox/kernel/process.h>
#include <inwox/kernel/terminal.h>

Process *Process::current;
static Process *firstProcess;
static Process *idleProcess;

/**
 * 这里的进程我们只指一个程序的基本可执行实体，并不代表线程的容器（区别于现代面向线程设计的系统）。
 * 当前的INWOX中进程控制块比较简单，包括一个独立的地址空间、运行上下文、指向下一个进程的指针和内核堆栈、用户堆栈
 */
Process::Process()
{
    addressSpace = kernelSpace;
    interruptContext = nullptr;
    next = nullptr;
    prev = nullptr;
    stack = nullptr;
    kstack = nullptr;
    memset(fd, 0, sizeof(fd));
    rootFd = nullptr;
    cwdFd = nullptr;
}

/**
 * 进程初始化
 * 创建空闲进程（没有其他任务执行时执行的进程）
 */
void Process::initialize(FileDescription *rootFd)
{
    idleProcess = (Process *)malloc(sizeof(Process));
    idleProcess->addressSpace = kernelSpace;
    idleProcess->next = 0;
    idleProcess->prev = 0;
    idleProcess->kstack = 0;
    idleProcess->stack = 0;
    idleProcess->rootFd = rootFd;
    idleProcess->interruptContext = (struct regs *)malloc(sizeof(struct regs));
    current = idleProcess;
    firstProcess = nullptr;
}

/**
 * 加载ELF可执行文件并运行
 *
 * 加载ELF可执行文件的过程：将p_vaddr开始p_memsz长的内存设为0，然后从p_offset
 * 开始复制p_filesz个字节到p_vaddr。
 */
Process *Process::loadELF(inwox_vir_addr_t elf)
{
    struct ElfHeader *header = (struct ElfHeader *)elf;
    if (check_elf_magic(header)) {
        Print::printf("Elf Header Incorrect\n");
        return NULL;
    }
    struct ProgramHeader *programHeader = (struct ProgramHeader *)(elf + header->e_phoff);

    AddressSpace *addressSpace = kernelSpace->fork();

    for (size_t i = 0; i < header->e_phnum; i++) {
        if (programHeader[i].p_type != PT_LOAD) {
            continue;
        }
        inwox_vir_addr_t loadAddressAligned = programHeader[i].p_paddr & ~0xFFF;
        ptrdiff_t offset = programHeader[i].p_paddr - loadAddressAligned;
        const void *src = (void *)(elf + programHeader[i].p_offset);
        size_t pages_number = ALIGN_UP(programHeader[i].p_memsz + offset, 0x1000) / 0x1000;
        inwox_phy_addr_t destPhys[pages_number + 1];
        /* 到物理内存申请pages_number个页的内存 */
        for (size_t j = 0; j < pages_number; j++) {
            destPhys[j] = PhysicalMemory::popPageFrame();
        }
        destPhys[pages_number] = 0;
        /* 将申请到的物理内存映射到连续虚拟内存 */
        inwox_vir_addr_t dest = kernelSpace->mapRange(destPhys, PAGE_PRESENT | PAGE_WRITABLE);
        /* 将申请到的虚拟内存保存的内容全部设为0 */
        memset((void *)(dest + offset), 0, programHeader[i].p_memsz);
        /* 将p_offset开始，长度为p_filesz的内容复制到目标内存 */
        memcpy((void *)(dest + offset), src, programHeader[i].p_filesz);
        /* 取消映射 */
        kernelSpace->unMapRange(dest, pages_number);
        /* 将已经复制好内容的物理内存映射到虚拟内存 p_paddr */
        addressSpace->mapRangeAt(loadAddressAligned, destPhys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }
    return startProcess((void *)header->e_entry, addressSpace);
}

/**
 * 进程调度函数
 *
 * 若当前正在执行的进程next域有其他进程，那么返回这个进程
 * 若没有其他进程，执行空闲进程
 */
struct regs *Process::schedule(struct regs *context)
{
    current->interruptContext = context;
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

/**
 * 开始新进程
 * 传入地址空间是为能在调用处确定内核态或用户态
 * 最后返回新创建的进程
 */
Process *Process::startProcess(void *entry, AddressSpace *addressSpace)
{
    Process *process = (Process *)malloc(sizeof(Process));
    process->prev = 0;
    process->next = 0;
    /**
     * 分配两个栈，内核栈用来保存上下文信息
     * 用户栈处理其他
     */
    process->kstack = (void *)kernelSpace->allocate(1);
    process->stack = (void *)addressSpace->allocate(1);

    process->interruptContext = (struct regs *)((uintptr_t)process->kstack + 0x1000 - sizeof(struct regs));
    process->interruptContext->eax = 0;
    process->interruptContext->ebx = 0;
    process->interruptContext->ecx = 0;
    process->interruptContext->edx = 0;
    process->interruptContext->esi = 0;
    process->interruptContext->edi = 0;
    process->interruptContext->ebp = 0;
    process->interruptContext->int_no = 0;
    process->interruptContext->err_code = 0;
    process->interruptContext->eip = (uint32_t)entry;
    process->interruptContext->cs = 0x1B;      /* 用户代码段是0x18 因为在ring3，
                                                * 按Intel规定，要使用(0x18 | 0x3)
                                                * 下边作为段选择子偏移ss的0x23也是
                                                * 一样道理。
                                                */
    process->interruptContext->eflags = 0x200; /* 开启中断 */
    process->interruptContext->useresp = (uint32_t)process->stack + 0x1000;
    process->interruptContext->ss = 0x23; /* 用户数据段 */

    process->addressSpace = addressSpace;
    process->fd[0] = new FileDescription(&terminal); /* 文件描述符0指向 stdin */
    process->fd[1] = new FileDescription(&terminal); /* 文件描述符1指向 stdout */
    process->fd[2] = new FileDescription(&terminal); /* 文件描述符2指向 stderr */
    process->rootFd = idleProcess->rootFd;
    process->cwdFd = process->rootFd;
    process->next = firstProcess;
    if (process->next) {
        process->next->prev = process;
    }
    firstProcess = process;
    return process;
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

    Print::printf("Process exited with status: %d\n", status);
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
