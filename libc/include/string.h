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
 * lib/include/stdlib.h
 * 标准库定义
 */

#ifndef STRING_H
#define STRING_H

#define __need_NULL
#define __need_size_t
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int memcmp(const void *, const void *, size_t);
void *memcpy(void *__restrict, const void *__restrict, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);

char *strchr(const char *, int);
char *stpcpy(char *__restrict, const char *__restrict);
char *strdup(const char *);
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
char *strcpy(char *__restrict, const char *__restrict);
int strcmp(const char *str1, const char *str2);
size_t strcspn(const char *, const char *);
int strncmp(const char *str1, const char *str2, size_t length);
char *strrchr(const char *, int);
size_t strspn(const char *, const char *);
char *strtok(char *__restrict, const char *__restrict);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STRING_H */
