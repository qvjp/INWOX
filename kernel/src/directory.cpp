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

/* kernel/src/directory.cpp
 * DirectoryVnode class.
 */

#include <errno.h>
#include <string.h>
#include <inwox/kernel/directory.h>

DirectoryVnode::DirectoryVnode() {
    childCount = 0;
    childNodes = nullptr;
    fileNames = nullptr;
}

void DirectoryVnode::addChildNode(const char *path, Vnode *vnode) {
    Vnode **newChildNodes = new Vnode*[childCount + 1];
    const char** newFileNames = new const char*[childCount + 1];

    memcpy(newChildNodes, childNodes, sizeof(Vnode*) * childCount);
    memcpy(newFileNames, fileNames, sizeof(const char*) * childCount);

    childNodes = newChildNodes;
    fileNames = newFileNames;

    childNodes[childCount] = vnode;
    fileNames[childCount] = path;
    childCount++;
}

Vnode* DirectoryVnode::openat(const char* path, int flags, mode_t mode) {
    while (*path == '/') {
        path++;
    }

    size_t length = strcspn(path, "/");

    if (length == 0) {
        return this;
    }

    for (size_t i = 0; i < childCount; i++) {
        if (strncmp(path, fileNames[i], length) == 0) {
            if (path[length] == '\0') {
                return childNodes[i];
            } else {
                return childNodes[i]->openat(path + length, flags, mode);
            }
        }
    }

    errno = ENOENT;
    return nullptr;
}
