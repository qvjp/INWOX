/** MIT License
 *
 * Copyright (c) 2021 Qv Junping
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
 * libc/src/stdlib/setenv.c
 * 设置环境变量
 */

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;
extern char **__mallocedEnviron;
extern size_t __environLength;
extern size_t __environSize;

static bool reset_environ(void)
{
    if (__mallocedEnviron) {
        while (*__mallocedEnviron) {
            free(*__mallocedEnviron);
        }
        free(__mallocedEnviron);
    }
    size_t length = 0;
    if (environ) {
        char **envp = environ;
        while (*envp++) {
            length++;
        }
    }

    size_t allocationSize = length > 15 ? length + 1 : 16;
    __mallocedEnviron = malloc(allocationSize * sizeof(char **));
    if (!__mallocedEnviron) {
        return false;
    }
    __environLength = length;
    __environSize = allocationSize;
    for (size_t i = 0; i < length; i++) {
        __mallocedEnviron[i] = strdup(environ[i]);
    }
    __mallocedEnviron[length] = NULL;
    environ = __mallocedEnviron;
    return true;
}

int setenv(const char *name, const char * value, int overwrite)
{
    if (!*name || strchr(name, '=')) {
        errno = EINVAL;
        return -1;
    }

    if (!environ || environ != __mallocedEnviron) {
        if (!reset_environ()) {
            return -1;
        }
    }

    size_t nameLength = strlen(name);
    size_t i = 0;
    for (; environ[i]; i++) {
        if (nameLength == strcspn(environ[i], "=") &&
        strncmp(environ[i], name, nameLength) == 0) {
            if (!overwrite) {
                return 0;
            }
            size_t entryLength = nameLength + strlen(value) + 2;
            char *newEntry = malloc(entryLength);
            if (!newEntry) {
                return -1;
            }
            stpcpy(stpcpy(stpcpy(newEntry, name), "="), value);
            free(environ[i]);
            environ[i] = newEntry;
            return 0;
        }
    }
    if (__environLength + 1 == __environSize) {
        size_t resultSize;
        if (__builtin_mul_overflow(__environSize, 2 * sizeof(char*), &resultSize)) {
            errno = ENOMEM;
            return -1;
        }
        char **newEnviron = realloc(environ, resultSize);
        if (!newEnviron) {
            return -1;
        }
        environ = __mallocedEnviron = newEnviron;
        __environSize *= 2;
    }
    size_t entryLength = nameLength + strlen(value) + 2;
    environ[i] = malloc(entryLength);
    if (!environ[i]) return -1;
    stpcpy(stpcpy(stpcpy(environ[i], name), "="), value);
    environ[i + 1] = NULL;
    __environLength++;
    return 0;
}