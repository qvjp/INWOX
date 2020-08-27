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
 * lib/include/sys/types.h
 * 数据类型定义
 */

#include <inwox/types.h>

#if defined(__need_FILE) && !defined(__FILE_defined)
typedef struct __FILE FILE;
#  define __FILE_defined
#endif

#if defined(__need_fpos_t) && !defined(__fpos_t_defined)
typedef __off_t fpos_t;
#  define __fpos_t_defined
#endif

#if defined(__need_mode_t) && !defined(__mode_t_defined)
typedef __mode_t mode_t;
#  define __mode_t_defined
#endif

#if defined(__need_off_t) && !defined(__off_t_defined)
typedef __off_t off_t;
#  define __off_t_defined
#endif

#if defined(__need_pid_t) && !defined(__pid_t_defined)
typedef __pid_t pid_t;
#  define __pid_t_defined
#endif

#if defined(__need_size_t) || defined(__need_NULL)
#  include <stddef.h>
#endif

#if defined(__need_ssize_t) && !defined(__ssize_t_defined)
typedef __SSIZE_TYPE__ ssize_t;
#  define __ssize_t_defined
#endif

#undef __need_FILE
#undef __need_fpos_t
#undef __need_mode_t
#undef __need_off_t
#undef __need_pid_t
#undef __need_ssize_t
