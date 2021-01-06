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
 * 时间相关声明
 */

#ifndef TIME_H
#define TIME_H

#define __need_clock_t
#define __need_locale_t
#define __need_NULL
#define __need_size_t
#define __need_time_t
#if __USE_INWOX || __USE_POSIX
#define __need_clockid_t
#define __need_timer_t
#endif
#include <sys/types.h>
#include <inwox/timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __USE_INWOX || __USE_POSIX
int nanosleep(const struct timespec *, struct timespec *);
#endif

#ifdef __cplusplus
}
#endif

#endif /* TIME_H */
