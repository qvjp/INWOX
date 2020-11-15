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
 * libc/src/arch/i686/syscall.s
 * 系统调用函数
 */

.section .text
.global __syscall
.type __syscall, @function
__syscall:
    push %ebp
    mov %esp, %ebp

    # 保存调用者的现存
    push %edi
    push %esi
    push %ebx
    sub $12, %esp

    # 将系统调用各参数放入合适寄存器
    mov 8(%ebp), %ebx
    mov 12(%ebp), %ecx
    mov 16(%ebp), %edx
    mov 20(%ebp), %esi
    mov 24(%ebp), %edi

    int $0x49

    # 错误码用ecx传递，如果不为0设置errno
    test %ecx, %ecx
    jz 1f
    mov %ecx, errno

1:  add $12, %esp
    pop %ebx
    pop %esi
    pop %edi
    pop %ebp
    ret
.size __syscall, . - __syscall
