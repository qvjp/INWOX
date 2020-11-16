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
 * kernel/src/addressspace.cpp
 * 实现地址空间的基本操作
 */

#include <assert.h>
#include <errno.h>
#include <string.h> /* memset() */
#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/physicalmemory.h> /* pushPageFrame popPageFrame */
#include <inwox/kernel/print.h>          /* Print::printf() */
#include <inwox/kernel/process.h>
#include <inwox/kernel/syscall.h>

AddressSpace AddressSpace::_kernelSpace;
AddressSpace *kernelSpace;

static AddressSpace *firstAddressSpace = nullptr;

static inline int protectionToFlags(int protection) {
    int flags = PAGE_PRESENT;
    if (protection & PROT_WRITE) {
        flags |= PAGE_WRITABLE;
    }
    return flags;
}

/**
 * 每个进程对应一个地址空间，地址空间包含两部分
 * 所使用的页目录和指向下一个地址空间的指针。
 */
AddressSpace::AddressSpace()
{
    if (this == &_kernelSpace) {
        pageDir = 0;
        firstSegment = nullptr;
        next = nullptr;
    } else {
        // 用户态新建地址空间时，先分配物理内存用来存放该地址空间的页目录，并将页目录复制过去，
        pageDir = PhysicalMemory::popPageFrame();
        inwox_vir_addr_t kernelPageDir = (RECURSIVE_MAPPING + 0x3FF000); // FFFFF000为4G地址空间的最后4K，存放页目录
        inwox_vir_addr_t newPageDir = kernelSpace->map(pageDir, PROT_WRITE);
        memcpy((void*) newPageDir, (const void*) kernelPageDir, 0x1000);
        kernelSpace->unMap(newPageDir);

        firstSegment = new MemorySegment(0, 0x1000, PROT_NONE | SEG_NOUNMAP, nullptr, nullptr);
        MemorySegment::addSegment(firstSegment, 0xC0000000, -0xC0000000, PROT_NONE | SEG_NOUNMAP);

        next = firstAddressSpace;
        firstAddressSpace = this;
    }
}

