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

/* libc/src/unistd/execl.c
 * 执行程序
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

int execl(const char *path, const char *argv0, ...)
{
    int argc = 1;
    va_list ap;
    va_start(ap, argv0);
    while (va_arg(ap, char *)) {
        argc++;
    }
    va_end(ap);

    char **argv = malloc(sizeof(char *) * (argc + 1));
    va_start(ap, argv0);
    argv[0] = (char *)argv0;
    for (int i = 1; i < argc; i++) {
        argv[i] = va_arg(ap, char *);
    }
    argv[argc] = NULL;
    va_end(ap);
    execv(path, argv);
    free(argv);
    return -1;
}
