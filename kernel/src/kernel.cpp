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

/* GCC提供的头文件 */
#include <stddef.h> /* size_t */
#include <stdint.h> /* uint8_t */

extern "C" void kernel_main(uint32_t /*magic*/, uintptr_t /*multiboot*/)
{
    /* 0xB8000是x86的VGA文本模式缓存地址，要显示文字，向这个内存地址写数据即可 */
    uint8_t *video = (uint8_t*)0xB8000;
    const char *hello = "Hello World! From INWOX";

    /* 初始化终端，将其设置为灰色背景 */

    /*
     * 每个VGA buffer都是BBBBFFFFCCCCCCCC的结构
     * B是背景色
     * F是前景色
     * C是ASCII字符
     */

    /**
     * 文本模式颜色常量
     *
     * BLACK = 0,
     * BLUE = 1,
     * GREEN = 2,
     * CYAN = 3,
     * RED = 4,
     * MAGENTA = 5,
     * BROWN = 6,
     * LIGHT_GREY = 7,
     * DARK_GREY = 8,
     * LIGHT_BLUE = 9,
     * LIGHT_GREEN = 10,
     * LIGHT_CYAN = 11,
     * LIGHT_RED = 12,
     * LIGHT_MAGENTA = 13,
     * LIGHT_BROWN = 14,
     * WHITE = 15,
     */
    for (size_t i = 0; i < 25; i++)
        for (size_t j =0; j < 80; j++)
        {
            video[2 * (80 * i + j)] = ' ';
            video[2 * (80 * i + j) + 1] = 0x77;
        }

    /* 字符串长度23是硬编码了，后边这里会重构 */
    for (size_t i = 0; i < 23; i++)
    {
        video[2 * i] = hello[i];
        /* 灰底蓝字 */
        video[2 * i + 1] = 0x79;
    }
}
