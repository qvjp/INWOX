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
 * libc/src/arch/i686/rfork.s
 * rfork -- manipulate process resources，参考Plan9
 * https://9fans.github.io/plan9port/man/man3/rfork.html
 */

.global rfork
.type rfork, @function
rfork:
    push %ebp
    mov %esp, %ebp
    sub $12, %esp

    # 这里需要解释一下，fork的实现原理
    # 下边主要是将内核中regfork()需要的参数进行设置，具体对应如下：
    # regfork    operation
    # rf_ebp     push %ebp
    # rf_edi     push %edi
    # rf_esi     push %esi
    # rf_edx     push %edx
    # rf_ecx     push %ecx
    # rf_ebx     push %ebx
    # rf_eax     push $0
    # rf_eip     push $afterFork
    # rf_esp     push 0
    # 很明显，代码将子进程返回值（%eax）设为0，并将下一条命令（%eip）设为
    # $afterFork即直接返回，而父进程则继续向下执行，最终返回sys_fork返回的子进程id
    push %ebp
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx
    # 子进程eax为0
    push $0
    # 子进程eip指向此处
    push $afterFork
    # 设置子进程esp为0
    push $0

    push %esp
    push 8(%ebp)

    call sys_fork
afterFork:
    mov %ebp, %esp
    pop %ebp
    ret
.size rfork, . - rfork
