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
 * libc/src/arch/i686/crtn.s
 *
 * System V ABI指定用5个文件一起控制程序的初始化，即crt0.o, crti.o
 * crtbegin.o, crtend.o, crtn.o, 这些文件共同实现了两个函数_init, _fini
 * 正如字面意思，这两个函数分别运行全局构造函数和其他初始化任务，_fini
 * 运行全局析构函数和其他终止任务
 *
 * GCC在编译时会自动将
 * i686-elf-gcc foo.o bar.o -o program重写成如下格式
 * i686-elf-ld crt0.o crti.o crtbegin.o foo.o bar.o crtend.o crtn.o
 * 其中crtbegin.o和crtend.o交叉编译器会提供
 * 本文件即是提供crti.o
 */
 
.section .init
    /** GCC将把crtend.o的.init section 放到这里
     *  调用全局构造函数
     */
    /* 恢复原来栈的内容 */
    pop %ebp
    ret

.section .fini
    /** GCC将把crtend.o的.fini section 放到这里
     *  调用全局析构函数
     */
    /* 同样是是恢复原来栈的内容 */
    pop %ebp
    ret
