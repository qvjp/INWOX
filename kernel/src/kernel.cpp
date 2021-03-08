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
 * kernel/src/kernel.cpp
 * 内核main函数
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/directory.h>
#include <inwox/kernel/file.h>
#include <inwox/kernel/initrd.h>
#include <inwox/kernel/inwox.h>
#include <inwox/kernel/interrupt.h>
#include <inwox/kernel/physicalmemory.h>
#include <inwox/kernel/pit.h>
#include <inwox/kernel/ps2.h>
#include <inwox/kernel/print.h>
#include <inwox/kernel/process.h>
#include <inwox/kernel/terminal.h>

#ifndef INWOX_VERSION
#define INWOX_VERSION ""
#endif

static multiboot_info multiboot;

/**
 * 从multiboot信息中解析initrd
 *
 * 先从multiboot的mod信息中找到模块首地址并给它分配虚拟地址
 * 然后通过mod_start和mod_end解析每一个模块，并将模块的首地址
 * 传给Initrd::loadInitrd。最终释放过程中使用的虚拟地址。
 */
static DirectoryVnode *loadInitrd(multiboot_info *multiboot)
{
    DirectoryVnode *root = nullptr;
    inwox_phy_addr_t modulesAligned = multiboot->mods_addr & ~0xFFF;
    ptrdiff_t offset = multiboot->mods_addr - modulesAligned;
    size_t mappedSize = ALIGN_UP(offset + multiboot->mods_count * sizeof(multiboot_mod_list), PAGESIZE);
    inwox_vir_addr_t modulesPage = kernelSpace->mapPhysical(modulesAligned, mappedSize, PROT_READ);

    /* *modules是真正要处理模块的首地址 */
    const struct multiboot_mod_list *modules = (struct multiboot_mod_list *)(modulesPage + offset);
    for (size_t i = 0; i < multiboot->mods_count; i++) {
        /* 按页对齐后再分配内存 */
        size_t size = ALIGN_UP(modules[i].mod_end - modules[i].mod_start, PAGESIZE);
        inwox_vir_addr_t initrd = kernelSpace->mapPhysical(modules[i].mod_start, size, PROT_READ);
        root = Initrd::loadInitrd(initrd);
        kernelSpace->unmapPhysical(initrd, size);

        if (root->childCount) {
            break;
        }
    }
    kernelSpace->unmapPhysical((inwox_vir_addr_t)modulesPage, mappedSize);
    return root;
}

/**
 * @brief 内核入口函数
 * 
 * @param magic multiboot兼容引导程序传递来的魔数
 * @param multibootAddress  multiboot的起始地址
 * 
 * 上述两个参数均由kernel_main的调用方loader.s保证正确传入，在kernel_main中做校验，然后通过multiboot所
 * 提供信息进行初始化，初始化的内容包括：
 * 1. 初始化终端，当前为字符模式，主要将终端清屏并设置预定的背景色
 * 2. 初始存储
 *    1. 初始化地址空间
 *    2. 初始化物理内存
 * 3. 初始化PS2控制器
 * 4. 加载initrd
 * 5. 初始化进程
 * 6. 运行shell
 * 7. 设置中断
 */
extern "C" void kernel_main(uint32_t magic, inwox_phy_addr_t multibootAddress)
{
    terminal.initTerminal();
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        terminal.warnTerminal();
        Print::printf("Invalid magic number: 0x%x\n", magic);
        return;
    }

    Print::printf("Welcome to INWOX " INWOX_VERSION "\n");
    Print::printf("Initializing AddressSpace...\n");
    AddressSpace::initialize();

    Print::printf("Initializing Physical Memory...\n");
    multiboot_info *multibootMapped = (multiboot_info *)kernelSpace->mapPhysical(multibootAddress, PAGESIZE, PROT_READ);
    memcpy(&multiboot, multibootMapped, sizeof(multiboot_info));
    kernelSpace->unmapPhysical((inwox_vir_addr_t)multibootMapped, PAGESIZE);

    PhysicalMemory::initialize(&multiboot);

    Print::printf("Initializing PS/2 Controller...\n");
    PS2::initialize();

    Print::printf("Loading initrd...\n");
    DirectoryVnode *rootDir = loadInitrd(&multiboot);
    FileDescription *rootFd = new FileDescription(rootDir);

    Print::printf("Initializing Process...\n");
    Process::initialize(rootFd);

    FileVnode *program = (FileVnode *)resolvePath(rootDir, "/bin/shell");
    if (program) {
        Print::printf("Launching shell...\n");
        Process *newProcess = new Process();
        const char *argv[] = {"/bin/sh", nullptr};
        const char *envp[] = {"PATH=/bin", nullptr};
        newProcess->execute(program, (char **)argv, (char **)envp);
        Process::addProcess(newProcess);
    } else {
        Print::printf("launch shell failed\n");
    }

    Print::printf("Initializing Interrupt...\n");
    Interrupt::initPic();
    Pit::initialize();
    Interrupt::enable();

    Print::printf("Initialization completed!\n");

    while (1) {
        /* 暂停CPU，当中断到来再开始执行 */
        __asm__ __volatile__("hlt");
    }
}
