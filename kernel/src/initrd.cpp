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

#include <string.h>
#include <stdlib.h>
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
    DirectoryVnode *root = new DirectoryVnode();
    TarHeader *header = (TarHeader *)initrd;

    while (strcmp(header->magic, TMAGIC) == 0) {
        Print::printf("start header poiner %p\n", header);
        size_t size = (size_t)strtoul(header->size, NULL, 8);

        if (header->typeflag == REGTYPE || header->typeflag == AREGTYPE) {
            FileVnode *file = new FileVnode(header + 1, size);
            root->addChildNode(strdup(header->name), file);

            Print::printf("File: /%s, size = %zu\n", header->name, size);
        }

        header += 1 + ALIGN_UP(size, 512) / 512;
        Print::printf("end header poiner %p\n", header);
    }

    return root;
}
