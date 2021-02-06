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

/* kernel/src/uname.cpp
 * 系统信息
 */

#include <errno.h>
#include <string.h>
#include <inwox/conf.h>
#include <inwox/kernel/syscall.h>

#define SYSNAME "INWOX"

#ifndef INWOX_VERSION
#define INWOX_VERSION "unknown"
#endif

#ifdef __i386__
#define MACHINE "i686"
#else
#error "Unknown architecture."
#endif

static const char *getConfStr(int name)
{
    switch (name) {
        case _UNAME_SYSNAME: return SYSNAME;
        case _UNAME_RELEASE: return INWOX_VERSION;
        case _UNAME_VERSION: return __DATE__ " " __TIME__;
        case _UNAME_MACHINE: return MACHINE;
        default: return "";
    }
}

int Syscall::uname(struct utsname *uname)
{
    memset(uname->sysname, 0, _UNAME_LENGTH);
    memset(uname->release, 0, _UNAME_LENGTH);
    memset(uname->version, 0, _UNAME_LENGTH);
    memset(uname->nodename, 0, _UNAME_LENGTH);
    memset(uname->machine, 0, _UNAME_LENGTH);
    memcpy(uname->sysname, getConfStr(_UNAME_SYSNAME), sizeof(uname->sysname));
    memcpy(uname->release, getConfStr(_UNAME_RELEASE), sizeof(uname->release));
    memcpy(uname->version, getConfStr(_UNAME_VERSION), sizeof(uname->version));
    memcpy(uname->machine, getConfStr(_UNAME_MACHINE), sizeof(uname->machine));
    return 0;
}
