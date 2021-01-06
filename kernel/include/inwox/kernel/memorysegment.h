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

/* kernel/include/inwox/kernel/memorysegment.h
 * Memory segment
 */

#ifndef KERNEL_MEMORYSEGMENT_H_
#define KERNEL_MEMORYSEGMENT_H_

#include <stddef.h>
#include <inwox/kernel/inwox.h>

#define SEG_NOUNMAP (1 << 16)

class MemorySegment {
public:
    MemorySegment(inwox_vir_addr_t address, size_t size, int flags, MemorySegment *prev, MemorySegment *next);

public:
    inwox_vir_addr_t address;
    size_t size;
    int flags;
    MemorySegment *prev;
    MemorySegment *next;

public:
    static void addSegment(MemorySegment *segment, inwox_vir_addr_t address, size_t size, int protection);
    static void removeSegment(MemorySegment *segment, inwox_vir_addr_t address, size_t size);
    static inwox_vir_addr_t findFreeSegment(MemorySegment *segment, size_t size);

private:
    static void addSegment(MemorySegment *firstSegment, MemorySegment *newSegment);
    static MemorySegment *allocateSegment(inwox_vir_addr_t address, size_t size, int flags);
    static void deallocateSegment(MemorySegment *segment);
    static void verifySegmentList();
};

#endif /* KERNEL_MEMORYSEGMENT_H_ */
