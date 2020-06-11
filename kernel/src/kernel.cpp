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
 * kernel/src/kernel.cpp
 * 内核main函数
 */

#include <stddef.h>                     /* size_t */
#include <stdint.h>                     /* uint8_t */
#include <inwox/kernel/addressspace.h>  /**/
#include <inwox/kernel/inwox.h>         /* MULTIBOOT_BOOTLOADER_MAGIC */
#include <inwox/kernel/interrupt.h>     /* Interrupt::initPic() Interrupt::enable() */
#include <inwox/kernel/physicalmemory.h>
#include <inwox/kernel/print.h>         /* printf() */
#include <inwox/kernel/process.h>
#include <stdlib.h>                     /* malloc() free() */

static void processA()
{
    static int a = 0;
    while (1)
    {
        Print::printf("A:%x\n", &a);
    }
}

static void processB()
{
    static int b = 0;
    while (1)
    {
        Print::printf("B:%x\n", &b);
    }
}

extern "C" void kernel_main(uint32_t magic, inwox_phy_addr_t multibootAddress)
{
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        Print::warnTerminal();
        Print::printf("Invalid magic number: 0x%x\n", magic);
        return;
    }

    Print::initTerminal();
    Print::printf("HELLO WORLD!\n");

    AddressSpace::initialize();
    Print::printf("AddressSpace Initialized\n");

    multiboot_info* multiboot = (multiboot_info*)kernelSpace->map(multibootAddress, 0x3);
    PhysicalMemory::initialize(multiboot);
    kernelSpace->unMap((inwox_vir_addr_t)multiboot);
    Print::printf("Physical Memory Initialized\n");


    Interrupt::initPic();
    Interrupt::enable();
    Print::printf("Interrrupt Initialized\n");

    Process::initialize();
    Print::printf("Processes Initialized\n");

    Process::startProcess((void*) processA);
    Process::startProcess((void*) processB);

    while(1)
    {
        /* 暂停CPU，当中断到来再开始执行 */
        __asm__ __volatile__ ("hlt");
    }
}