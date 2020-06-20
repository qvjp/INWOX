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
 * kernel/include/inwox/kernel/idt.h
 * 定义IDT
 */

#ifndef KERNEL_IDT_H__
#define KERNEL_IDT_H__

#include <stdint.h> /* uint16_t uint8_t uint32_t */

#define IDT_INTERRUPT_GATE 0xE
#define IDT_RING0 (0 << 5)
#define IDT_RING3 (3 << 5)
#define IDT_PRESENT (1 << 7)

/**
 *                               * IDT ENTRT *
 * |-------------------------------------------------------------------------|
 * |                     |        |        |                |                |
 * |     Offset          |  Attr  |  Zero  |    Selector    |     Offset     |
 * |      16:31          |        | 16:23  |      0:15      |      0:15      |
 * |                     |        |        |                |                |
 * |-------------------------------------------------------------------------|
 * |63                 48|47    40|39    32|31            16|15             0|
 * 
 * 如图，IDT每个条目都是64位，其中：
 * 0-15和48-63共32位，用来存放ISR的入口地址
 * 16-31共16位，用来存放指向GDT某个段的段选择子
 * 32-39共8位，没有被使用，全部设为0
 * 40-47共8位，是类型和属性位，下边具体分析：
 *
 *             * Attr Byte *
 * |-------------------------------------|
 * |    |       |    |                   |
 * | Pr |  DPL  | S  |       TYPE        |
 * |    |       |    |                   |
 * |-------------------------------------|
 * |    7       5    4    3    2    1   0|
 *
 * TYPE共四位，在32位模式中，TYPE可能的设置有
 *     0101 表示IDT中此条是一个task gate
 *     1110 表示IDT中此条是一个interrupt gate
 *     1111 表示IDT中此条是一个trap gate
 * S（Storage Segment）设置为0
 * DPL即Descriptor Privilege Level，指定调用此描述符所属最高特权级，不被用户空间直接使用，设置为0即可
 * PR设置为0表示未使用
 */

/**
 * 定义IDT入口，指定packed是为了阻止编译器“优化”
 */
struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attr;
    uint16_t offset_high;
} __attribute__((__packed__));

/**
 *                      * IDT Descriptor*
 * |------------------------------------------------------------|
 * |                                       |                    |
 * |               Offset                  |      Size          |
 * |                                       |                    |
 * |------------------------------------------------------------|
 * |47                                   16|15                 0|
 *
 * 如上图，IDTR和GDTR一样，共48位，分为两部分，其中
 * 0-15位是Size，是IDT的大小
 * 16-47位是偏移量，也就是IDT的线性地址
 */

/**
 * 定义指向IDT的结构，包括IDT的大小和IDT的首地址，和设置GDT时一样
 * 同样这里设置为packed
 */
struct idt_desc
{
    uint16_t size;
    void* offset;
} __attribute__((__packed__));

/**
 * 设置IDT中每一个入口
 * num: 中断号
 * offset: 中断服务程序入口偏移
 * selector: 中断服务程序所在段
 * attr: 权限
 */
void idt_set_gate(uint8_t num, uint32_t offset, uint16_t selector, uint8_t attr);


#endif /* KERNEL_IDT_H__ */ 
