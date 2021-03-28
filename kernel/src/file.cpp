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

/* kernel/src/file.cpp
 * FileVnode class.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <inwox/kernel/file.h>
#include <inwox/stat.h>

FileVnode::FileVnode(const void *data, size_t size, mode_t mode) : Vnode(S_IFREG | mode)
{
    this->data = new char[size];
    memcpy(this->data, data, size);
    fileSize = size;
    mutex = KTHREAD_MUTEX_INITIALIZER;
}

FileVnode::~FileVnode()
{
    delete data;
}

int FileVnode::ftruncate(off_t length)
{
    if (length < 0 || length > __SIZE_MAX__) {
        errno = EINVAL;
        return -1;
    }
    ScopedLock lock(&mutex);
    void *newData = realloc(data, (size_t)length);
    if (!newData) {
        errno = ENOSPC;
        return -1;
    }
    data = (char *)newData;
    if (length > fileSize) {
        memset(data + fileSize, '\0', length - fileSize);
    }
    fileSize = (size_t)length;
    return 0;
}

bool FileVnode::isSeekable()
{
    return true;
}

ssize_t FileVnode::pread(void *buffer, size_t size, off_t offset)
{
    ScopedLock lock(&mutex);
    char *buf = (char *)buffer;

    for (size_t i = 0; i < size; i++) {
        if (offset + i >= fileSize) {
            return i;
        }
        buf[i] = data[offset + i];
    }

    return size;
}

ssize_t FileVnode::pwrite(const void *buffer, size_t size, off_t offset)
{
    if (offset < 0) {
        errno = EINVAL;
        return -1;
    }
    ScopedLock lock(&mutex);
    size_t newSize;
    if (__builtin_add_overflow(offset, size, &newSize)) {
        errno = ENOSPC;
        return -1;
    }
    if (newSize > fileSize) {
        void *newData = realloc(data, newSize);
        if (!newData) {
            errno = ENOSPC;
            return -1;
        }
        data = (char *)newData;
        fileSize = newSize;
    }
    memcpy(data + offset, buffer, size);
    return size;
}
