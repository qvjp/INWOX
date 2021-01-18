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
 * kernel/src/memorysegment.cpp
 * 内存分段,段是一个逻辑概念，每个地址空间有一个段表（链），给地址空间分配内存时也相应的分配段，
 * 释放内存时也一起释放，当地址空间销毁时则遍历段链，将全部内存释放。
 */

#include <assert.h>
#include <string.h>
#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/memorysegment.h>

/**
 * 用1页的空间存放segment的索引，当前一个segment用20个字节表示，最多可以管理204个
 * 若有更多的段需要管理，可自动分配此字段的长度
 */
static char segmentsPage[PAGESIZE] ALIGNED(PAGESIZE) = {0};

/**
 * @brief 获取segment可用空间大小
 * 
 * 由于segment在虚拟地址中是按顺序存放，所以空闲部分可以通过上一个和下一个的首地址和长度来计算得出
 * 
 * @param segment 要查找的segment
 * @return size_t 空闲空间大小
 */
static inline size_t getFreeSpaceAfter(MemorySegment *segment)
{
    return segment->next->address - (segment->address + segment->size);
}

/**
 * @brief MemorySegment构造函数
 * 
 * 内存段通过两个指针连接成一个链，一个地址空间对应一个内存段链，释放一个地址空间的全部内存
 * 可以通过遍历此链进行
 * 
 * @param address 本段开始地址
 * @param size 段长度
 * @param flags 保护位
 * @param prev 指向前一个段的指针
 * @param next 指向后一个段的指针
 */
MemorySegment::MemorySegment(inwox_vir_addr_t address, size_t size, int flags, MemorySegment *prev, MemorySegment *next)
{
    this->address = address;
    this->size = size;
    this->flags = flags;
    this->prev = prev;
    this->next = next;
}

/**
 * @brief 将指定地址指定长度的虚拟内存加入段链
 * 
 * @param firstSegment 段首元素
 * @param address 待加入段链的虚拟内存起始地址
 * @param size 虚拟内存长度
 * @param protection 保护位
 */
void MemorySegment::addSegment(MemorySegment *firstSegment, inwox_vir_addr_t address, size_t size, int protection)
{
    MemorySegment *newSegment = allocateSegment(address, size, protection);
    addSegment(firstSegment, newSegment);
    verifySegmentList();
}

/**
 * @brief 从内存段链表移除指定区段虚拟内存
 * 
 * @param firstSegment 链表首元素
 * @param address 移除的虚拟内存首地址
 * @param size 长度
 */
void MemorySegment::removeSegment(MemorySegment *firstSegment, inwox_vir_addr_t address, size_t size)
{
    MemorySegment *currentSegment = firstSegment;

    while (currentSegment->address + currentSegment->size <= address) {
        currentSegment = currentSegment->next;
    }

    while (size) {
        if (currentSegment->address == address && currentSegment->size <= size) {
            // 将整个段移除
            address += currentSegment->size;
            size -= currentSegment->size;

            if (size < getFreeSpaceAfter(currentSegment)) {
                size = 0;
            } else {
                size -= getFreeSpaceAfter(currentSegment);
            }

            MemorySegment *next = currentSegment->next;
            if (next) {
                next->prev = currentSegment->prev;
            }
            if (currentSegment->prev) {
                currentSegment->prev->next = next;
            }

            deallocateSegment(currentSegment);
            currentSegment = next;
            continue;
        } else if (currentSegment->address == address && currentSegment->size > size) {
            currentSegment->address += size;
            currentSegment->size -= size;
            size = 0;
        } else if (currentSegment->address + currentSegment->size < address + size) {
            size_t diff = currentSegment->address + currentSegment->size - address;
            currentSegment->size -= diff;
            size -= diff;
            address += diff;
        } else {
            // 拆分段
            size_t firstSize = address - currentSegment->address;
            size_t secondSize = currentSegment->size - firstSize - size;

            MemorySegment *newSegment = allocateSegment(address + size, secondSize, currentSegment->flags);

            newSegment->prev = currentSegment;
            newSegment->next = currentSegment->next;

            currentSegment->next = newSegment;
            currentSegment->size = firstSize;
        }

        if (size < getFreeSpaceAfter(currentSegment)) {
            size = 0;
        } else {
            size -= getFreeSpaceAfter(currentSegment);
        }

        currentSegment = currentSegment->next;
    }
}

