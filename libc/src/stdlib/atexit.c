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
 * libc/src/stdlib/atexit.c
 * 注册一个在进程正常退出时执行的函数
 */
#define __need_ssize_t
#include <stdlib.h>
#include <sys/types.h>

#define ATEXIT_MAX 32
static void (*atexitHandlers[ATEXIT_MAX])(void);

int atexit(void (*func)(void)) {
    for (size_t i = 0; i < ATEXIT_MAX; i++) {
        if (!atexitHandlers[i]) {
            atexitHandlers[i] = func;
            return 0;
        }
    }
    return -1;
}

void __callAtexitHandlers(void) {
    for (ssize_t i = ATEXIT_MAX - 1; i >= 0; i--) {
        if (atexitHandlers[i]) {
            atexitHandlers[i]();
        }
    }
}
