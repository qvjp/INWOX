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
 * 实现libc中malloc函数
 */

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include "malloc.h"

/**
 * 初始时的空Big Mem_Ctrl_Blk
 * +----------+
 * |//////////+
 * +----------+ <------ MAGIC_END_MCB
 * |//////////|
 * +----------+ <------ MAGIC_BIG_MCB
 */
static Mem_Ctrl_Blk emptyBigBlock[2] = {
    {MAGIC_BIG_MCB, sizeof(emptyBigBlock), NULL, NULL},
    {MAGIC_END_MCB, 0, NULL, NULL}
};

/* firstBigBlock指向最开始的空内存控制块，每次分配内存都从这里开始 */
Mem_Ctrl_Blk *firstBigBlock = emptyBigBlock;

/**
 * 申请一个大内存控制块，并将其链在上一个大内存控制块之后
 * 申请时按页对齐
 */
Mem_Ctrl_Blk *__allocateBigBlock(Mem_Ctrl_Blk *lastBigBlock, size_t size)
{
    assert(lastBigBlock->magic == MAGIC_BIG_MCB);
    /* 每次申请都会多出两个头尾MCB，所以要多申请两个的空间 */
    size += 2 * sizeof(Mem_Ctrl_Blk);
    /* 因为是按页申请，所以页对齐 */
    size = ALIGN_UP(size, PAGESIZE);

    /* 每次申请最少4页 */
    /* if (size < 4 * PAGESIZE)
    {
        size = 4 * PAGESIZE;
    } */

    Mem_Ctrl_Blk *bigBlock = mapMemory(size);
    Mem_Ctrl_Blk *block = bigBlock + 1;
    Mem_Ctrl_Blk *endBlock = (void *)bigBlock + size - sizeof(Mem_Ctrl_Blk);

    bigBlock->magic = MAGIC_BIG_MCB;
    bigBlock->size = size;
    bigBlock->prev = lastBigBlock;
    bigBlock->next = NULL;

    lastBigBlock->next = bigBlock;

    block->magic = MAGIC_FREE_MCB;
    block->size = size - 3 * sizeof(Mem_Ctrl_Blk);
    block->prev = NULL;
    block->next = endBlock;

    endBlock->magic = MAGIC_END_MCB;
    endBlock->size = 0;
    endBlock->prev = block;
    endBlock->next = NULL;

    return bigBlock;
}

/**
 * 将两个内存控制块合并，返回前一个的首地址
 */
Mem_Ctrl_Blk *__unifyBlocks(Mem_Ctrl_Blk *first, Mem_Ctrl_Blk *second)
{
    first->next = second->next;
    first->size += sizeof(Mem_Ctrl_Blk) + second->size;
    second->next->prev = first;

    return first;
}

/**
 * 将所给block切分成两部分，一部分是size大小，剩下是另一部分
 * 切完后示意图如下：
 * +----------+
 * |          |
 * |          |
 * |          |
 * |          |
 * |          |
 * +----------+
 * |//////////|
 * +----------+ <------ newBlock
 * |          | \
 * |          | |______用户申请的大小
 * |          | |
 * |          | /
 * +----------+ <------ 最终返回给用户的位置
 * |//////////|
 * +----------+ <------ block
 */
void __splitBlock(Mem_Ctrl_Blk *block, size_t size)
{
    Mem_Ctrl_Blk *newBlock = (Mem_Ctrl_Blk *)((void *)block + sizeof(Mem_Ctrl_Blk) + size);
    newBlock->magic = MAGIC_FREE_MCB;
    newBlock->size = block->size - sizeof(Mem_Ctrl_Blk) - size;
    newBlock->prev = block;
    newBlock->next = block->next;

    block->size = size;
    block->next->prev = newBlock;
    block->next = newBlock;
}

void *malloc(size_t size)
{
    if (size == 0)
        size = 1;
    size_t blockSize = sizeof(Mem_Ctrl_Blk);
    /* 16字节对齐 */
    size = ALIGN_UP(size, alignof(max_align_t));
    size_t totalSize = blockSize + size;

    Mem_Ctrl_Blk *currentBigBlock = firstBigBlock;
    Mem_Ctrl_Blk *currentBlock = currentBigBlock + 1;

    while (1) {
        switch (currentBlock->magic) {
            case MAGIC_FREE_MCB:
                if (currentBlock->size >= totalSize) {
                    /* 找到需要的块 */
                    if (currentBlock->size > totalSize + blockSize) {
                        __splitBlock(currentBlock, size);
                    }
                    currentBlock->magic = MAGIC_USED_MCB;
                    return (void *)(currentBlock + 1);
                } else {
                    currentBlock = currentBlock->next;
                }
                break;
            case MAGIC_USED_MCB:
                currentBlock = currentBlock->next;
                break;
            case MAGIC_END_MCB:
                /* 若遇到大内存控制块结束，去下一个大内存控制块找，若还没有，则分配新的Big_MCB */
                if (currentBigBlock->next) {
                    currentBigBlock = currentBigBlock->next;
                    currentBlock = currentBigBlock + 1;
                } else {
                    currentBigBlock = __allocateBigBlock(currentBigBlock, totalSize);
                    currentBlock = currentBigBlock + 1;
                    if (!currentBigBlock) {
                        return NULL;
                    }
                }
                break;
            default:
                return NULL;
        }
    }
}
