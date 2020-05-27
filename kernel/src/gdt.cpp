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
 * kernel/src/gdt.cpp
 * 定义GDT
 */

#include <inwox/kernel/gdt.h>

/**
 * x86支持两种虚拟内存方案
 * 分段 必须有的，用GDT管理
 * 分页 可选
 * 
 * 我们的操作系统打算使用分页，但分段不可以也不能关闭
 * 所以这里的解决方法是创建2个大的互相重叠的段（和NULL segment一起）
 * 这些段都是从0x00000000开始到0xFFFFFFFF，分别是特权数据段和特权代码段（kernel）
 * 后期还会增加非特权数据段和非特权代码段（userspace）
 * 这样将打开虚拟内存并相当于禁用了段的效果
 */
struct gdt_entry gdt[] = {
    /* NUll Segment */
    GDT_ENTRY(0,0,0,0),

    /* Kernel Code Segment access: 10011010, flags: 1100
     * 第二个段是代码段，基地址是0，最大地址单元4GBytes，粒度是4Kbyte，使用32位保护模式
     */
    GDT_ENTRY(0, 0xFFFFFFFF,GDT_PRESENT | GDT_RING0 | GDT_EXECUTABLE | GDT_READ_WRITE,
              GDT_GRANULARITY_4K | GDT_PROTECTED_MODE),

    /* Kernel Data Segment access: 10010010, flags: 1100
     * 第三个是数据段，和代码段类似
     */
    GDT_ENTRY(0, 0xFFFFFFFF, GDT_PRESENT | GDT_RING0 | GDT_READ_WRITE,
              GDT_GRANULARITY_4K | GDT_PROTECTED_MODE),
};

/**
 * 汇编代码lgdt将会加载这里
 */
struct gdt_desc gdt_descriptor = 
{
    sizeof(gdt) - 1,
    gdt
};
