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

#include <string.h>
#include <inwox/kernel/file.h>
#include <inwox/stat.h>

FileVnode::FileVnode(const void *data, size_t size, mode_t mode) : Vnode(S_IFREG | mode)
{
    this->data = new char[size];
    memcpy(this->data, data, size);
    fileSize = size;
}

FileVnode::~FileVnode()
{
    delete data;
}

bool FileVnode::isSeekable()
{
    return true;
}

ssize_t FileVnode::pread(void *buffer, size_t size, off_t offset)
{
    char *buf = (char *)buffer;

    for (size_t i = 0; i < size; i++) {
        if (offset + i >= fileSize) {
            return i;
        }
        buf[i] = data[offset + i];
    }

    return size;
}
