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
 * lib/include/stdio.h
 * 标准输入输出定义
 */

#ifndef STDIO_H__
#define STDIO_H__

#define __need___va_list
#include <stdarg.h>
#define __need_FILE
#define __need_size_t
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FILE* stderr;
#define stderr stderr

/* 编译GCC需要此函数 */
int fflush(FILE*);
int fprintf(FILE* __restrict, const char* __restrict, ...);

/* 编译GCC需要此函数 */
#define SEEK_SET 1
int fclose(FILE*);
FILE* fopen(const char* __restrict, const char* __restrict);
size_t fread(void* __restrict, size_t, size_t, FILE* __restrict);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void* __restrict, size_t, size_t, FILE* __restrict);
void setbuf(FILE* __restrict, char* __restrict);
int vfprintf(FILE* __restrict, const char* __restrict, __gnuc_va_list);

#ifdef __cplusplus
}
#endif

#endif
