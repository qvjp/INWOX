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

/* kernel/src/directory.cpp
 * DirectoryVnode class.
 */

#include <errno.h>
#include <string.h>
#include <inwox/dirent.h>
#include <inwox/kernel/directory.h>
#include <inwox/stat.h>

DirectoryVnode::DirectoryVnode(DirectoryVnode *parentVnode, mode_t mode) : Vnode(S_IFDIR | mode)
{
    childCount = 0;
    childNodes = nullptr;
    fileNames = nullptr;
    mutex = KTHREAD_MUTEX_INITIALIZER;
    parent = parentVnode;
}

DirectoryVnode::~DirectoryVnode()
{
    delete childNodes;
    delete fileNames;
}

void DirectoryVnode::addChildNode(const char *path, Vnode *vnode)
{
    ScopedLock lock(&mutex);
    Vnode **newChildNodes = new Vnode *[childCount + 1];
    const char **newFileNames = new const char *[childCount + 1];

    memcpy(newChildNodes, childNodes, sizeof(Vnode *) * childCount);
    memcpy(newFileNames, fileNames, sizeof(const char *) * childCount);

    childNodes = newChildNodes;
    fileNames = newFileNames;

    childNodes[childCount] = vnode;
    fileNames[childCount] = strdup(path);
    childCount++;
}

Vnode *DirectoryVnode::getChildNode(const char *path)
{
    ScopedLock lock(&mutex);
    if (strcmp(path, ".") == 0) {
        return this;
    } else if (strcmp(path, "..") == 0) {
        return parent ? parent : this;
    }

    for (size_t i = 0; i < childCount; i++) {
        if (strcmp(path, fileNames[i]) == 0) {
            return childNodes[i];
        }
    }

    errno = ENOENT;
    return nullptr;
}

ssize_t DirectoryVnode::readdir(unsigned long offset, void *buffer, size_t size)
{
    ScopedLock lock(&mutex);
    const char *name;
    if (offset == 0) {
        name = ".";
    } else if (offset == 1) {
        name = "..";
    } else if (offset - 2 < childCount) {
        name = fileNames[offset - 2];
    } else if (offset - 2 == childCount) {
        return 0;
    } else {
        return -1;
    }
    size_t structSize = sizeof(struct dirent) + strlen(name) + 1;
    if (size >= structSize) {
        struct dirent *entry = (struct dirent *)buffer;
        entry->d_reclen = size;
        strcpy(entry->d_name, name);
    }
    return structSize;
}
