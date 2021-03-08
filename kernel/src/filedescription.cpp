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

/* kernel/src/filedescription.cpp
 * FileDescription class.
 */

#include <inwox/kernel/filedescription.h>

FileDescription::FileDescription(Vnode *vnode)
{
    this->vnode = vnode;
    offset = 0;
}

FileDescription *FileDescription::openat(const char *path, int /* flags */, mode_t /* mode */)
{
    Vnode *node = resolvePath(vnode, path);
    if (!node) {
        return nullptr;
    }
    return new FileDescription(node);
}

ssize_t FileDescription::read(void *buffer, size_t size)
{
    if (vnode->isSeekable()) {
        ssize_t result = vnode->pread(buffer, size, offset);

        if (result != -1) {
            offset += result;
        }
        return result;
    }
    return vnode->read(buffer, size);
}

ssize_t FileDescription::readdir(unsigned long offset, void *buffer, size_t size)
{
    return vnode->readdir(offset, buffer, size);
}

ssize_t FileDescription::write(const void *buffer, size_t size)
{
    return vnode->write(buffer, size);
}

int FileDescription::tcgetattr(struct termios *result)
{
    return vnode->tcgetattr(result);
}

int FileDescription::tcsetattr(int flags, const struct termios *termio)
{
    return vnode->tcsetattr(flags, termio);
}
