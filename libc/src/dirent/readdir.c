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

/* libc/src/dirent/readdir.c
 * 读取目录内容
 */

#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/syscall.h>
#define __need_ssize_t
#include <sys/types.h>

DEFINE_SYSCALL(SYSCALL_READDIR, ssize_t, sys_readdir, (int, unsigned long, void *, size_t));

struct dirent *readdir(DIR *dir)
{
    if (!dir->dirent) {
        dir->dirent = malloc(sizeof(struct dirent));
        dir->dirent->d_reclen = sizeof(struct dirent);
    }
    size_t entrySize = dir->dirent->d_reclen;
    ssize_t size = sys_readdir(dir->fd, dir->offset, dir->dirent, entrySize);

    if (size < 0) {
        return NULL;
    } else if (size == 0) {
        return NULL;
    } else if ((size_t) size <= entrySize) {
        dir->offset++;
        return dir->dirent;
    } else {
        free(dir->dirent);
        dir->dirent = malloc((size_t) size);
        sys_readdir(dir->fd, dir->offset, dir->dirent, (size_t) size);
        dir->offset++;
        return dir->dirent;
    }
}
