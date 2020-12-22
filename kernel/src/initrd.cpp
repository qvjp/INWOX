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
 * kernel/src/initrd.cpp
 * Init ram disk
 */

#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <tar.h>
#include <inwox/kernel/file.h>
#include <inwox/kernel/initrd.h>
#include <inwox/kernel/print.h>

struct TarHeader {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

DirectoryVnode *Initrd::loadInitrd(inwox_vir_addr_t initrd)
{
    DirectoryVnode *root = new DirectoryVnode(nullptr, 0755);
    TarHeader *header = (TarHeader *)initrd;

    while (strcmp(header->magic, TMAGIC) == 0) {
        size_t size = (size_t)strtoul(header->size, NULL, 8);  // 获取当前文件的大小，注意按8进制读取
        char *path;

        // 获取文件路径（包含文件名）
        if (header->prefix[0]) {
            path = (char *)malloc(strlen(header->name) + strlen(header->prefix) + 2);

            stpcpy(stpcpy(stpcpy(path, header->prefix), "/"), header->name);
        } else {
            path = strdup(header->name);
        }

        // 获取文件名、所在目录（绝对路径）
        char *path2 = strdup(path);
        char *dirName = dirname(path);
        char *fileName = basename(path2);

        // 打开该文件所在目录
        DirectoryVnode *directory = (DirectoryVnode *)root->openat(dirName, 0, 0);

        if (!directory) {
            Print::printf("Could not add '%s' to nonexistent directory '%s'.\n", fileName, dirName);
            return root;
        }

        // 读取文件（标准文件或目录）到newFile并给header加上偏移量
        Vnode *newFile;
        mode_t mode = strtol(header->mode, nullptr, 8);
        if (header->typeflag == REGTYPE || header->typeflag == AREGTYPE) {
            newFile = new FileVnode(header + 1, size, mode);
            header += 1 + ALIGN_UP(size, 512) / 512;
        } else if (header->typeflag == DIRTYPE) {
            newFile = new DirectoryVnode(directory, mode);
            header++;
        } else {
            Print::printf("Unknown typeflag '%c'\n", header->typeflag);
            return root;
        }

        // 将文件添加到目录
        directory->addChildNode(fileName, newFile);

        free(path);
        free(path2);
    }

    return root;
}
