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
  * kernel/linker.ld
  * 加载内核到1MB位置，1M以下部分被BIOS和GRUB占用。
  */

/**
 * bootloader在系统镜像中找下边指出的入口
 */
ENTRY(_start);

 /**
  * 内核虚拟空间开始位置
  */
KERNEL_VIRTUAL = 0xC0000000;

SECTIONS {
    /**
     * 从1M处开始放我们的各种段，因为1M以内是x86保留的，我们不能用
     */
    . = 1M;
    bootstrapBegin = .;

    bootstrap_text : ALIGN(4K)
    {
        /* 首先放置multiboot header，因为bootloader只会在最开始的8K内找我们设置好的魔法数字 */
        *(.multiboot)
        *(bootstrap_text)
    }

    bootstrap_bss : ALIGN(4K)
    {
        *(bootstrap_bss)
    }

    bootstrapEnd = ALIGN(4K);
    numBootstrapPages = (bootstrapEnd - bootstrapBegin) / 4K;

    /* 内核0xC0100000开始 */
    . += KERNEL_VIRTUAL;
    kernelVirtualBegin = ALIGN(4K);
    kernelPhysicalBegin = kernelVirtualBegin - KERNEL_VIRTUAL;

    /* 4K（页大小）对齐，加载到1M处。 */
    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL)
    {
        *(.text)    /* 所有文件的所有text section */
    }

    /* 只读数据 4K对齐，加载到1M + .处。 */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL)
    {
        *(.rodata)  /* 所有文件的所有只读section */
    }

    /* 可读可写数据（已被初始化）4K对齐，加载到1M + .处。 */
    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL)
    {
        *(.data)    /* 所有文件的所有data section */
    }

    /* 可读可写数据（未被初始化）和栈 4K对齐，加载到1M + .处。 */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL)
    {
        *(COMMON)   /* 所有文件的所有COMMON section */
        *(.bss)     /* 所有文件的所有bss section */
    }

    kernelVirtualEnd = ALIGN(4K);
    kernelPhysicalEnd = kernelVirtualEnd - KERNEL_VIRTUAL;
    numKernelPages = (kernelVirtualEnd - kernelVirtualBegin) / 4K;
}
