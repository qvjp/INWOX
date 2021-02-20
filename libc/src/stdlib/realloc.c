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

/**
 * libc/src/stdlib/realloc.c
 * realloc
 */

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define __need_ssize_t
#include <sys/types.h>
#include "malloc.h"

static void changeChunkSize(Mem_Ctrl_Blk *chunk, size_t sizeDiff)
{
    Mem_Ctrl_Blk *next = chunk->next;
    Mem_Ctrl_Blk *newNextChunk = (Mem_Ctrl_Blk *)((uintptr_t)next + sizeDiff);
    memmove(newNextChunk, next, sizeof(Mem_Ctrl_Blk));
    chunk->next = newNextChunk;
    chunk->size += sizeDiff;
    newNextChunk->size -= sizeDiff;
}

void *realloc(void *addr, size_t size)
{
    if (addr == NULL) {
        return malloc(size);
    }
    __lockHeap();
    Mem_Ctrl_Blk *chunk = (Mem_Ctrl_Blk *)addr - 1;
    assert(chunk->magic == MAGIC_USED_MCB);
    if (size == 0) {
        size = 1;
    }
    size = ALIGN_UP(size, alignof(max_align_t));
    ssize_t sizeDiff = size - chunk->size;
    if (sizeDiff == 0) {
        __unlockHeap();
        return addr;
    }

    Mem_Ctrl_Blk *next = chunk->next;
    if (next->magic == MAGIC_FREE_MCB) {
        if ((sizeDiff > 0 && next->size > sizeDiff + sizeof(Mem_Ctrl_Blk)) || sizeDiff < 0) {
            changeChunkSize(chunk, sizeDiff);
            __unlockHeap();
            return addr;
        }
    }
    __unlockHeap();
    void *newAddr = malloc(size);
    if (newAddr == NULL) {
        return NULL;
    }
    size_t copySize = sizeDiff < 0 ? size : chunk->size;
    memcpy(newAddr, addr, copySize);
    free(addr);
    return newAddr;
}
