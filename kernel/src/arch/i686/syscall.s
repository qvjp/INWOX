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
 * kernel/src/arch/i686/syscall.s
 * 系统调用Handler
 */
.section .text
.global syscallHandler
.type syscallHandler, @function
syscallHandler:

    push %ebx       /* 系统调用参数 %ebx %eax是通过*/
    push %eax       /* 调用处int $73" :: "a"(1), "b"(0)
                     * 规定的*ax和*bx
                     */

    mov $0x10, %cx  /* 内核数据段 */
    mov %cx, %ds

    call getSyscallHandler

    add $4, %esp    /* %esp加4是为了将栈顶原素指向前边push的%ebx，作为具体系统调用的参数 */
    call *%eax      /* 系统调用处理程序 */

    mov $0x23, %cx  /* 切换回用户段 */
    mov %cx, %ds

    iret            /* 返回用户态 */

.size syscallHandler, . - syscallHandler
