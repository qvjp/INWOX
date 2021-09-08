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
 * libc/src/stdlib/unsetenv.c
 * unset 环境变量
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;
extern char **__mallocedEnviron;
extern size_t __environLength;

int unsetenv(const char *name)
{
    if (!*name || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }
    if (!environ) {
        return 0;
    }
    size_t name_length = strlen(name);
    for (size_t i = 0; environ[i]; i++) {
        if (name_length == strcspn(environ[i], "=") &&
        strncmp(environ[i], name, name_length) == 0) {
            if (environ == __mallocedEnviron) {
                free(environ[i]);
                environ[i] = environ[--__environLength];
                environ[__environLength] = NULL;
            } else {
                for (size_t j = i; environ[j]; j++) {
                    environ[j] = environ[j + 1];
                }
            }
        }
    }
    return 0;
}
