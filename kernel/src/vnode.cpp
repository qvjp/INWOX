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

/* kernel/src/vnode.cpp
 * Vnode class.
 */

#include <errno.h>
#include <inwox/kernel/vnode.h>

Vnode::Vnode(mode_t mode)
{
    this->mode = mode;
}

/* 默认实现，具体看继承函数如何实现 */
ssize_t Vnode::read(void * /* buffer */, size_t /* size */)
{
    errno = EBADF;
    return -1;
}

ssize_t Vnode::write(const void * /* buffer */, size_t /* size */)
{
    errno = EBADF;
    return -1;
}

bool Vnode::isSeekable()
{
    return false;
}

Vnode *Vnode::openat(const char * /* path */, int /* flags */, mode_t /* mode */)
{
    errno = ENOTDIR;
    return nullptr;
}

ssize_t Vnode::pread(void * /* buffer */, size_t /* size */, off_t /* offset */)
{
    errno = EBADF;
    return -1;
}

int Vnode::stat(struct stat *result)
{
    result->st_mode = mode;
    return 0;
}
