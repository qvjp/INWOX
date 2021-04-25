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
/* kernel/include/inwox/file.h
 * 文件vnode
 */

#ifndef KERNEL_FILE_H_
#define KERNEL_FILE_H_

#include <inwox/kernel/kthread.h>
#include <inwox/kernel/vnode.h>

class FileVnode : public Vnode {
public:
    FileVnode(const void *data, size_t size, mode_t mode, dev_t dev, ino_t ino);
    ~FileVnode();
    virtual int ftruncate(off_t length);
    virtual bool isSeekable();
    virtual ssize_t pread(void *buffer, size_t size, off_t offset);
    virtual ssize_t pwrite(const void *buffer, size_t size, off_t offset);

public:
    char *data;
    size_t fileSize;
    kthread_mutex_t mutex;
};

#endif /* KERNEL_FILE_H_ */