AddressSpace::~AddressSpace()
{
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

static MemorySegment segment1(0, 0xC0000000, PROT_NONE, nullptr, nullptr);
static MemorySegment segment2(0xC0000000, 0x1000, PROT_READ | PROT_WRITE, &segment1, nullptr);
static MemorySegment segment3((inwox_vir_addr_t) &kernelVirtualBegin, (inwox_vir_addr_t) &kernelVirtualEnd - (inwox_vir_addr_t) &kernelVirtualEnd,
        PROT_READ | PROT_WRITE | PROT_EXEC, &segment2, nullptr);
static MemorySegment segment4(RECURSIVE_MAPPING, -RECURSIVE_MAPPING, PROT_READ | PROT_WRITE, &segment3, nullptr);

/**
 * 通过页目录和页表号找到对应的虚拟地址
 */
static inline inwox_vir_addr_t offset2address(size_t pdOffset, size_t ptOffset)
{
    assert(pdOffset <= 0x3FF);
    assert(ptOffset <= 0x3FF);
    return (pdOffset << 22) | (ptOffset << 12);
}

/**
 * 虚拟地址转换到对应的页目录号和页表号
 */
static inline void address2offset(inwox_vir_addr_t virtualAddress, size_t &pdOffset, size_t &ptOffset)
{
    assert(!(virtualAddress & 0xFFF));
    /* 高10位是页目录偏移 */
    pdOffset = virtualAddress >> 22;
    /* 中间10位是页表偏移 */
    ptOffset = (virtualAddress >> 12) & 0x3FF;
}

/**
 * 初始化地址空间
 * 在开启分页时曾经将内核虚拟地址映射到其物理地址（identity mapping）
 * 分页开启后这个映射不再需要，取消这个映射。
 */
void AddressSpace::initialize()
{
    kernelSpace = &_kernelSpace;
    kernelSpace->pageDir = (inwox_phy_addr_t)&kernelPageDirectory;
    inwox_vir_addr_t p = (inwox_vir_addr_t)&bootstrapBegin;

    while (p < (inwox_vir_addr_t)&bootstrapEnd) {
        kernelSpace->unMap(p);
        p += 0x1000;
    }
    /**
     * 可以将下边这个映射干掉是因为第一个页表对应的内存是已知的最高4M。
     */
    kernelSpace->unMap(RECURSIVE_MAPPING);
    kernelSpace->firstSegment = &segment1;
    segment1.next = &segment2;
    segment2.next = &segment3;
    segment3.next = &segment4;
}

/**
 * 将当前地址空间激活
 * 也就是将当前的页目录设置给%cr3
 */
void AddressSpace::activate()
{
    __asm__ __volatile__("mov %0, %%cr3" ::"r"(pageDir));
}

/**
 * 启动新进程时，fork前一个进程的地址空间
 */
AddressSpace *AddressSpace::fork()
{
    AddressSpace *result = new AddressSpace();
    MemorySegment *segment = firstSegment->next;
    while (segment) {
        // 找到未使用的segment，分配并复制父进程的地质空间内容
        if (!(segment->flags & SEG_NOUNMAP)) {
            size_t size = segment->size;
            result->mapMemory(segment->address, size, segment->flags);
            inwox_vir_addr_t source = kernelSpace->mapFromOtherAddressSpace(this, segment->address, size, PROT_READ);
            inwox_vir_addr_t dest = kernelSpace->mapFromOtherAddressSpace(result, segment->address, size, PROT_WRITE);
            memcpy((void*) dest, (const void*) source, size);
            kernelSpace->unmapPhysical(source, size);
            kernelSpace->unmapPhysical(dest, size);
        }
        segment = segment->next;
    }

    return result;
}

/**
 * 获取虚拟地址对应的物理地址
 */
inwox_phy_addr_t AddressSpace::getPhysicalAddress(inwox_vir_addr_t virtualAddress)
{
    size_t pdOffset;
    size_t ptOffset;
    address2offset(virtualAddress, pdOffset, ptOffset);

    uintptr_t *pageDirectory;
    uintptr_t *pageTable = nullptr;
    inwox_phy_addr_t result = 0;
    if (this == kernelSpace) {
        /* 内核态页目录从RECURSIVE_MAPPING + 0x3FF000开始 */
        pageDirectory = (uintptr_t *)(RECURSIVE_MAPPING + 0x3FF000);
        pageTable = (uintptr_t *)(RECURSIVE_MAPPING + 0x1000 * pdOffset);
    } else {
        pageDirectory = (uintptr_t *)kernelSpace->map(pageDir, PROT_READ);
    }
    if (pageDirectory[pdOffset]) {
        if (this != kernelSpace) {
            pageTable = (uintptr_t *)kernelSpace->map(pageDirectory[pdOffset] & ~0xFFF, PROT_READ);
        }
        result = pageTable[ptOffset] & ~0xFFF;
    }
    if (this != kernelSpace) {
        if (pageTable) {
            kernelSpace->unMap((inwox_vir_addr_t)pageTable);
        }
        kernelSpace->unMap((inwox_vir_addr_t)pageDirectory);
    }
    return result;
}

/**
 * 判断所给页目录页表所指内存页是否空闲
 */
bool AddressSpace::isFree(size_t pdOffset, size_t ptOffset)
{
    if (pdOffset == 0 && ptOffset == 0) {
        return false;
    }
    uintptr_t *pageDirectory;
    uintptr_t *pageTable = nullptr;
    bool result;

    if (this == kernelSpace) {
        pageDirectory = (uintptr_t *)(RECURSIVE_MAPPING + 0x3FF000);
        pageTable = (uintptr_t *)(RECURSIVE_MAPPING + 0x1000 * pdOffset);
    } else {
        pageDirectory = (uintptr_t *)kernelSpace->map(pageDir, PROT_READ);
    }

    if (!pageDirectory[pdOffset]) {
        result = true;
    } else {
        if (this != kernelSpace) {
            pageTable = (uintptr_t *)kernelSpace->map(pageDirectory[pdOffset] & ~0xFFF, PROT_READ);
        }
        result = !pageTable[ptOffset];
    }

    if (this != kernelSpace) {
        if (pageTable) {
            kernelSpace->unMap((inwox_vir_addr_t)pageTable);
        }
        kernelSpace->unMap((inwox_vir_addr_t)pageDirectory);
    }
    return result;
}

/**
 * 将物理地址映射到虚拟地址
 * flags 0x3是可读可写，0x1是只读，0x0不可用
 */
inwox_vir_addr_t AddressSpace::map(inwox_phy_addr_t physicalAddress, int protection)
{
    size_t begin;
    size_t end;
    if (this == kernelSpace) {
        begin = 0x300;
        end = 0x400;
    } else {
        begin = 0;
        end = 0x300;
    }

    /* 找到没有映射的页，并映射 */
    for (size_t pdOffset = begin; pdOffset < end; pdOffset++) {
        for (size_t ptOffset = 0; ptOffset < 0x400; ptOffset++) {
            if (isFree(pdOffset, ptOffset)) {
                return mapAt(pdOffset, ptOffset, physicalAddress, protection);
            }
        }
    }
    return 0;
}

/**
 * 下边两个函数是将虚拟地址和物理地址映射起来
 * 分别通过具体的虚拟地址和通过PDE、PTE
 * flags和map函数一样，都是0x3是可读可写，0x1是只读，0x0不可用
 */
inwox_vir_addr_t AddressSpace::mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, int protection)
{
    size_t pdOffset;
    size_t ptOffset;
    address2offset(virtualAddress, pdOffset, ptOffset);
    return mapAt(pdOffset, ptOffset, physicalAddress, protection);
}

