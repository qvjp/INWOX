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
 * kernel/include/inwox/kernel/addressSpace.h
 * 定义AddressSpace类
 */

#ifndef KERNEL_ADDRESSSPACE_H__
#define KERNEL_ADDRESSSPACE_H__

#include <stddef.h> /* size_t */
#include <inwox/kernel/inwox.h> /* inwox_phy_addr_t inwox_vir_addr_t */

#define RECURSIVE_MAPPING 0xFFC00000

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4

/**
 * 获取linker.ld中
 * bootstapBegin
 * bootstrapEnd
 * kernelPhysicalBegin
 * kernelPhysicalEnd
 * 的地址
 */
extern "C"
{
    void bootstrapBegin();
    void bootstrapEnd();
    void kernelPhysicalBegin();
    void kernelPhysicalEnd();
    void kernelPageDirectory();
}

/**
 * 地址空间类
 */ 
class AddressSpace
{
    public:
        void activate();
        inwox_vir_addr_t allocate(size_t pages);
        AddressSpace* fork();
        void free(inwox_vir_addr_t virtualAddress, size_t pages);
        inwox_phy_addr_t getPhysicalAddress(inwox_vir_addr_t virtualAddress);
        bool isFree(inwox_vir_addr_t virtualAddress);
        bool isFree(size_t pdOffset, size_t ptOffset);

        inwox_vir_addr_t map(inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapAt(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapRange(inwox_phy_addr_t* physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapRange(inwox_phy_addr_t firstPhysicalAddress, size_t page_number, uint16_t flags);
        inwox_vir_addr_t mapRangeAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t* physicalAddress, uint16_t flags);

        void unMap(inwox_vir_addr_t virtualAddress);
        void unMapRange(inwox_vir_addr_t firstVirtualAddress, size_t page_number);
        static void initialize();
    private:
        /**
         * 每个进程对应一个地址空间，每个地址空间都有自己的页表，使得某逻辑地址对应于某个物理地址。
         * 正因为每个进程都有自己的页表，才使相同的逻辑地址映射到不同的物理内存。每次创建新进程（地址空间）
         * 都会将fork父进程的页表，然后更新%cr3寄存器
         */
        inwox_phy_addr_t pageDir;
        AddressSpace* next;

    private:
        AddressSpace();
        static AddressSpace _kernelSpace;
};
/* 将内核地址空间设置为全局变量 */
extern AddressSpace* kernelSpace;

#endif /* KERNEL_ADDRESSSPACE_H__ */