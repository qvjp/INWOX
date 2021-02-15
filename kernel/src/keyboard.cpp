/** MIT License
 *
 * Copyright (c) 2020 - 2021 Qv Junping
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
#include <inwox/keyboardkeys.h>

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

#define CSI "\e["
static const struct {
    int key;
    const char *sequence;
} sequences[] = {
    { S1_KB_UP, CSI"A"},
    { S1_KB_DOWN, CSI"B" },
    { S1_KB_RIGHT, CSI"C" },
    { S1_KB_LEFT, CSI"D" },
    { S1_KB_HOME, CSI"1~" },
    { S1_KB_INSERT, CSI"2~" },
    { S1_KB_DELETE, CSI"3~" },
    { S1_KB_END, CSI"4~" },
    { S1_KB_PAGEUP, CSI"5~" },
    { S1_KB_PAGEDOWN, CSI"6~" },
    { S1_KB_F1, CSI"OP" },
    { S1_KB_F2, CSI"OQ" },
    { S1_KB_F3, CSI"OR" },
    { S1_KB_F4, CSI"OS" },
    { S1_KB_F5, CSI"15~" },
    { S1_KB_F6, CSI"17~" },
    { S1_KB_F7, CSI"18~" },
    { S1_KB_F8, CSI"19~" },
    { S1_KB_F9, CSI"20~" },
    { S1_KB_F10, CSI"21~" },
    { S1_KB_F11, CSI"23~" },
    { S1_KB_F12, CSI"24~" },
    { 0, NULL }
};

char Keyboard::getCharFromKey(int key)
{
    static bool leftShift = false;
    static bool rightShift = false;
    static bool capsLock = false;
    static bool leftControl = false;
    static bool rightControl = false;

    if (key == S1_KB_LSHIFT) {
        leftShift = true;
    } else if (key == S1_KB_RSHIFT) {
        rightShift = true;
    } else if (key == S1_KB_CAPSLOCK) {
        capsLock = !capsLock;
    } else if (key == S1_KB_LCONTROL) {
        leftControl = true;
    } else if (key == S1_KB_RCONTROL) {
        rightControl = true;
    } else if (key == -S1_KB_LSHIFT) {
        leftShift = false;
    } else if (key == -S1_KB_RSHIFT) {
        rightShift = false;
    } else if (key == -S1_KB_LCONTROL) {
        leftControl= false;
    } else if (key == -S1_KB_RCONTROL) {
        rightControl = false;
    }

    if (key < 0) {
        return '\0';
    }

    char result = '\0';

    size_t index = key << 1 | ((leftShift || rightShift) ^ capsLock);
    if (index < sizeof(KBLAYOUT_US)) {
        result = KBLAYOUT_US[index];
    } else if (key == S1_KB_NUMPAD_ENTER) {
        result = '\n';
    } else if (key == S1_KB_NUMPAD_DIV) {
        result = '/';
    }

    if (leftControl || rightControl) {
        result = result & 0x1F;
    }

    return result;
}

const char *Keyboard::getSequenceFromkey(int key)
{
    for (size_t i = 0; sequences[i].key != 0; i++) {
        if (sequences[i].key == key) {
            return sequences[i].sequence;
        }
    }
    return NULL;
}
