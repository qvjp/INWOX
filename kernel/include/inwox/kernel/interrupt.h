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
 * kernel/include/inwox/kernel/interrupt.h
 * 定义中断处理程序，开启中断等
 */
#ifndef KERNEL_INTERRUPT_H
#define KERNEL_INTERRUPT_H

#include <stdint.h> /* uint32_t */


/**
 * 异常提示信息
 */
const char* const exception_messages[] =
{
    "Devide-by-zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "x87 Floating-Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point",
    "Virtualization",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/**
 * Exceptions 处理CPU异常的方法
 * F-Fault
 * T-Trap
 * A-Abort
 **/
extern "C"{
                                 /* Type    Descriptipn                                 Error Code */
    extern void isr_0(void);     /* F       Devide-by-zero Exception                    NO         */
    extern void isr_1(void);     /* F/T     Debug Exception                             NO         */
    extern void isr_2(void);     /* I       Non-maskable Interrupt                      NO         */
    extern void isr_3(void);     /* T       Breakpoint Exception                        NO         */
    extern void isr_4(void);     /* T       Into Detected Overflow Exception            NO         */
    extern void isr_5(void);     /* F       Bound Range Exceeded Exception              NO         */
    extern void isr_6(void);     /* F       Invalid Opcode Exception                    NO         */
    extern void isr_7(void);     /* F       No Coprocessor Exception                    NO         */
    extern void isr_8(void);     /* A       Double Fault Exception                      YES        */
    extern void isr_9(void);     /* F       Coprocessor Segment Overrun Exception       No         */
    extern void isr_10(void);    /* F       Invalid TSS Exception                       Yes        */
    extern void isr_11(void);    /* F       Segment Not Present Exception               Yes        */
    extern void isr_12(void);    /* F       Stack-Segment Fault Exception               Yes        */
    extern void isr_13(void);    /* F       General Protection Fault Exception          Yes        */
    extern void isr_14(void);    /* F       Page Fault Exception                        Yes        */
    extern void isr_15(void);    /*         Unknown Interrupt Exception                 No         */
    extern void isr_16(void);    /* F       x87 Floating-Point Exception                No         */
    extern void isr_17(void);    /* F       Alignment Check Exception                   Yes        */
    extern void isr_18(void);    /* A       Machine Check                               No         */
    extern void isr_19(void);    /* F       SIMD Floating-Point Exception               No         */
    extern void isr_20(void);    /* F       Virtualization Exception                    NO         */
    extern void isr_21(void);    /* Reserved Exception */
    extern void isr_22(void);    /* Reserved Exception */
    extern void isr_23(void);    /* Reserved Exception */
    extern void isr_24(void);    /* Reserved Exception */
    extern void isr_25(void);    /* Reserved Exception */
    extern void isr_26(void);    /* Reserved Exception */
    extern void isr_27(void);    /* Reserved Exception */
    extern void isr_28(void);    /* Reserved Exception */
    extern void isr_29(void);    /* Reserved Exception */
    extern void isr_30(void);    /* Reserved Exception */
    extern void isr_31(void);    /* Reserved Exception */


    /* IRQs 声明每个IRQ专门的处理程序 */
    extern void isr_32(void);      /* Programmable Interrupt Timer Interrupt */
    extern void isr_33(void);      /* Keyboard Interrupt */
    extern void isr_34(void);      /* Cascade (used internally by the two PICs. never raised) */
    extern void isr_35(void);      /* COM2 (if enabled) */
    extern void isr_36(void);      /* COM1 (if enabled) */
    extern void isr_37(void);      /* LPT2 (if enabled) */
    extern void isr_38(void);      /* Floppy Disk */
    extern void isr_39(void);      /* LPT1 / Unreliable "spurious" interrupt (usually) */
    extern void isr_40(void);      /* CMOS real-time clock (if enabled) */
    extern void isr_41(void);      /* Free for peripherals / legacy SCSI / NIC */
    extern void isr_42(void);      /* Free for peripherals / SCSI / NIC */
    extern void isr_43(void);      /* Free for peripherals / SCSI / NIC */
    extern void isr_44(void);      /* PS2 Mouse */
    extern void isr_45(void);      /* FPU / Coprocessor / Inter-processor */
    extern void isr_46(void);      /* Primary ATA Hard Disk */
    extern void isr_47(void);      /* Secondary ATA Hard Disk */
}
/**
 * 这个结构定义ISR运行后栈的样子
 */
struct regs
{
    /* uint32_t gs, fs, es, ds; */                          /* 最后push的几个寄存器 */
    /* 为不混淆视听，遂将几个段寄存器删掉，用时再说 */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* "pasha" 将数据压如这几个寄存器 */
    uint32_t int_no, err_code;                        /* 我们"push $\no"就是将错误吗push到int_no中 */
    uint32_t eip, cs, eflags, useresp, ss;            /* CPU自动压入 */
};

/* 将IRQs设置到IDT中 */
void irqs_install();

namespace Interrupt
{
    void initPic();
    void enable();
}

#endif /* KERNEL_INTERRUPTS_H */ 