inwox_vir_addr_t AddressSpace::mapAt(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, int protection)
{
    assert(!(protection & ~_PROT_FLAGS));
    assert(!(physicalAddress & 0xFFF));

    int flags = protectionToFlags(protection);

    if (this != kernelSpace) {
        // Memory in user space is always accessable by user.
        flags |= PAGE_USER;
    }

    return mapAtWithFlags(pdIndex, ptIndex, physicalAddress, flags);
}

inwox_vir_addr_t AddressSpace::mapAtWithFlags(size_t pdOffset, size_t ptOffset, inwox_phy_addr_t physicalAddress, int flags)
{
    assert(!(flags & ~0xFFF));
    uintptr_t *pageDirectory;
    uintptr_t *pageTable = nullptr;

    if (this == kernelSpace) {
        /* 页目录从0xFFFFF000（RECURSIVE_MAPPING+0x3FF000）开始 */
        pageDirectory = (uintptr_t *)(RECURSIVE_MAPPING + 0x3FF000);
        pageTable = (uintptr_t *)(RECURSIVE_MAPPING + 0x1000 * pdOffset);
    } else {
        pageDirectory = (uintptr_t *)kernelSpace->map(pageDir, PROT_READ | PROT_WRITE);
    }

    if (!pageDirectory[pdOffset]) {
        inwox_phy_addr_t pageTablePhys = PhysicalMemory::popPageFrame();
        int pdFlags = PAGE_PRESENT | PAGE_WRITABLE;
        if (this != kernelSpace) {
            pdFlags |= PAGE_USER;
        }
        pageDirectory[pdOffset] = pageTablePhys | pdFlags;
        if (this != kernelSpace) {
            pageTable = (uintptr_t *)kernelSpace->map(pageTablePhys, PROT_READ | PROT_WRITE);
        }
        memset(pageTable, 0, 0x1000);

        if (this == kernelSpace) {
            AddressSpace *addressSpace = firstAddressSpace;
            while (addressSpace) {
                uintptr_t *pageDir = (uintptr_t *)map(addressSpace->pageDir, PROT_READ | PROT_WRITE);
                pageDir[pdOffset] = pageTablePhys | 0x3;
                unMap((inwox_vir_addr_t)pageDir);
                addressSpace = addressSpace->next;
            }
        }
    } else if (this != kernelSpace) {
        pageTable = (uintptr_t *)kernelSpace->map(pageDirectory[pdOffset] & ~0xFFF, PROT_READ | PROT_WRITE);
    }
    pageTable[ptOffset] = physicalAddress | flags;

    if (this != kernelSpace) {
        kernelSpace->unMap((inwox_vir_addr_t)pageTable);
        kernelSpace->unMap((inwox_vir_addr_t)pageDirectory);
    }
    inwox_vir_addr_t virtualAddress = offset2address(pdOffset, ptOffset);

    /* 由于虚拟地址指向的物理地址改变了，所以要更新TLB */
    __asm__ __volatile__("invlpg (%0)" ::"r"(virtualAddress));

    return virtualAddress;
}

inwox_vir_addr_t AddressSpace::mapFromOtherAddressSpace(AddressSpace* sourceSpace,
        inwox_vir_addr_t sourceVirtualAddress, size_t size, int protection) {
    inwox_vir_addr_t destination = MemorySegment::findFreeSegment(firstSegment, size);

    for (size_t i = 0 ; i < size; i += 0x1000) {
        inwox_phy_addr_t physicalAddress =
                sourceSpace->getPhysicalAddress(sourceVirtualAddress + i);
        if (!physicalAddress ||
                !mapAt(destination + i, physicalAddress, protection)) {
            return 0;
        }
    }

    MemorySegment::addSegment(firstSegment, destination, size, protection);

    return destination;
}

