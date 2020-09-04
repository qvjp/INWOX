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
 * kernel/include/inwox/kernel/multiboot.h
 * 定义multiboot各信息
 */

#ifndef KERNEL_MULTIBOOT_H_
#define KERNEL_MULTIBOOT_H_

#include <stdint.h> /* uint32_t uint64_t */

/**
 * 检查MULTIBOOT信息中flags域中某位是否存在，
 * 存在为1，不存在为0
 */
#define CHECK_MULTIBOOT_FLAG(flags, bit) ((flags) & (1 << (bit)))

struct multiboot_elf_section_header_table {
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

/*************      Multiboot information      ******************
             +-------------------+
     0       | flags             |    (required)
             +-------------------+
     4       | mem_lower         |    (present if flags[0] is set) 单位KB，低内存从0开始
     8       | mem_upper         |    (present if flags[0] is set) 高内存从1M开始
             +-------------------+
     12      | boot_device       |    (present if flags[1] is set)
             +-------------------+
     16      | cmdline           |    (present if flags[2] is set)
             +-------------------+
     20      | mods_count        |    (present if flags[3] is set)
     24      | mods_addr         |    (present if flags[3] is set)
             +-------------------+
     28 - 40 | syms              |    (present if flags[4] or
             |                   |                flags[5] is set)
             +-------------------+
     44      | mmap_length       |    (present if flags[6] is set) BIOS提供的内存映射buffer，
     48      | mmap_addr         |    (present if flags[6] is set) 由下边的multiboot_mmap_entry组成
             +-------------------+
     52      | drives_length     |    (present if flags[7] is set)
     56      | drives_addr       |    (present if flags[7] is set)
             +-------------------+
     60      | config_table      |    (present if flags[8] is set)
             +-------------------+
     64      | boot_loader_name  |    (present if flags[9] is set)
             +-------------------+
     68      | apm_table         |    (present if flags[10] is set)
             +-------------------+
     72      | vbe_control_info  |    (present if flags[11] is set)
     76      | vbe_mode_info     |
     80      | vbe_mode          |
     82      | vbe_interface_seg |
     84      | vbe_interface_off |
     86      | vbe_interface_len |
             +-------------------+
     88      | framebuffer_addr  |    (present if flags[12] is set)
     96      | framebuffer_pitch |
     100     | framebuffer_width |
     104     | framebuffer_height|
     108     | framebuffer_bpp   |
     109     | framebuffer_type  |
     110-115 | color_info        |
             +-------------------+
*/
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;   /* BIOS所给可用内存 */
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;  /* 和内核一起被加载的模块数量 */
    uint32_t mods_addr;   /* 第一个模块的地址 模块的结构在下边的multiboot_mod_list中定义 */
    multiboot_elf_section_header_table elf_sec;
    uint32_t mmap_length; /* 内存映射缓存 */
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
};

/**
 * multiboot_mmap_entry是multiboot_info中mmap_addr和
 * mmap_length所指内存的组成单位
 *
 * size是这个结构的大小，单位字节
 * base_addr开始地址
 * length是这块内存区域的大小，单位字节
 * type是这块内存的种类
 *     1 可用的RAM
 *     3 保存ACPI信息的可用内存
 *     ...
 */
struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((__packed__));

/* multiboot_mmap_entry中为可用RAM的标志 */
#define MULTIBOOT_MEMORY_AVAILABLE 1

/**
 * 和内核一起被加载的模块
 * mod_start是模块开始地址
 * mod_end模块结束地址
 * cmdline以0结尾的ASCII字符，保存与该模块相关的任何信息
 * pad bootloader设为0，OS忽略
 */
struct multiboot_mod_list {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
};

#endif /* KERNEL_MULTIBOOT_H_ */
