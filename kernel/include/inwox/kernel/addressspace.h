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

/* kernel/include/inwox/kernel/addressspace.h
 * 进程地址空间
 */

#ifndef KERNEL_ADDRESSSPACE_H_
#define KERNEL_ADDRESSSPACE_H_

#include <stddef.h>
#include <inwox/mman.h>
#include <inwox/kernel/inwox.h>
#include <inwox/kernel/memorysegment.h>

/**
 * 地址空间递归映射开始位置
 * 
 * 0xFFC0 0000即4092M，4G内存的最后4M用来存放页目录、页表，其对应的pd、pt为1023和0
 */
#define RECURSIVE_MAPPING 0xFFC00000

/**
 * 内存页访问权限标识，即页目录和页表项最低12位所表示的权限
 * 
 * +-----------------------------------------+-----+-+-+-+-+-+-+-+-+-+
 * |          Physical Page Address          |Avail|G|0|D|A|C|W|U|R|P|
 * +-----------------------------------------+-----+-+-+-+-+-+-+-+-+-+
 * 31                                        11    9                 0
 * 分为已分配（PAGE_PRESENT）可写（PAGE_WRITABLE）用户态可用（PAGE_USER）
 */
#define PAGE_PRESENT   (1 << 0)
#define PAGE_WRITABLE  (1 << 1)
#define PAGE_USER      (1 << 2)

/**
 * 获取linker.ld中
 * bootstapBegin
 * bootstrapEnd
 * kernelPhysicalBegin
 * kernelPhysicalEnd
 * kernelPageDirectory
 * kernelVirtualBegin
 * kernelVirtualEnd
 * 的地址
 */
extern "C" {
void bootstrapBegin();
void bootstrapEnd();
void kernelPhysicalBegin();
void kernelPhysicalEnd();
void kernelPageDirectory();
void kernelVirtualBegin();
void kernelReadOnlyEnd();
void kernelVirtualEnd();
}

/**
 * 地址空间类
 */
class AddressSpace {
public:
    AddressSpace();
    ~AddressSpace();
    static void initialize();
    void activate();
    AddressSpace *fork();
    inwox_phy_addr_t getPhysicalAddress(inwox_vir_addr_t virtualAddress);
    inwox_vir_addr_t mapFromOtherAddressSpace(AddressSpace *sourceSpace, inwox_vir_addr_t sourceVirtualAddress,
                                              size_t size, int protection);
    inwox_vir_addr_t mapMemory(size_t size, int protection);
    inwox_vir_addr_t mapMemory(inwox_vir_addr_t virtualAddress, size_t size, int protection);
    inwox_vir_addr_t mapPhysical(inwox_phy_addr_t physicalAddress, size_t size, int protection);
    inwox_vir_addr_t mapPhysical(inwox_phy_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, size_t size,
                                 int protection);
    void unmapMemory(inwox_vir_addr_t virtualAddress, size_t size);
    void unmapPhysical(inwox_vir_addr_t firstVirtualAddress, size_t size);

private:
    bool isFree(size_t pdOffset, size_t ptOffset);
    inwox_vir_addr_t map(inwox_phy_addr_t physicalAddress, int protection);
    inwox_vir_addr_t mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, int protection);
    inwox_vir_addr_t mapAt(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, int flags);
    inwox_vir_addr_t mapAtWithFlags(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, int flags);
    void unMap(inwox_vir_addr_t virtualAddress);

private:
    /**
     * 每个进程对应一个地址空间，每个地址空间都有自己的页表，使得某逻辑地址对应于某个物理地址。
     * 正因为每个进程都有自己的页表，才使相同的逻辑地址映射到不同的物理内存。每次创建新进程（地址空间）
     * 都会将fork父进程的页表，然后更新%cr3寄存器
     */
    inwox_phy_addr_t pageDir;
    /**
     * 每个进程都有自己独立的段空间，使用链表进行管理，根据首个段可以遍历整个地址空间
     */
    MemorySegment *firstSegment;
    /**
     * 地址空间间通过next指针连成一个链，在对全部地址空间都要进行一个操作时，可以用到此字段
     */
    AddressSpace *next;

private:
    static AddressSpace _kernelSpace;
};
/* 全局内核地址空间指针，指向_kernelSpace */
extern AddressSpace *kernelSpace;

#endif /* KERNEL_ADDRESSSPACE_H_ */
