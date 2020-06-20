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
 * kernel/src/arch/i686/interrrupt.s
 * 处理中断，保存现场调用ISRs恢复现场
 */

.section .text

/* 处理没有错误码的ISR */
.macro isr no
.global isr_\no
isr_\no:
    cli
    push $0 /* 对于没有错误码的异常，push一个0是为了统一栈帧 */
    push $\no
    jmp isr_commonHandler
.endm

/* 处理有错误码的ISR */
.macro isr_error_code no
.global isr_\no
isr_\no:
    cli
    /* 有错误码的isr就不需要手动push了，已经自动push进错误码了 */
    push $\no
    jmp isr_commonHandler
.endm

/**
 * 当ISR发生调用时，保存当前运行状态，
 * 设置内核模式段，调用C语言编写的错误处理程序，
 * 最后回到之前的栈帧
 */

isr_commonHandler:
    pusha                   /* push eax, ecx, edx, ebx, esp, ebp, esi, edi. */

    /* 切换到内核数据段处理中断 */
    mov $0x10, %ax
    mov %ax, %ds

    push %esp               /* interrupt_handler的参数，指向刚pusha后的栈d顶 */
    call interrupt_handler  /* 调用C语言编写的中断处理程序 */

    /* 恢复现场的顺序和保存现场的顺序正好相反 */
    mov %eax, %esp          /* eax是interrupt_handler的返回值，也就是newContext */

    /* 处理完中断切换到用户数据段 */
    mov $0x23, %ax          /* 0x23 = 0x18 | 0x3 即 3*8 | 011b*/
    mov %ax, %ds

    popa                    /* pop edi, esi, ebp, esp, ebx, edx, ecx, eax */

    add $8, %esp            /* 从堆栈中移除错误码和ISR号 */
    iret                    /* pop cs, eip, eflags, ss, esp */

/* Exception */
isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr_error_code 8
isr 9
isr_error_code 10
isr_error_code 11
isr_error_code 12
isr_error_code 13
isr_error_code 14
isr 15
isr 16
isr 17
isr 18
isr 19
isr 20
isr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
isr 29
isr 30
isr 31

/* IRQs */
isr 32
isr 33
isr 34
isr 35
isr 36
isr 37
isr 38
isr 39
isr 40
isr 41
isr 42
isr 43
isr 44
isr 45
isr 46
isr 47

isr 48 # Padding
isr 49 # Schedule

# isr 73 # Syscall