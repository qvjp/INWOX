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
 * kernel/src/memorysegment.cpp
 * 内存分段
 */

#include <assert.h>
#include <string.h>
#include <inwox/kernel/addressspace.h>
#include <inwox/kernel/memorysegment.h>

static char segmentsPage[0x1000] ALIGNED(0x1000) = {0};

static inline size_t getFreeSpaceAfter(MemorySegment* segment) {
    return segment->next->address - (segment->address + segment->size);
}

MemorySegment::MemorySegment(inwox_vir_addr_t address, size_t size, int flags, MemorySegment *prev, MemorySegment *next)
{
    this->address = address;
    this->size = size;
    this->flags = flags;
    this->prev = prev;
    this->next = next;
}
void MemorySegment::addSegment(MemorySegment *segment, inwox_vir_addr_t address, size_t size, int protection)
{
    MemorySegment *newSegment = allocateSegment(address, size, protection);
    addSegment(segment, newSegment);
    verifySegmentList();
}
void MemorySegment::removeSegment(MemorySegment *segment, inwox_vir_addr_t address, size_t size)
{
    MemorySegment* currentSegment = segment;

    while (currentSegment->address + currentSegment->size <= address) {
        currentSegment = currentSegment->next;
    }

    while (size) {
        if (currentSegment->address == address &&
                currentSegment->size <= size) {
            // Delete the whole segment
            address += currentSegment->size;
            size -= currentSegment->size;

            if (size < getFreeSpaceAfter(currentSegment)) {
                size = 0;
            } else {
                size -= getFreeSpaceAfter(currentSegment);
            }

            MemorySegment* next = currentSegment->next;
            if (next) {
                next->prev = currentSegment->prev;
            }
            if (currentSegment->prev) {
                currentSegment->prev->next = next;
            }

            deallocateSegment(currentSegment);
            currentSegment = next;
            continue;
        } else if (currentSegment->address == address &&
                currentSegment->size > size) {
            currentSegment->address += size;
            currentSegment->size -= size;
            size = 0;
        } else if (currentSegment->address + currentSegment->size <
                address + size) {
            size_t diff = currentSegment->address + currentSegment->size -
                    address;
            currentSegment->size -= diff;
            size -= diff;
            address += diff;
        } else {
            // Split the segment
            size_t firstSize = address - currentSegment->address;
            size_t secondSize = currentSegment->size - firstSize - size;

            MemorySegment* newSegment = allocateSegment(address + size,
                    secondSize, currentSegment->flags);

            newSegment->prev = currentSegment;
            newSegment->next = currentSegment->next;

            currentSegment->next = newSegment;
            currentSegment->size = firstSize;
        }

        if (size < getFreeSpaceAfter(currentSegment)) {
            size = 0;
        } else {
            size -= getFreeSpaceAfter(currentSegment);
        }

        currentSegment = currentSegment->next;
    }
}
inwox_vir_addr_t MemorySegment::findFreeSegment(MemorySegment *segment, size_t size)
{
    MemorySegment *currentSegment = segment;
    while (getFreeSpaceAfter(currentSegment) < size) {
        currentSegment = currentSegment->next;
        if (!currentSegment) {
            return 0;
        }
    }
    return currentSegment->address + currentSegment->size;
}
void MemorySegment::addSegment(MemorySegment *firstSegment, MemorySegment *newSegment)
{
    inwox_vir_addr_t endAddress = newSegment->address + newSegment->size;
    MemorySegment* currentSegment = firstSegment;

    while (currentSegment->next &&
            currentSegment->next->address < endAddress) {
        currentSegment = currentSegment->next;
    }

    assert(currentSegment->address + currentSegment->size <=
            newSegment->address);
    assert(!currentSegment->next ||
            currentSegment->next->address >= endAddress);

    newSegment->prev = currentSegment;
    newSegment->next = currentSegment->next;

    currentSegment->next = newSegment;
    if (newSegment->next) {
        newSegment->next->prev = newSegment;
    }
}

MemorySegment* MemorySegment::allocateSegment(inwox_vir_addr_t address, size_t size, int flags)
{
    MemorySegment* current = (MemorySegment*) segmentsPage;

    while (current->address != 0 && current->size != 0) {
        current++;
        if (((uintptr_t) current & 0xFFF) ==
                (0x1000 - 0x1000 % sizeof(MemorySegment))) {
            MemorySegment** nextPage = (MemorySegment**) current;
            assert(nextPage != nullptr);
            current = *nextPage;
        }
    }

    current->address = address;
    current->size = size;
    current->flags = flags;

    return current;
}

void MemorySegment::deallocateSegment(MemorySegment *segment)
{
    memset(segment, 0, sizeof(MemorySegment));
}

void MemorySegment::verifySegmentList()
{
    MemorySegment *current = (MemorySegment *) segmentsPage;
    MemorySegment **nextPage;

    int freeSegmentSpaceFound = 0;
    while (true) {
        if (current->address == 0 && current->size == 0) {
            freeSegmentSpaceFound++;
        }
        current++;
        if (((uintptr_t) current & 0xFFF) == (0x1000 - 0x1000 % sizeof(MemorySegment))) {
            nextPage = (MemorySegment **) current;
            if (!*nextPage) {
                break;
            }
            current = *nextPage;
        }
    }
    if (freeSegmentSpaceFound == 1) {
        *nextPage = (MemorySegment *) kernelSpace->mapMemory(0x1000, PROT_READ | PROT_WRITE);
        memset(*nextPage, 0, 0x1000);
    }
}
