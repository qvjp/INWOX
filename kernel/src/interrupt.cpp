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
 * kernel/src/interrrupt.cpp
 * 处理中断
 */
#include <inwox/kernel/idt.h>       /* idt_set_gate() IDT_INTERRUPT_GATE IDT_RING0 IDT_PRESENT*/
#include <inwox/kernel/interrupt.h>
#include <inwox/kernel/pic.h>       /* pic_remap() PIC1_COMMAND ... */
#include <inwox/kernel/port.h>      /* outportb() */
#include <inwox/kernel/print.h>     /* printf() warnTerminal() */

/**
 * 挨个设置IDT中的ISR
 */
void irqs_install()
{
    /* Exceptions */
    idt_set_gate(0, (uint32_t)isr_0, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(1, (uint32_t)isr_1, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(2, (uint32_t)isr_2, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(3, (uint32_t)isr_3, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(4, (uint32_t)isr_4, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(5, (uint32_t)isr_5, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(6, (uint32_t)isr_6, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(7, (uint32_t)isr_7, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(8, (uint32_t)isr_8, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(9, (uint32_t)isr_9, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(10, (uint32_t)isr_10, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(11, (uint32_t)isr_11, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(12, (uint32_t)isr_12, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(13, (uint32_t)isr_13, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(14, (uint32_t)isr_14, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(15, (uint32_t)isr_15, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(16, (uint32_t)isr_16, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(17, (uint32_t)isr_17, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(18, (uint32_t)isr_18, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(19, (uint32_t)isr_19, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(20, (uint32_t)isr_20, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(21, (uint32_t)isr_21, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(22, (uint32_t)isr_22, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(23, (uint32_t)isr_23, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(24, (uint32_t)isr_24, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(25, (uint32_t)isr_25, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(26, (uint32_t)isr_26, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(27, (uint32_t)isr_27, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(28, (uint32_t)isr_28, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(29, (uint32_t)isr_29, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(30, (uint32_t)isr_30, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(31, (uint32_t)isr_31, 0x8, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    /* IRQs */
    idt_set_gate(32, (unsigned)isr_32, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(33, (unsigned)isr_33, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(34, (unsigned)isr_34, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(35, (unsigned)isr_35, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(36, (unsigned)isr_36, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(37, (unsigned)isr_37, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(38, (unsigned)isr_38, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(39, (unsigned)isr_39, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(40, (unsigned)isr_40, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(41, (unsigned)isr_41, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(42, (unsigned)isr_42, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(43, (unsigned)isr_43, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(44, (unsigned)isr_44, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(45, (unsigned)isr_45, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(46, (unsigned)isr_46, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
    idt_set_gate(47, (unsigned)isr_47, 0x08, IDT_INTERRUPT_GATE | IDT_RING0 | IDT_PRESENT);
}

/**
 * 开启中断
 */
void Interrupt::enable()
{
    __asm__ __volatile__ ("sti");
}

void (*isr_routines[256])(regs *) =
{
    0
};

void isr_install_handler(int isr, void (*handler)(struct regs *r))
{
    isr_routines[isr] = handler;
}

void isr_uninstall_handler(int isr)
{
    isr_routines[isr] = 0;
}

void Interrupt::initPic()
{
    /**
    * 先重新映射了IRQ在IDT中的位置，再在IDT正确的位置放适当的IRQ处理程序
    */
    pic_remap();
    irqs_install();
}

extern "C" void interrupt_handler(struct regs *r)
{
    /**
     * 我们所有的ISR都将通过这个方法进入具体的处理程序
     * 现在只是简单的打印异常信息并通过一个死循环停止系统运行
    */
    if (r->int_no < 32)
    {
        Print::warnTerminal();
        Print::printf("eax: 0x%x, ebx: 0x%x, ecx: 0x%x, edx: 0x%x\n", r->eax, r->ebx, r->ecx, r->edx);
        Print::printf("edi: 0x%x, esi: 0x%x, ebp: 0x%x, esp: 0x%x\n", r->edi, r->esi, r->ebp, r->esp);
        Print::printf("cs: 0x%x, eip: 0x%x, eflags: 0x%x, ss: 0x%x\n", r->cs, r->eip, r->eflags, r->ss);
        Print::printf("%s", exception_messages[r->int_no]);
        Print::printf(" Exception. System Halted!\n");
        while(1);
    }

    if (r->int_no <= 47 && r->int_no >= 32)
    {
        /**
         * 定义一个空函数指针用来放具体的IRQ处理程序
         */
        void (*handler)(struct regs *r);
        if (r->int_no != 32)
        {
            Print::printf("IRQ %d occurred!\n", r->int_no - 32);
        }
        handler = isr_routines[r->int_no];
        if (handler)
        {
            handler(r);
        }
        /**
         * 中断处理结束后，要发送EOI(End Of Interrupt)给PIC的命令端口
         * 如果IDT入口号大于40(IRQ8-15)，也就是说这个IRQ来自从PIC，我们需要给从PIC的命令端口发送一个EOI 
         * 而如果IRQ来自主PIC，我们只需要向主PIC发送EOI即可
         */
        if (r->int_no >= 40)
        {
            Hardwarecommunication::outportb(PIC2_COMMAND, PIC_EOI);
        }
        Hardwarecommunication::outportb(PIC1_COMMAND, PIC_EOI);
    }
    else
    {
        Print::printf("Unknow interrupt %u!\n", r->int_no);
    }
}
