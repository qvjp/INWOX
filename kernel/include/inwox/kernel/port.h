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
 * kernel/include/inwox/kernel/port.h
 * 向I/O读写数据
 */
#ifndef KERNEL_PORT_H__
#define KERNEL_PORT_H__

#include <stdint.h> /* uint16_t uint8_t */

namespace Hardwarecommunication
{
    /**
     * x86处理器有两套独立的地址空间---内存地址空间和I/O地址空间
     * I/O地址空间的读写用到汇编OUT/IN
     * 发送8/16/32位的值到I/O地址，分别用outb/outw/outl
     */
    static inline void outportb(uint16_t port, uint8_t value)
    {
        __asm__ __volatile__ ("outb %0, %1"
                               :
                               : "a"(value), "Nd"(port));
    }
}

#endif /* KERNEL_PORT_H__ */ 
