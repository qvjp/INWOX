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
}

/**
 * 地址空间类
 */ 
class AddressSpace
{
    public:
        inwox_vir_addr_t allocate(size_t pages);
        void free(inwox_vir_addr_t virtualAddress, size_t pages);
        inwox_phy_addr_t getPhysicalAddress(inwox_vir_addr_t virtualAddress);

        inwox_vir_addr_t map(inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapAt(size_t pdIndex, size_t ptIndex, inwox_phy_addr_t physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapRange(inwox_phy_addr_t* physicalAddress, uint16_t flags);
        inwox_vir_addr_t mapRangeAt(inwox_vir_addr_t virtualAddress, inwox_phy_addr_t* physicalAddress, uint16_t flags);

        void unMap(inwox_vir_addr_t virtualAddress);
        static void initialize();
    private:
        AddressSpace();
        static AddressSpace _kernelSpace;
};
/* 将内核地址空间设置为全局变量 */
extern AddressSpace* kernelSpace;

#endif /* KERNEL_ADDRESSSPACE_H__ */