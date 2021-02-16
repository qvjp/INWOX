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
 * kernel/src/addressspace.cpp
 * 实现地址空间的基本操作
 * 每个进程拥有自己的地址空间，使其以为自己独享整个物理内存
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/physicalmemory.h>
#include <inwox/kernel/print.h>
#include <inwox/kernel/process.h>
#include <inwox/kernel/syscall.h>

/**
 * 内核地址空间
 */
AddressSpace AddressSpace::_kernelSpace;

/**
 * 内核地址空间指针
 */
AddressSpace *kernelSpace;

/**
 * 第一个地址空间，通过它可以访问整个地址空间链表
 */
static AddressSpace *firstAddressSpace = nullptr;

/**
 * @brief 将物理内存保护位转为页的访问标识
 * 
 * @param protection 物理内存保护位
 * @return int 页访问权限标识
 */
static inline int protectionToFlags(int protection)
{
    int flags = PAGE_PRESENT;
    if (protection & PROT_WRITE) {
        flags |= PAGE_WRITABLE;
    }
    return flags;
}

/**
 * @brief 创建一个新的地址空间
 * 
 * 对于每个用户进程，都有自己独立的地址空间，每个地址空间有自己的页目录、页表结构，
 * 并且所有的地址空间连成一个链表，若创建新的地址空间，则将其链入表头
 * 
 * 内核地址空间和用户地址空间互相独立，内核地址空间不链入用户地址空间链表，但每个用户空间都
 * 有一份内核页目录的拷贝，之所以要进行拷贝，是因为一个时刻只有一个地址空间被激活，地址空
 * 间的这个部分包含内核在代表进程执行指令时（比如当应用执行系统调用时）使用的代码、数据和栈
 */
AddressSpace::AddressSpace()
{
    if (this == &_kernelSpace) {  // 具体初始化在initialize()进行
        pageDir = 0;
        pageDirMapped = RECURSIVE_MAPPING + 0x3FF000; // FFFFF000为4G地址空间的最后4K，存放页目录
        firstSegment = nullptr;
        prev = nullptr;
        next = nullptr;
    } else {
        // 用户态新建地址空间时，先分配物理内存用来存放该地址空间的页目录，并将内核页目录复制过去
        pageDir = PhysicalMemory::popPageFrame();
        inwox_vir_addr_t kernelPageDir = kernelSpace->pageDirMapped;
        pageDirMapped = kernelSpace->mapPhysical(pageDir, PAGESIZE, PROT_READ | PROT_WRITE);
        memcpy((void *)pageDirMapped, (const void *)kernelPageDir, PAGESIZE);

        // 创建地址空间时，会为此地址空间创建一个segment
        firstSegment = new MemorySegment(0, PAGESIZE, PROT_NONE | SEG_NOUNMAP, nullptr, nullptr);
        MemorySegment::addSegment(firstSegment, 0xC0000000, -0xC0000000, PROT_NONE | SEG_NOUNMAP);

        // 将新地址空间放入链表头
        prev = nullptr;
        next = firstAddressSpace;
        if (next) {
            next->prev = this;
        }
        firstAddressSpace = this;
    }
}

/**
 * @brief 销毁一个地址空间
 * 
 * 1. 释放本地址空间的内存（segment在地址空间布局整齐，可以方便的将全部内存进行释放）
 * 2. 删除地址空间的页目录
 */
AddressSpace::~AddressSpace()
{
    // 将地址空间从地址空间链表删除
    if (prev) {
        prev->next = next;
    }
    if (next) {
        next->prev = prev;
    }
    if (this == firstAddressSpace) {
        firstAddressSpace = next;
    }
    MemorySegment *currentSegment = firstSegment;
    while (currentSegment) {
        MemorySegment *next = currentSegment->next;
        if (!(currentSegment->flags & SEG_NOUNMAP)) {
            unmapMemory(currentSegment->address, currentSegment->size);
        }
        currentSegment = next;
    }
    PhysicalMemory::pushPageFrame(pageDir);
}

/**
 * 需要在编译期先声明如下的段，因为在内存管理初始化前就要用到
 */
static MemorySegment userSegment(0, 0xC0000000, PROT_NONE, nullptr, nullptr);
static MemorySegment videoSegment(0xC0000000, PAGESIZE, PROT_READ | PROT_WRITE, &userSegment, nullptr);
static MemorySegment readOnlySegment((inwox_vir_addr_t) &kernelVirtualBegin,
                                    (inwox_vir_addr_t) &kernelReadOnlyEnd - (inwox_vir_addr_t) &kernelVirtualBegin,
                                    PROT_READ | PROT_EXEC, &videoSegment, nullptr);
