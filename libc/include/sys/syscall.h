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
 * lib/include/sys/syscall.h
 * 定义系统调用section
 */

#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H

#include <inwox/syscall.h>

/**
 * 以系统调用read为例
 * DEFINE_SYSCALL_GLOBAL(SYSCALL_READ, ssize_t, read, (int, void *, size_t));
 * 最终展开为：
 * 
 * .global read
 * .pushsection .text
 * read:
 *     mov $3, %eax
 *     jmp __syscall
 * .popsection
 * extern ssize_t read (int, void *, size_t)
 */
#define _SYSCALL_TOSTRING(x) #x

#define _SYSCALL_BODY(number, name)                    \
    asm("\n"                                           \
    ".pushsection .text\n"                             \
    #name ":\n\t"                                      \
        "mov $" _SYSCALL_TOSTRING(number) ", %eax\n\t" \
        "jmp __syscall\n"                              \
    ".popsection\n")

#define DEFINE_SYSCALL(number, type, name, params) \
    _SYSCALL_BODY(number, name);                   \
    extern type name params

#define DEFINE_SYSCALL_GLOBAL(number, type, name, params) \
    asm(".global " #name);                                \
    DEFINE_SYSCALL(number, type, name, params)

#endif /* SYS_SYSCALL_H */
