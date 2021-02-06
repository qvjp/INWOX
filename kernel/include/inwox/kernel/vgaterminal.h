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

/* kernel/include/inwox/kernel/vgaterminal.h
 * VGA Terminal
 */

#ifndef KERNEL_VGATERMINAL_H_
#define KERNEL_VGATERMINAL_H_

#include <stdint.h>
#include <stddef.h>
typedef uint8_t color_t;

namespace VgaTerminal {
void backspace();
void fillRect(size_t x1, size_t x2, size_t y1, size_t y2, char ch, color_t color);
void init();
void printCharacter(char c);
void printCharacterRaw(char c);
void printEscapeCode(char c);
void updateCursorPosition();
void setCursorVisibility(bool v);
void warnTerminal();
}

#endif /* KERNEL_VGATERMINAL_H_ */
