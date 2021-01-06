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

/* kernel/include/inwox/fork.h
 * fork头文件
 */

#ifndef INWOX_FORK_H_
#define INWOX_FORK_H_

#include <stdint.h>

// 下边宏定义为rfork的flags参数，参考Plan9/FreeBSD的rfork
// https://9fans.github.io/plan9port/man/man3/rfork.html
#define RFPROC  (1 << 0) // 如果设置此位，则创建新进程，否则操作原进程
#define RFFDG   (1 << 1) // 如果设置此位，则复制文件描述符，否则使用共享的同一个fd表
#define _RFFORK (RFPROC | RFFDG)

struct regfork {
    uint32_t rf_esp;
    uint32_t rf_eip;
    uint32_t rf_eax;
    uint32_t rf_ebx;
    uint32_t rf_ecx;
    uint32_t rf_edx;
    uint32_t rf_esi;
    uint32_t rf_edi;
    uint32_t rf_ebp;
};

#endif /* INWOX_FORK_H_ */
