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
 * kernel/src/physicalmemory.cpp
 * 实现物理内存基本操作
 */

#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/physicalmemory.h>
#include <inwox/kernel/print.h>

/* 内存栈开始位置 */
static inwox_phy_addr_t *const stack = (inwox_phy_addr_t *)0xFFC00000;
/* 可用内存页数目 */
static size_t stackUsed = 0;
/* 内存栈中当前页面剩余可以存放物理地址的数量 */
static size_t stackLeft = 0;

/**
 * 识别某物理地址是否已经使用，
 * 通过判断该地址是否在bootstrapBegin->bootstrapEnd或
 * kernelPhysicalBegin->kernelPhysicalEnd内
 */
static inline bool isAddressInUse(inwox_phy_addr_t physicalAddress)
{
    return (physicalAddress >= (inwox_phy_addr_t)&bootstrapBegin && physicalAddress <= (inwox_phy_addr_t)&bootstrapEnd) ||
           (physicalAddress >= (inwox_phy_addr_t)&kernelPhysicalBegin && physicalAddress <= (inwox_phy_addr_t)&kernelPhysicalEnd) || physicalAddress == 0;
}

/**
 * 初始化物理内存
 * 通过解析multiboot所给信息，将可用物理内存全部push到栈中，
 * 以便后边的分配、释放
 */
void PhysicalMemory::initialize(multiboot_info *multiboot)
{
    /* mem_* 是否有效？ */
    if (!CHECK_MULTIBOOT_FLAG(multiboot->flags, 0)) {
        return;
    }
    /* mmap_* 是否有效 */
    if (!CHECK_MULTIBOOT_FLAG(multiboot->flags, 6)) {
        return;
    }

    inwox_phy_addr_t mmapPhys = (inwox_phy_addr_t)multiboot->mmap_addr;
    inwox_phy_addr_t mmapAligned = mmapPhys & ~0xFFF;
    ptrdiff_t offset = mmapPhys - mmapAligned;

    inwox_vir_addr_t virtualAddress = kernelSpace->mapPhysical(mmapAligned, 0x1000, PROT_READ);

    inwox_vir_addr_t mmap = virtualAddress + offset;
    inwox_vir_addr_t mmapEnd = mmap + multiboot->mmap_length;

    while (mmap < mmapEnd) {
        multiboot_mmap_entry *mmapEntry = (multiboot_mmap_entry *)mmap;
        if (mmapEntry->type == MULTIBOOT_MEMORY_AVAILABLE && mmapEntry->base_addr + mmapEntry->length <= UINTPTR_MAX) {
            inwox_phy_addr_t addr = (inwox_phy_addr_t)mmapEntry->base_addr;
            for (uint64_t i = 0; i < mmapEntry->length; i += 0x1000) {
                if (isAddressInUse(addr + i))
                    continue;
                pushPageFrame(addr + i);
            }
        }
        mmap += mmapEntry->size + 4;
    }
    kernelSpace->unmapPhysical(virtualAddress, 0x1000);
    Print::printf("Free Memory: %u KiB\n", stackUsed * 4);
}

/**
 * 将物理内存页压入栈中
 * 若当前栈空间不足，则申请给栈申请空间
 */
void PhysicalMemory::pushPageFrame(inwox_phy_addr_t physicalAddress)
{
    if (unlikely(stackLeft == 0)) {
        kernelSpace->mapPhysical((inwox_vir_addr_t)stack - stackUsed * 4 - 0x1000, physicalAddress, 0x1000, PROT_READ | PROT_WRITE);
        stackLeft += 1024;
    } else {
        stack[-++stackUsed] = physicalAddress;
        stackLeft--;
    }
}

/**
 * 将内存页出栈
 */
inwox_phy_addr_t PhysicalMemory::popPageFrame()
{
    if (unlikely(stackUsed == 0)) {
        if (likely(stackLeft > 0)) {
            inwox_vir_addr_t virtualAddress = (inwox_vir_addr_t)stack - stackLeft * 4;
            inwox_phy_addr_t result = kernelSpace->getPhysicalAddress(virtualAddress);
            kernelSpace->unmapPhysical(virtualAddress, 0x1000);
            stackLeft -= 1024;
            return result;
        } else {
            Print::printf("Out of Memory\n");
            return 0;
        }
    } else {
        stackLeft++;
        return stack[-stackUsed--];
    }
}
