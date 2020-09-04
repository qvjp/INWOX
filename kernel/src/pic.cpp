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
 * kernel/src/pic.cpp
 * 初始化PIC
 */

#include <inwox/kernel/pic.h>
#include <inwox/kernel/port.h> /* outportb() */

/**
 * 初始化PIC
 * 进入保护模式后，首先要给两个PIC一个初始化命令（向命令端口写入0x11）
 * 这将使PIC在数据端口等待3个额外的初始化设置，分别是：
 * 1. 向量偏移（ICW2）
 * 2. 主/从PIC如何连接（ICW3）
 * 3. 额外信息（ICW4）
 */
void pic_remap()
{
    /* 开始初始化序列 */
    Hardwarecommunication::outportb(PIC1_COMMAND, 0x11);
    Hardwarecommunication::outportb(PIC2_COMMAND, 0x11);

    /* 主PIC向量偏移0x20(也就是主PIC重新映射后是从INT的32-39)，从PIC向量偏移0x28(从PIC40-47) */
    Hardwarecommunication::outportb(PIC1_DATA, 0x20);
    Hardwarecommunication::outportb(PIC2_DATA, 0x28);

    /**
     * 告诉主PIC在IRQ2(0000 0100)有从PIC
     * 告诉从PIC它的层叠号吗(0000 0010)
     */
    Hardwarecommunication::outportb(PIC1_DATA, 0x04);
    Hardwarecommunication::outportb(PIC2_DATA, 0x02);

    /**
     * ICW4，额外信息
     */
    Hardwarecommunication::outportb(PIC1_DATA, 0x01);
    Hardwarecommunication::outportb(PIC2_DATA, 0x01);
}