/**
 * @brief 在可用的地址空间中找到一块空闲的segment
 * 
 * @param firstSegment 第一个segment
 * @param size 要找的空闲segment大小
 * @return inwox_vir_addr_t 空闲segment虚拟地址
 */
inwox_vir_addr_t MemorySegment::findFreeSegment(MemorySegment *firstSegment, size_t size)
{
    MemorySegment *currentSegment = firstSegment;
    while (getFreeSpaceAfter(currentSegment) < size) {
        currentSegment = currentSegment->next;
        if (!currentSegment) {
            return 0;
        }
    }
    return currentSegment->address + currentSegment->size;
}

/**
 * @brief 将新段加入到段链表
 * 
 * @param firstSegment 段首元素
 * @param newSegment 新段元素
 */
void MemorySegment::addSegment(MemorySegment *firstSegment, MemorySegment *newSegment)
{
    inwox_vir_addr_t endAddress = newSegment->address + newSegment->size;
    MemorySegment *currentSegment = firstSegment;

    while (currentSegment->next && currentSegment->next->address < endAddress) {
        currentSegment = currentSegment->next;
    }

    assert(currentSegment->address + currentSegment->size <= newSegment->address);
    assert(!currentSegment->next || currentSegment->next->address >= endAddress);

    newSegment->prev = currentSegment;
    newSegment->next = currentSegment->next;

    currentSegment->next = newSegment;
    if (newSegment->next) {
        newSegment->next->prev = newSegment;
    }
}

/**
 * @brief 将指定位置大小的虚拟地址分配为一个段
 * 
 * @param address 虚拟地址
 * @param size 地址空间长度
 * @param flags 保护位
 * @return MemorySegment* 分配好的段
 */
MemorySegment *MemorySegment::allocateSegment(inwox_vir_addr_t address, size_t size, int flags)
{
    MemorySegment *current = (MemorySegment *)segmentsPage;

    // 找到一个还未使用的segment
    while (current->address != 0 && current->size != 0) {
        current++;
        // current到了segmentsPage的最后一个索引表示的segment，则去新的1K中分配可用segment
        if (((uintptr_t)current & 0xFFF) == (PAGESIZE - PAGESIZE % sizeof(MemorySegment))) {
            MemorySegment **nextPage = (MemorySegment **)current;
            assert(nextPage != nullptr);
            current = *nextPage;
        }
    }

    current->address = address;
    current->size = size;
    current->flags = flags;

    return current;
}

/**
 * @brief 将指定的段解除分配
 * 
 * @param segment 待操作的段
 */
void MemorySegment::deallocateSegment(MemorySegment *segment)
{
    memset(segment, 0, sizeof(MemorySegment));
}

/**
 * @brief 校验段链表是否还有剩余空间，空间不足自动分配，在addsegment处调用
 * 
 */
void MemorySegment::verifySegmentList()
{
    MemorySegment *current = (MemorySegment *)segmentsPage;
    MemorySegment **nextPage;

    // segmentsPage中，空闲条目个数
    int freeSegmentSpaceFound = 0;
    while (true) {
        if (current->address == 0 && current->size == 0) {
            freeSegmentSpaceFound++;
        }
        current++;
        // current到了segmentsPage的最后一个索引表示的segment
        if (((uintptr_t)current & 0xFFF) == (PAGESIZE - PAGESIZE % sizeof(MemorySegment))) {
            nextPage = (MemorySegment **)current;
            if (!*nextPage) {
                break;
            }
            current = *nextPage;
        }
    }
    // 当用完segmentsPage的空间后，再分配1页空间，最后一个元素指向新分配的空间
    if (freeSegmentSpaceFound == 1) {
        *nextPage = (MemorySegment *)kernelSpace->mapMemory(PAGESIZE, PROT_READ | PROT_WRITE);
        memset(*nextPage, 0, PAGESIZE);
    }
}
