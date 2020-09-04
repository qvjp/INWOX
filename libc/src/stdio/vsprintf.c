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

/* libc/src/stdio/vsprintf.c.
 * 使用可变参数格式化输出到标准输出
 */

#include <stdarg.h>
#include <stdio.h>

static size_t vsprintf_callback(void *arg, const char *s, size_t length)
{
    char **buffer = arg;
    for (size_t i = 0; i < length; i++) {
        **buffer = s[i];
        (*buffer)++;
    }

    return length;
}

int vsprintf(char *restrict s, const char *restrict format, va_list vl)
{
    int result = vcbprintf((void *)&s, vsprintf_callback, format, vl);
    if (result >= 0) {
        *s = '\0';
    }
    return result;
}
