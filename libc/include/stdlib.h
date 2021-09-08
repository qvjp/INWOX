/** MIT License
 *
 * Copyright (c) 2020 - 2021 Qv Junping
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
 * lib/include/stdlib.h
 * 标准库定义
 */

#ifndef STDLIB_H
#define STDLIB_H

#define __need_size_t
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX (0x7fffffff) // rand()可生成的最大随机数

char *canonicalize_file_name(const char *);

__attribute__((__noreturn__)) void _Exit(int);
__attribute__((__noreturn__)) void exit(int);

void free(void *);
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
long strtol(const char *__restrict, char **__restrict, int);
unsigned long strtoul(const char *__restrict, char **__restrict, int);
int rand(void);
int rand_r(unsigned int *);
void srand(unsigned int);

__attribute__((__noreturn__)) void abort(void);
int atexit(void (*)(void));
int atoi(const char *);
char *getenv(const char *);
int setenv(const char *, const char *, int);
int unsetenv(const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STDLIB_H */
