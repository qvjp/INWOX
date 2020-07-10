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
 * kernel/src/print.cpp
 * 格式化输出至屏幕
 */

#include <stdarg.h> /* va_* */
#include <stdio.h>
#include <stdint.h> /* uint8_t uint32_t */
#include <inwox/kernel/print.h>
#include <inwox/kernel/terminal.h>

static size_t print_callback(void*, const char* string, size_t length)
{
    return (size_t) terminal.write(string, length);
}

void Print::printf(const char* format, ...)
{
    /*
     * 在这里写一下C语言中可变参数的基本用法
     * 
     * 其中va是指variable-argument(可变参数)
     * va_list是一个arg_ptr,指向参数的指针
     * va_start()初始化va_list对象
     * va_arg()返回参数指针当前指向的值
     * va_end()和va_start()对应，调用过va_start后就必须调用va_end
     */
    va_list vl;
    va_start(vl, format);
    vcbprintf(nullptr, print_callback, format, vl);
    va_end(vl);
}

void Print::warnTerminal()
{
    terminal.warnTerminal();
}

void Print::initTerminal()
{
    terminal.initTerminal();
}
