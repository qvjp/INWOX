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
 * 实现物理内存基本操作，主要分为两部分
 * 1. 从multiboot中获取内存信息
 * 2. 使用栈来管理可用内存
 * 在栈中的内存表示可用，可以分配给调用者，使用完毕后，再push回栈中
 */

#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/physicalmemory.h>
#include <inwox/kernel/print.h>

/**
 * 0xFFC00000即4G-4M,最高的4M是页目录页表占用，
 * 物理内存管理的栈空间紧跟着页表页目录存放，向低地址空间生长，在系统启动时默认分配4K的空间
 * 可以管理4M内存，后续动态扩展栈来管理更大内存
 */
static inwox_phy_addr_t *const stack = (inwox_phy_addr_t *)0xFFC00000;
/* 栈已经使用的大小（可用内存页数目） */
static size_t stackUsed = 0;
/* 栈剩余空间大小（还可以存放多少内存页） */
static size_t stackLeft = 0;

/**
 * @brief 判断某物理地址是否被内核使用
 * 
 * @param physicalAddress 待判断的物理地址
 * @return true 被内核使用
 * @return false 没有被内核使用
 */
static inline bool isUsedByKernel(inwox_phy_addr_t physicalAddress)
{
    return (physicalAddress >= (inwox_phy_addr_t)&bootstrapBegin &&
            physicalAddress < (inwox_phy_addr_t)&bootstrapEnd) ||
           (physicalAddress >= (inwox_phy_addr_t)&kernelPhysicalBegin &&
            physicalAddress < (inwox_phy_addr_t)&kernelPhysicalEnd) ||
           physicalAddress == 0;
}

/**
 * @brief 判断某物理地址是否被multiboot module使用
 * 
 * @param physicalAddress 待判断的物理地址
 * @param modules 
 * @param moduleCount 
 * @return true 
 * @return false 
 */
static inline bool isUsedByModule(inwox_phy_addr_t physicalAddress, multiboot_mod_list *modules, uint32_t moduleCount)
{
    for (size_t i = 0; i < moduleCount; i++) {
        if (physicalAddress >= modules[i].mod_start && physicalAddress < modules[i].mod_end) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 判断某物理地址是否被multiboot使用
 * 
 * @param physicalAddress 
 * @param multiboot 
 * @return true 
 * @return false 
 */
static inline bool isUsedByMultiboot(inwox_phy_addr_t physicalAddress, multiboot_info *multiboot)
{
    inwox_phy_addr_t mmapEnd = multiboot->mmap_addr + multiboot->mmap_length;
    inwox_phy_addr_t modsEnd = multiboot->mods_addr + multiboot->mods_count * sizeof(multiboot_mod_list);
    return ((physicalAddress >= (multiboot->mmap_addr & ~0xFFF) && physicalAddress < mmapEnd) ||
            (physicalAddress >= (multiboot->mods_addr & ~0xFFF) && physicalAddress < modsEnd));
}

/**
 * @brief 初始化物理内存
 * 
 * 找到所有可用内存，并将其用栈管理起来，以后请求内存就是从栈pop，归还内存就是push
 * 具体内存信息从multiboot中获取，方法如下：
 * 
 * @param multiboot 
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
    ptrdiff_t mmapOffset = mmapPhys - mmapAligned;
    size_t mmapSize = ALIGN_UP(mmapOffset + multiboot->mmap_length, PAGESIZE);

    inwox_phy_addr_t modulesPhys = (inwox_phy_addr_t)multiboot->mods_addr;
    inwox_phy_addr_t modulesAligned = modulesPhys & ~0xFFF;
    ptrdiff_t modulesOffset = modulesPhys - modulesAligned;
    size_t modulesSize = ALIGN_UP(modulesOffset + multiboot->mods_count * sizeof(multiboot_mod_list), PAGESIZE);

    inwox_vir_addr_t mmapMapped = kernelSpace->mapPhysical(mmapAligned, mmapSize, PROT_READ);
    inwox_vir_addr_t modulesMapped = kernelSpace->mapPhysical(modulesAligned, modulesSize, PROT_READ);

    inwox_vir_addr_t mmap = mmapMapped + mmapOffset;
    inwox_vir_addr_t mmapEnd = mmap + multiboot->mmap_length;

    multiboot_mod_list *modules = (multiboot_mod_list *)(modulesMapped + modulesOffset);

    while (mmap < mmapEnd) {
        multiboot_mmap_entry *mmapEntry = (multiboot_mmap_entry *)mmap;
        if (mmapEntry->type == MULTIBOOT_MEMORY_AVAILABLE && mmapEntry->base_addr + mmapEntry->length <= UINTPTR_MAX) {
            inwox_phy_addr_t addr = (inwox_phy_addr_t)mmapEntry->base_addr;
            for (uint64_t i = 0; i < mmapEntry->length; i += PAGESIZE) {
                if (isUsedByModule(addr + i, modules, multiboot->mods_count) || isUsedByKernel(addr + i) ||
                    isUsedByMultiboot(addr + i, multiboot)) {
                    continue;
                }
                pushPageFrame(addr + i);
            }
        }
        mmap += mmapEntry->size + 4;
    }
    kernelSpace->unmapPhysical(mmapMapped, mmapSize);
    kernelSpace->unmapPhysical(modulesMapped, modulesSize);
    Print::printf("Free Memory: %u KiB\n", stackUsed * 4);
}

/**
 * @brief 将物理内存压入物理内存管理栈中
 * 
 * 当管理栈还有空间时，直接将物理内存压栈，若栈空间用完，则将此页作为物理内存管理栈的一部分
 * 合并入栈的虚拟地址空间
 * 
 * @param physicalAddress 待归还物理内存管理器的内存
 */
void PhysicalMemory::pushPageFrame(inwox_phy_addr_t physicalAddress)
{
    if (unlikely(stackLeft == 0)) {
        kernelSpace->mapPhysical((inwox_vir_addr_t)stack - stackUsed * 4 - PAGESIZE, physicalAddress, PAGESIZE,
                                 PROT_READ | PROT_WRITE);
        stackLeft += 1024;
    } else {
        stack[-++stackUsed] = physicalAddress;
        stackLeft--;
    }
}

/**
 * @brief 将物理内存从物理内存管理栈弹出
 * 
 * @return inwox_phy_addr_t 分配到的物理内存
 */
inwox_phy_addr_t PhysicalMemory::popPageFrame()
{
    if (unlikely(stackUsed == 0)) {
        if (likely(stackLeft > 0)) {
            inwox_vir_addr_t virtualAddress = (inwox_vir_addr_t)stack - stackLeft * 4;
            inwox_phy_addr_t result = kernelSpace->getPhysicalAddress(virtualAddress);
            kernelSpace->unmapPhysical(virtualAddress, PAGESIZE);
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
