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

/**
 * lib/include/sys/mman.h
 * mmap munmap声明
 */

#ifndef SYS_MMAN_H__
#define SYS_MMAN_H__

#define __need_mode_t
#define __need_off_t
#define __need_size_t
#include <sys/types.h>
#include <inwox/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 要求内核创建一个新的虚拟内存区域，从addr开始，将文件描述符fd指定的对象的一个连续片映射到这个新区域，
 * 连续对象片大小为size字节，从距文件开始处偏移量为offset字节的位置开始
 */
void* mmap(void *addr, size_t size, int protection, int flags, int fd, off_t offset);

/**
 * 删除从虚拟地址addr开始，由接下俩size个字节组成的区域，接下来再对这块区域使用将产生段错误
 */
int munmap(void *addr, size_t size);

#ifdef __cplusplus
}
#endif

#endif