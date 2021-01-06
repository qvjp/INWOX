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
 * libc/src/stdlib/malloc.c
 * 定义malloc内部实现所需语句
 */
#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h> /* size_t */
#include <stdlib.h>

#if __is_inwox_libc
#include <sys/mman.h>
#define mapMemory(size)         mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#define unmapMemory(addr, size) munmap(addr, size)
#else /* if __is_inwox_libk */
extern void *__mapMemory(size_t);
extern void __unmapMemory(void *, size_t);
#define mapMemory(size)         __mapMemory(size)
#define unmapMemory(addr, size) __unmapMemory(addr, size)
#endif
typedef struct Mem_Ctrl_Blk {
    size_t magic;
    size_t size;
    struct Mem_Ctrl_Blk *prev;
    struct Mem_Ctrl_Blk *next;
} Mem_Ctrl_Blk;

#define MAGIC_BIG_MCB  0xC001C0DE
#define MAGIC_FREE_MCB 0xBEEFBEEF
#define MAGIC_USED_MCB 0xDEADBEEF
#define MAGIC_END_MCB  0xDEADDEAD

#define PAGESIZE 0x1000

#define ALIGN_UP(value, alignment) ((((value)-1) & ~((alignment)-1)) + (alignment))

extern Mem_Ctrl_Blk *firstBigBlock;

Mem_Ctrl_Blk *__allocateBigBlock(Mem_Ctrl_Blk *lastBigBlock, size_t size);
void __splitBlock(Mem_Ctrl_Blk *block, size_t size);
Mem_Ctrl_Blk *__unifyBlocks(Mem_Ctrl_Blk *first, Mem_Ctrl_Blk *second);

#endif /* MALLOC_H */