static MemorySegment writableSegment((inwox_vir_addr_t)&kernelReadOnlyEnd,
                              (inwox_vir_addr_t)&kernelVirtualEnd - (inwox_vir_addr_t)&kernelReadOnlyEnd,
                              PROT_READ | PROT_WRITE, &readOnlySegment, nullptr);
// 紧挨着页目录页表的4M为物理内存段
static MemorySegment physicalMemorySegment(RECURSIVE_MAPPING - 0x400000, 0x400000, PROT_READ | PROT_WRITE, &writableSegment, nullptr);
static MemorySegment recursiveMappingSegment(RECURSIVE_MAPPING, -RECURSIVE_MAPPING, PROT_READ | PROT_WRITE, &physicalMemorySegment, nullptr);

/**
 * @brief 将页目录页表索引转为虚拟地址
 * 
 * @param pdIndex 页目录索引
 * @param ptIndex 页表索引
 * @return inwox_vir_addr_t 对应的虚拟地址
 */
static inline inwox_vir_addr_t IndexToaddress(size_t pdIndex, size_t ptIndex)
{
    // 页目录页表的最后一个0x400/0x400递归索引，所以可用的索引肯定小于等于0x3FF/0x3FF
    assert(pdIndex <= 0x3FF);
    assert(ptIndex <= 0x3FF);
    return (pdIndex << 22) | (ptIndex << 12);
}

/**
 * @brief 将虚拟地址转为页目录页表索引
 * 
 * @param virtualAddress 虚拟地址
 * @param pdIndex 页目录索引
 * @param ptIndex 页表索引
 */
static inline void addressToIndex(inwox_vir_addr_t virtualAddress, size_t &pdIndex, size_t &ptIndex)
{
    assert(!(virtualAddress & 0xFFF));
    // 高10位是页目录索引
    pdIndex = virtualAddress >> 22;
    // 中间10位是页表索引
    ptIndex = (virtualAddress >> 12) & 0x3FF;
}

/**
 * @brief 初始化地址空间
 * 
 * 干如下几件事:
 * 1. 设置内核页目录
 * 2. 将启动过程所用内存取消映射
 * 3. 将段信息设置给内核空间
 */
void AddressSpace::initialize()
{
    kernelSpace = &_kernelSpace;
    kernelSpace->pageDir = (inwox_phy_addr_t)&kernelPageDirectory;

    inwox_vir_addr_t p = (inwox_vir_addr_t)&bootstrapBegin;
    while (p < (inwox_vir_addr_t)&bootstrapEnd) {
        kernelSpace->unMap(p);
        p += PAGESIZE;
    }
    // 可以将下边这个映射删掉是因为此地址为整个地址空间的最高4M，用于存放分页系统的页目录和页表，而
    // 对分页系统的访问不能使用虚拟地址，否则会产生死循环
    kernelSpace->unMap(RECURSIVE_MAPPING);
    kernelSpace->firstSegment = &userSegment;
    userSegment.next = &videoSegment;
    videoSegment.next = &readOnlySegment;
    readOnlySegment.next = &writableSegment;
    writableSegment.next = &physicalMemorySegment;
    physicalMemorySegment.next = &recursiveMappingSegment;
}

/**
 * @brief 将当前地址空间激活
 * 
 * 也就是将当前的页目录设置给%cr3
 */
void AddressSpace::activate()
{
    __asm__ __volatile__("mov %0, %%cr3" ::"r"(pageDir));
}

/**
 * @brief fork地址空间
 * 
 * 启动新进程时，fork父进程地址空间
 * 1. 创建一个新的地址空间
 * 2. 复制原地址空间内存
 * @return AddressSpace* 新地址空间
 */
AddressSpace *AddressSpace::fork()
{
    AddressSpace *result = new AddressSpace();
    MemorySegment *segment = firstSegment->next;
    while (segment) {
        // 找到未使用的segment，分配并复制父进程的地址空间内容
        if (!(segment->flags & SEG_NOUNMAP)) {
            size_t size = segment->size;
            result->mapMemory(segment->address, size, segment->flags);
            inwox_vir_addr_t source = kernelSpace->mapFromOtherAddressSpace(this, segment->address, size, PROT_READ);
            inwox_vir_addr_t dest = kernelSpace->mapFromOtherAddressSpace(result, segment->address, size, PROT_WRITE);
            memcpy((void *)dest, (const void *)source, size);
            kernelSpace->unmapPhysical(source, size);
            kernelSpace->unmapPhysical(dest, size);
        }
        segment = segment->next;
    }

    return result;
}

