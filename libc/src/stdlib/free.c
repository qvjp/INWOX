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
 * libc/src/stdlib/free.c
 * 实现libc中free函数
 */

#include "malloc.h"

void free(void *addr)
{
    if (addr == NULL) {
        return;
    }

    Mem_Ctrl_Blk *block = (Mem_Ctrl_Blk *)addr - 1;
    block->magic = MAGIC_FREE_MCB;

    /* 如果释放的内存可以和前后合并，则合并 */
    if (block->prev && block->prev->magic == MAGIC_FREE_MCB) {
        block = __unifyBlocks(block->prev, block);
    }
    if (block->next->magic == MAGIC_FREE_MCB) {
        block = __unifyBlocks(block, block->next);
    }
    /* 如果是凑够了一整个大的Block，则返回给OS */
    if (block->prev == NULL && block->next->magic == MAGIC_END_MCB) {
        Mem_Ctrl_Blk *bigBlock = block - 1;
        if (bigBlock->prev) {
            bigBlock->prev->next = bigBlock->next;
        }
        if (bigBlock->next) {
            bigBlock->next->prev = bigBlock->prev;
        }
        unmapMemory(bigBlock, bigBlock->size);
    }
}
