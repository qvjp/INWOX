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
 * kernel/include/inwox/kernel/process.h
 * 定义进程控制块和进程的操作方法
 */
#ifndef KERNEL_PROCESS_H__
#define KERNEL_PROCESS_H__

#include <inwox/kernel/addressspace.h> /* AddressSpace s*/
#include <inwox/kernel/interrupt.h>    /* struct regs */

class Process
{
public:
    Process();
private:
    AddressSpace* addressSpace;       /* 每个进程都有自己独立的地址空间 */
    struct regs* interruptContext;
    Process* next;
    void* kstack;
public:
    static void initialize();
    static struct regs* schedule(struct regs* context);
    static Process* startProcess(void* entry);
};

#endif