inwox_vir_addr_t AddressSpace::mapMemory(size_t size, int protection) {
    inwox_vir_addr_t virtualAddress =
            MemorySegment::findFreeSegment(firstSegment, size);
    return mapMemory(virtualAddress, size, protection);
}

inwox_vir_addr_t AddressSpace::mapMemory(inwox_vir_addr_t virtualAddress, size_t size,
        int protection) {
    inwox_phy_addr_t physicalAddress;

    for (size_t i = 0; i < size; i += 0x1000) {
        physicalAddress = PhysicalMemory::popPageFrame();
        if (!physicalAddress ||
                !mapAt(virtualAddress + i, physicalAddress, protection)) {
            return 0;
        }
    }

    MemorySegment::addSegment(firstSegment, virtualAddress, size, protection);

    return virtualAddress;
}

inwox_vir_addr_t AddressSpace::mapPhysical(inwox_phy_addr_t physicalAddress, size_t size,
        int protection) {
    inwox_vir_addr_t virtualAddress =
            MemorySegment::findFreeSegment(firstSegment, size);
    return mapPhysical(virtualAddress, physicalAddress, size, protection);
}

inwox_vir_addr_t AddressSpace::mapPhysical(inwox_vir_addr_t virtualAddress,
        inwox_phy_addr_t physicalAddress, size_t size, int protection) {
    for (size_t i = 0; i < size; i += 0x1000) {
        if (!mapAt(virtualAddress + i, physicalAddress + i, protection)) {
            return 0;
        }
    }

    MemorySegment::addSegment(firstSegment, virtualAddress, size, protection);

    return virtualAddress;
}

void AddressSpace::unMap(inwox_vir_addr_t virtualAddress) {
    size_t pdIndex, ptIndex;
    address2offset(virtualAddress, pdIndex, ptIndex);
    mapAtWithFlags(pdIndex, ptIndex, 0, 0);
}

void AddressSpace::unmapMemory(inwox_vir_addr_t virtualAddress, size_t size) {
    for (size_t i = 0; i < size; i += 0x1000) {
        inwox_phy_addr_t physicalAddress = getPhysicalAddress(virtualAddress + i);
        unMap(virtualAddress + i);
        PhysicalMemory::pushPageFrame(physicalAddress);
    }

    MemorySegment::removeSegment(firstSegment, virtualAddress, size);
}

void AddressSpace::unmapPhysical(inwox_vir_addr_t virtualAddress, size_t size) {
    for (size_t i = 0; i < size; i += 0x1000) {
        unMap(virtualAddress + i);
    }

    MemorySegment::removeSegment(firstSegment, virtualAddress, size);
}

static void* mmapImplementation(void* /*addr*/, size_t size,
        int protection, int flags, int /*fd*/, off_t /*offset*/) {
    if (size == 0 || !(flags & MAP_PRIVATE)) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    if (flags & MAP_ANONYMOUS) {
        AddressSpace* addressSpace = Process::current->addressSpace;
        return (void*) addressSpace->mapMemory(size, protection);
    }

    //TODO: Implement other flags than MAP_ANONYMOUS
    errno = ENOTSUP;
    return MAP_FAILED;
}

void *Syscall::mmap(__mmapRequest *request)
{
    return mmapImplementation(request->_addr, request->_size, request->_protection, request->_flags, request->_fd, request->_offset);
}

int Syscall::munmap(void *addr, size_t size)
{
    if (size == 0 || (inwox_vir_addr_t)addr & 0xFFF) {
        errno = EINVAL;
        return -1;
    }

    AddressSpace *addressSpace = Process::current->addressSpace;
    /* TODO: The userspace process could unmap kernel pages! */
    addressSpace->unmapMemory((inwox_vir_addr_t)addr, size);
    return 0;
}

/**
 * 下边两个函数供libk调用
 * 分配/释放大小为pages_number个页的连续内存
 */
extern "C" void *__mapMemory(size_t size)
{
    return (void*) kernelSpace->mapMemory(size, PROT_READ | PROT_WRITE);
}
extern "C" void __unmapMemory(void *addr, size_t size)
{
    kernelSpace->unmapMemory((inwox_vir_addr_t) addr, size);
}