/**
 * @brief 通过虚拟地址获取映射的物理地址
 * 
 * @param virtualAddress 虚拟地址
 * @return inwox_phy_addr_t 映射的物理地址
 */
inwox_phy_addr_t AddressSpace::getPhysicalAddress(inwox_vir_addr_t virtualAddress)
{
    size_t pdIndex;
    size_t ptIndex;
    addressToIndex(virtualAddress, pdIndex, ptIndex);
    uintptr_t *pageDirectory = (uintptr_t *)pageDirMapped;
    if (!pageDirectory[pdIndex]) {
        return 0;
    }
    uintptr_t *pageTable;
    // 内核页表页目录常驻内存，用户态页表使用时分配，用完回收
    if (this == kernelSpace) {
        pageTable = (uintptr_t *)(RECURSIVE_MAPPING + PAGESIZE * pdIndex);
    } else {
        pageTable = (uintptr_t *)kernelSpace->map(pageDirectory[pdIndex] & ~0xFFF, PROT_READ);
    }
    inwox_phy_addr_t result = pageTable[ptIndex] & ~0xFFF;

    if (this != kernelSpace) {
        kernelSpace->unMap((inwox_vir_addr_t)pageTable);
    }
    return result;
}

/**
 * @brief 将物理页映射到虚拟地址
 * 
 * @param physicalAddress 物理内存开始位置
 * @param protection 保护位（包括读（PROT_READ）、写（PROT_WRITE）、执行（PROT_EXEC）及无权限（PROT_NONE））
 * @return inwox_vir_addr_t 映射后的虚拟地址
 */
inwox_vir_addr_t AddressSpace::map(inwox_phy_addr_t physicalAddress, int protection)
{
    assert(this == kernelSpace);
    inwox_vir_addr_t address = MemorySegment::findFreeSegment(firstSegment, PAGESIZE);
    return mapAt(address, physicalAddress, protection);
}

/**
 * @brief 将物理内存（页大小）映射到指定的虚拟内存，此操作会写页目录页表
 * 
 * @param virtualAddress 虚拟内存 4K对齐
 * @param physicalAddress 物理内存 4K对齐
 * @param protection 映射方式（包括读（PROT_READ）、写（PROT_WRITE）、执行（PROT_EXEC）及无权限（PROT_NONE））
 * @return inwox_vir_addr_t 映射后的虚拟地址
 */
inwox_vir_addr_t AddressSpace::mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, int protection)
{
    size_t pdIndex;
    size_t ptIndex;
    addressToIndex(virtualAddress, pdIndex, ptIndex);
    return mapAt(pdIndex, ptIndex, physicalAddress, protection);
}


/**
 * @brief 将物理地址映射到指定的页目录页表，此操作会写页目录页表
 * 
 * @param pdIndex 页目录索引
 * @param ptIndex 页表索引
 * @param physicalAddress 待映射物理地址，4K对齐
 * @param protection 映射方式（包括读（PROT_READ）、写（PROT_WRITE）、执行（PROT_EXEC）及无权限（PROT_NONE））
 * @return inwox_vir_addr_t 映射后的虚拟地址
 */
inwox_vir_addr_t AddressSpace::mapAt(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, int protection)
{ 
    assert(!(protection & ~_PROT_FLAGS));
    assert(!(physicalAddress & 0xFFF));

    int flags = protectionToFlags(protection);

    if (this != kernelSpace) {
        // 用户空间分配的内存全部标记为用户可访问
        flags |= PAGE_USER;
    }

    return mapAtWithFlags(pdIndex, ptIndex, physicalAddress, flags);
}

/**
 * @brief 将物理地址设置到页表页目录中，返回对应的虚拟地址
 * 
 * @param pdIndex 页目录索引
 * @param ptIndex 页表索引
 * @param physicalAddress 待映射物理地址，4K对齐
 * @param flags 映射方式
 * @return inwox_vir_addr_t 映射后的虚拟地址
 */
