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

/* kernel/src/terminal.cpp
 * Terminal class.
 */

#include <inwox/kernel/inwox.h>
#include <inwox/kernel/terminal.h>

Terminal terminal;

Terminal::Terminal() : Vnode(S_IFCHR)
{
    termio.c_lflag = ECHO | ICANON;
    termio.c_cc[VMIN] = 1;
}

TerminalBuffer::TerminalBuffer()
{
    reset();
}

size_t TerminalBuffer::available()
{
    return lineIndex >= readIndex ? lineIndex - readIndex : readIndex - lineIndex;
}

bool TerminalBuffer::backspace()
{
    if (lineIndex == writeIndex) {
        return false;
    }
    if (likely(writeIndex != 0)) {
        writeIndex = (writeIndex - 1) % CIRCULAR_BUFFER_SIZE;
    } else {
        writeIndex = CIRCULAR_BUFFER_SIZE - 1;
    }
    return true;
}

void TerminalBuffer::reset()
{
    readIndex = 0;
    lineIndex = 0;
    writeIndex = 0;
}

void TerminalBuffer::write(char c, bool canonicalMode)
{
    while ((writeIndex + 1) % CIRCULAR_BUFFER_SIZE == readIndex) {
    } /* 写到读指针时停止写 */
    circularBuffer[writeIndex] = c;
    writeIndex = (writeIndex + 1) % CIRCULAR_BUFFER_SIZE;
    if (c == '\n' || !canonicalMode) {
        lineIndex = writeIndex;
    }
}

char TerminalBuffer::read()
{
    while (readIndex == lineIndex) {
    } /* 读到写指针时停止读 */
    char result = circularBuffer[readIndex];
    readIndex = (readIndex + 1) % CIRCULAR_BUFFER_SIZE;
    return result;
}

void Terminal::onKeyboardEvent(int key)
{
    char c = Keyboard::getCharFromKey(key);
    if (!c) {
        return;
    }
    if ((termio.c_lflag & ICANON) && c == '\b') {
        if (terminalBuffer.backspace() && (termio.c_lflag & ECHO)) {
            VgaTerminal::backspace();
        }
    } else {
        if (termio.c_lflag & ECHO) {
            VgaTerminal::printCharacterRaw(c);
        }
        terminalBuffer.write(c, termio.c_lflag & ICANON);
    }
    VgaTerminal::updateCursorPosition();
}

ssize_t Terminal::read(void *buffer, size_t size)
{
    char *buf = (char *)buffer;
    if (termio.c_cc[VMIN] == 0 && !terminalBuffer.available()) {
        return 0;
    }
    while (termio.c_cc[VMIN] > terminalBuffer.available()) {
    }

    for (size_t i = 0; i < size; i++) {
        buf[i] = terminalBuffer.read();
    }
    VgaTerminal::updateCursorPosition();
    return (ssize_t)size;
}

ssize_t Terminal::write(const void *buffer, size_t size)
{
    const char *buf = (const char *)buffer;

    for (size_t i = 0; i < size; i++) {
        VgaTerminal::printCharacter(buf[i]);
    }
    VgaTerminal::updateCursorPosition();
    return (ssize_t)size;
}

int Terminal::tcgetattr(struct termios *result)
{
    *result = termio;
    return 0;
}

int Terminal::tcsetattr(int flags, const struct termios *termio)
{
    this->termio = *termio;
    if (flags == TCSAFLUSH) {
        terminalBuffer.reset();
    }
    return 0;
}

/**
 * 初始化终端
 * 将屏幕背景设置为灰色
 */
void Terminal::initTerminal()
{
    VgaTerminal::init();
}

void Terminal::warnTerminal()
{
    VgaTerminal::warnTerminal();
}
