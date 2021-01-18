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

/* kernel/include/inwox/mman.h
 * mmap相关定义
 */

#ifndef INWOX_MMAN_H_
#define INWOX_MMAN_H_

/**
 * 内存的保护掩码，用于内存映射是权限的控制
 */
#define PROT_READ  (1 << 0)
#define PROT_WRITE (1 << 1)
#define PROT_EXEC  (1 << 2)
#define PROT_NONE  0

#define _PROT_FLAGS (PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE)

/**
 * 内存映射的flags，用于mmap/munmap
 */
#define MAP_PRIVATE   (1 << 0) /* 私有的、写时复制的映射，映射的更新对映射同一文件的其他进程不可见 */
#define MAP_ANONYMOUS (1 << 1) /* 映射的内存不基于任何后端文件，内容将被初始化为0 */
#define MAP_SHARED    (1 << 2) /* 共享映射，映射的修改对其他映射相同文件的进程可见 */

#define MAP_FAILED ((void *)0)

#if defined(__is_inwox_kernel) || defined(__is_inwox_libc)
#include <stddef.h>
#include <inwox/types.h>
struct __mmapRequest {
    void *_addr;
    size_t _size;
    int _protection;
    int _flags;
    int _fd;
    __off_t _offset;
};
#endif

#endif /* INWOX_MMAN_H_ */