inwox_vir_addr_t AddressSpace::mapAtWithFlags(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress,
                                              int flags)
{
    // 只可以使用低12位
    assert(!(flags & ~0xFFF));
    uintptr_t *pageDirectory = (uintptr_t *)pageDirMapped;
    uintptr_t *pageTable = nullptr;

    if (this == kernelSpace) {  // 内核页目录存放在地址空间最高4K（0xFFFF F000），且常驻内存
        pageTable = (uintptr_t *)(RECURSIVE_MAPPING + PAGESIZE * pdIndex);
    }

    // 若页表还未分配，则分配一个页的作为页表，并设置到页目录中
    if (!pageDirectory[pdIndex]) {
        inwox_phy_addr_t pageTablePhys = PhysicalMemory::popPageFrame();
        int pdFlags = PAGE_PRESENT | PAGE_WRITABLE;
        // 对于用户空间的操作,都加上用户可用标识
        if (this != kernelSpace) {
            pdFlags |= PAGE_USER;
        }
        pageDirectory[pdIndex] = pageTablePhys | pdFlags;
        if (this != kernelSpace) {  // 对于用户地址空间，直接将申请到的物理内存映射一个虚拟地址作为页表
            pageTable = (uintptr_t *)kernelSpace->map(pageTablePhys, PROT_READ | PROT_WRITE);
        }
        // 注意新分配的页表需要清零
        memset(pageTable, 0, PAGESIZE);

        // 对于内核地址空间，需要在全部地址空间映射这个页表
        if (this == kernelSpace) {
            AddressSpace *addressSpace = firstAddressSpace;
            while (addressSpace) {
                uintptr_t *pd = (uintptr_t *)addressSpace->pageDirMapped;
                pd[pdIndex] = pageTablePhys | PAGE_PRESENT | PAGE_WRITABLE;
                addressSpace = addressSpace->next;
            }
        }
    // 对于已经存在的页表，只需找到对应的页表虚拟地址
    } else if (this != kernelSpace) {
        pageTable = (uintptr_t *)kernelSpace->map(pageDirectory[pdIndex] & ~0xFFF, PROT_READ | PROT_WRITE);
    }

    // 将物理地址设置到页表中
    pageTable[ptIndex] = physicalAddress | flags;

    // 对于用户空间,每次映射完内存,将页目录和页表释放掉
    // 内核的页目录页表常驻内存最高区域
    if (this != kernelSpace) {
        kernelSpace->unMap((inwox_vir_addr_t)pageTable);
    }
    inwox_vir_addr_t virtualAddress = IndexToaddress(pdIndex, ptIndex);

    // 由于虚拟地址指向的物理地址改变了，所以要更新TLB
    __asm__ __volatile__("invlpg (%0)" ::"r"(virtualAddress));

    return virtualAddress;
}

/**
 * @brief 复制地址空间的一部分
 * 
 * 将地址空间的一段虚拟内存复制到新地址空间，返回新地址空间对应的虚拟地址
 * 
 * @param sourceSpace 源地址空间
 * @param sourceVirtualAddress 源地址空间待复制虚拟内存开始地址
 * @param size 要复制的大小
 * @param protection 复制模式
 * @return inwox_vir_addr_t 复制后新地址空间的虚拟地址
 */
inwox_vir_addr_t AddressSpace::mapFromOtherAddressSpace(AddressSpace *sourceSpace,
                                                        inwox_vir_addr_t sourceVirtualAddress, size_t size,
                                                        int protection)
{
    inwox_vir_addr_t destination = MemorySegment::findAndAddNewSegment(firstSegment, size, protection);

    for (size_t i = 0; i < size; i += PAGESIZE) {
        inwox_phy_addr_t physicalAddress = sourceSpace->getPhysicalAddress(sourceVirtualAddress + i);
        if (!physicalAddress || !mapAt(destination + i, physicalAddress, protection)) {
            return 0;
        }
    }

    return destination;
}

/**
 * @brief 分配虚拟内存
 * 
 * @param size 待分配内存大小
 * @param protection 内存访问权限，PROT_READ/PROT_WRITE/PROT_EXEC/PROT_NONE，必须指定至少一个权限
 * @return inwox_vir_addr_t 返回分配好的虚拟内存
 */
