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

#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/print.h> /* Print::printf() */

/**
 * 通过页目录和页表号找到对应的虚拟地址
 */
static inline inwox_vir_addr_t offset2address(size_t pdOffset, size_t ptOffset)
{
    return (pdOffset << 22) | (ptOffset << 12);
}

/**
 * 虚拟地址转换到对应的页目录号和页表号
 */
static inline void address2offset(inwox_vir_addr_t virtualAddress, size_t& pdOffset, size_t& ptOffset)
{
    /* 高10位是页目录偏移 */
    pdOffset = virtualAddress >> 22;
    /* 中间10位是页表偏移 */
    ptOffset = (virtualAddress >> 12) & 0x3FF;
}

AddressSpace AddressSpace::_kernelSpace;
AddressSpace* kernelSpace;

AddressSpace::AddressSpace()
{
}

/**
 * 初始化地址空间
 * 在开启分页时曾经将内核虚拟地址映射到其物理地址（identity mapping）
 * 分页开启后这个映射不再需要，取消这个映射。
 * 
 */
void AddressSpace::initialize()
{
    kernelSpace = &_kernelSpace;
    inwox_vir_addr_t p = (inwox_vir_addr_t) &bootstrapBegin;

    while (p < (inwox_vir_addr_t) &bootstrapEnd)
    {
        kernelSpace->unMap(p);
        p += 0x1000;
    }
}

/**
 * 获取虚拟地址对应的物理地址
 */
inwox_phy_addr_t AddressSpace::getPhysicalAddress(inwox_vir_addr_t virtualAddress)
{
    size_t pdOffset;
    size_t ptOffset;
    address2offset(virtualAddress, pdOffset, ptOffset);

    if (this == kernelSpace)
    {
        /* 内核态页目录从RECURSIVE_MAPPING + 0x3FF000开始 */
        uintptr_t* pageDirItem = (uintptr_t*)(RECURSIVE_MAPPING + 0x3FF000 + 4 * pdOffset);
        if (*pageDirItem)
        {
            uintptr_t* pageTableItem = (uintptr_t*)(RECURSIVE_MAPPING + 0x1000 * pdOffset + 4 * ptOffset);
            return *pageTableItem & ~0xFFF;
        }
        else
        {
            Print::printf("Error: Page Table does not exits.\n");
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/**
 * 将物理地址映射到虚拟地址
 * flags 0x3是可读可写，0x1是只读，0x0不可用
 */
inwox_vir_addr_t AddressSpace::map(inwox_phy_addr_t physicalAddress, uint16_t flags)
{
    /* 在高一半（PDE 768-1023）找到没有映射的页，并映射 */
    for (size_t pdOffset = 0x300; pdOffset < 0x400; pdOffset++)
    {
        for (size_t ptOffset = 0; ptOffset < 0x400; ptOffset++)
        {
            uintptr_t* pageTableItem = (uintptr_t*)(RECURSIVE_MAPPING + 0x1000 * pdOffset + 4 * ptOffset);
            if (!*pageTableItem)
            {
                return mapAt(pdOffset, ptOffset, physicalAddress, flags);
            }
        }
    }
    return 0;
}

/**
 * 下边两个函数是将虚拟地址映射和物理地址映射起来
 * 分别通过具体的虚拟地址和通过PDE、PTE
 * flags和map函数一样，都是0x3是可读可写，0x1是只读，0x0不可用
 */
inwox_vir_addr_t AddressSpace::mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, uint16_t flags)
{
    size_t pdOffset;
    size_t ptOffset;
    address2offset(virtualAddress, pdOffset, ptOffset);
    return mapAt(pdOffset, ptOffset, physicalAddress, flags);
}

inwox_vir_addr_t AddressSpace::mapAt(size_t pdOffset, size_t ptOffset, inwox_phy_addr_t physicalAddress, uint16_t flags)
{
    if (this == kernelSpace) {
        /* 页目录从0xFFFFF000（RECURSIVE_MAPPING+0x3FF000）开始 */
        uintptr_t* pageDirItem = (uintptr_t*)(RECURSIVE_MAPPING + 0x3FF000 + 4 * pdOffset);
        if (*pageDirItem) {
            uintptr_t* pageTableItem = (uintptr_t*)(RECURSIVE_MAPPING + 0x1000 * pdOffset + 4 * ptOffset);
            *pageTableItem = physicalAddress | flags;
        } else {
            /* TODO: 分配新页目录项 */
            Print::printf("Error: Page Table does not exist.\n");
            return 0;
        }
    } else {
        /* 现在只有内核空看，稍后实现其他地址空间的操作 */
        return 0;
    }

    inwox_vir_addr_t virtualAddress = offset2address(pdOffset, ptOffset);

    /* 由于虚拟地址指向的物理地址改变了，所以要更新TLB */
    __asm__ __volatile__ ("invlpg (%0)" :: "r"(virtualAddress));

    return virtualAddress;
}

/**
 * 取消虚拟地址到物理地址映射，通过将其映射到物理地址0，并将flags设为0.
 */
void AddressSpace::unMap(inwox_vir_addr_t virtualAddress)
{
    mapAt(virtualAddress, 0, 0);
}