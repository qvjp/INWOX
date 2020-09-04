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

/* kernel/src/keyboard.cpp
 * Keyboard.
 */

#include <stddef.h>
#include <inwox/kernel/keyboard.h>

static const char KBLAYOUT_US[] = {
    /* 0 - F*/
    '`', '~',
    0, 0,
    '1', '!',
    '2', '@',
    '3', '#',
    '4', '$',
    '5', '%',
    '6', '^',
    '7', '&',
    '8', '*',
    '9', '(',
    '0', ')',
    '-', '_',
    '=', '+',
    '\b', '\b',
    ' ', ' ',
    /* 10 - 1F*/
    'q', 'Q',
    'w', 'W',
    'e', 'E',
    'r', 'R',
    't', 'T',
    'y', 'Y',
    'u', 'U',
    'i', 'I',
    'o', 'O',
    'p', 'P',
    '[', '{',
    ']', '}',
    '\n', '\n',
    0, 0,
    'a', 'A',
    's', 'S',
    /* 20 - 2F */
    'd', 'D',
    'f', 'F',
    'g', 'G',
    'h', 'H',
    'j', 'J',
    'k', 'K',
    'l', 'L',
    ';', ':',
    '\'', '"',
    '`', '~',
    0, 0,
    '\\', '|',
    'z', 'Z',
    'x', 'X',
    'c', 'C',
    'v', 'V',
    /* 30 - 3F */
    'b', 'B',
    'n', 'N',
    'm', 'M',
    ',', '<',
    '.', '>',
    '/', '?',
    0, 0,
    0, 0,
    0, 0,
    ' ', ' ',
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    /* 40 - */
};

char Keyboard::getCharFromKey(int key)
{
    static bool leftShift = false;
    static bool rightShift = false;
    static bool capsLock = false;

    if (key == 0x2A) {
        leftShift = true;
    } else if (key == 0x36) {
        rightShift = true;
    } else if (key == 0x3A) {
        capsLock = !capsLock;
    } else if (key == -0x2A) {
        leftShift = false;
    } else if (key == -0x36) {
        rightShift = false;
    }

    if (key < 0) {
        return '\0';
    }

    size_t index = key << 1 | ((leftShift || rightShift) ^ capsLock);
    if (index < sizeof(KBLAYOUT_US)) {
        return KBLAYOUT_US[index];
    }

    if (key == 0x9C) {
        return '\n';
    } else if (key == 0xB5) {
        return '/';
    }

    return '\0';
}