inwox_vir_addr_t AddressSpace::mapMemory(size_t size, int protection)
{
    inwox_vir_addr_t virtualAddress = MemorySegment::findAndAddNewSegment(firstSegment, size, protection);
    inwox_phy_addr_t physicalAddress;
    for (size_t i = 0; i < size; i+=PAGESIZE) {
        physicalAddress = PhysicalMemory::popPageFrame();
        if (!physicalAddress || !mapAt(virtualAddress + i, physicalAddress, protection)) {
            return 0;
        }
    }
    return virtualAddress;
}

/**
 * @brief 映射虚拟内存到物理内存
 * 
 * 将传入的虚拟内存地址分配一个物理内存地址，并返回虚拟地址
 *  
 * @param virtualAddress 
 * @param size 
 * @param protection 
 * @return inwox_vir_addr_t 
 */
inwox_vir_addr_t AddressSpace::mapMemory(inwox_vir_addr_t virtualAddress, size_t size, int protection)
{
    MemorySegment::addSegment(firstSegment, virtualAddress, size, protection);
    inwox_phy_addr_t physicalAddress;

    for (size_t i = 0; i < size; i += PAGESIZE) {
        physicalAddress = PhysicalMemory::popPageFrame();
        if (!physicalAddress || !mapAt(virtualAddress + i, physicalAddress, protection)) {
            return 0;
        }
    }

    return virtualAddress;
}


/**
 * @brief 将指定的物理内存映射到虚拟地址空间
 * 
 * @param physicalAddress 物理内存开始地址
 * @param size 物理内存大小
 * @param protection 映射保护位
 * @return inwox_vir_addr_t 映射后的虚拟地址
 */
inwox_vir_addr_t AddressSpace::mapPhysical(inwox_phy_addr_t physicalAddress, size_t size, int protection)
{
    inwox_vir_addr_t virtualAddress = MemorySegment::findAndAddNewSegment(firstSegment, size, protection);
    for (size_t i = 0; i < size; i += PAGESIZE) {
        if (!mapAt(virtualAddress + i, physicalAddress + i, protection)) {
            return 0;
        }
    }

    return virtualAddress;
}

/**
 * @brief 取消内存页映射
 * 
 * @param virtualAddress 待操作内存开始地址
 */
void AddressSpace::unMap(inwox_vir_addr_t virtualAddress)
{
    size_t pdIndex, ptIndex;
    addressToIndex(virtualAddress, pdIndex, ptIndex);
    mapAtWithFlags(pdIndex, ptIndex, 0, 0);
}

/**
 * @brief 取消内存映射
 * 
 * 按页进行释放，释放后内存归还物理内存管理器，可再次分配给其他调用
 * 同时从段链中移除此段空间
 * 
 * @param virtualAddress 开始地址，必须是页面的整数倍
 * @param size 待取消映射的内存长度，不需要页对齐
 */
void AddressSpace::unmapMemory(inwox_vir_addr_t virtualAddress, size_t size)
{
    for (size_t i = 0; i < size; i += PAGESIZE) {
        inwox_phy_addr_t physicalAddress = getPhysicalAddress(virtualAddress + i);
        unMap(virtualAddress + i);
        PhysicalMemory::pushPageFrame(physicalAddress);
    }

    MemorySegment::removeSegment(firstSegment, virtualAddress, size);
}

/**
 * @brief 取消内存映射
 * 
 * 只将映射取消（虚拟地址不能访问物理地址），但不归还物理内存
 * 
 * @param virtualAddress 
 * @param size 
 */
void AddressSpace::unmapPhysical(inwox_vir_addr_t virtualAddress, size_t size)
{
    for (size_t i = 0; i < size; i += PAGESIZE) {
        unMap(virtualAddress + i);
    }

    MemorySegment::removeSegment(firstSegment, virtualAddress, size);
}

/**
 * @brief 分配内存，用于libk
 * 
 * 对于libk分配内存，直接使用kernelSpace分配内存即可分配指定大小的连续虚拟内存
 * 
 * @param size 分配内存的大小，单位字节
 * @return void* 返回已分配的虚拟内存
 */
extern "C" void *__mapMemory(size_t size)
{
    return (void *)kernelSpace->mapMemory(size, PROT_READ | PROT_WRITE);
}

/**
 * @brief 解除内存的分配，用于libk
 * 
 * 对于libk分配内存，直接使用kernelSpace内存映射即可
 * 
 * @param addr 待解除映射的虚拟内存地址
 * @param size 内存的大小，单位字节
 */
extern "C" void __unmapMemory(void *addr, size_t size)
{
    kernelSpace->unmapMemory((inwox_vir_addr_t)addr, size);
}
