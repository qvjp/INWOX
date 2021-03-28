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

/* kernel/include/inwox/directory.h
 * 目录vnode
 */

#ifndef KERNEL_DIRECTORY_H_
#define KERNEL_DIRECTORY_H_

#include <inwox/kernel/kthread.h>
#include <inwox/kernel/vnode.h>

class DirectoryVnode : public Vnode {
public:
    DirectoryVnode(DirectoryVnode *parent, mode_t mode);
    ~DirectoryVnode();
    void addChildNode(const char *path, Vnode *vnode);
    virtual Vnode *getChildNode(const char *path);
    virtual ssize_t readdir(unsigned long offset, void *buffer, size_t size);

public:
    size_t childCount;

private:
    Vnode **childNodes;
    const char **fileNames;
    kthread_mutex_t mutex;
    DirectoryVnode *parent;
};

#endif /* KERNEL_DIRECTORY_H_ */
