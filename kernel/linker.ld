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
 * bootloader在系统镜像中找下边指出的入口
 */
ENTRY(_start)

/**
 * 指定各个段在最终镜像中的位置
 */
SECTIONS {
    /**
     * 从1M处开始放各种段，因为1M以内是x86保留的，我们不能用
     */
    . = 1M;

    /**
     * 首先放置multiboot header，因为bootloader只会在最开始的8KiB内找设置好的魔法数字
     * 接着放 .text section
     */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* 只读数据 */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* 可读可写数据（已被初始化）*/
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* 可读可写数据（未被初始化）和栈 */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
    /*
     * 编译器可能会生成其他的section，默认情况下，会将它们
     * 放在具有相同名称的段中。 只需根据需要添加内容即可。
     */
}
