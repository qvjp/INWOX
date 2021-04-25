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

#include <sched.h>
#include <inwox/kernel/inwox.h>
#include <inwox/kernel/terminal.h>

#define CTRL(c) ((c) & 0x1F)

Terminal terminal;

Terminal::Terminal() : Vnode(S_IFCHR, 0, 0)
{
    termio.c_iflag = 0;
    termio.c_oflag = 0;
    termio.c_cflag = 0;
    termio.c_lflag = ECHO | ICANON;

    termio.c_cc[VEOF] = CTRL('D');
    termio.c_cc[VEOL] = 0;
    termio.c_cc[VERASE] = '\b';
    termio.c_cc[VINTR] = CTRL('C');
    termio.c_cc[VKILL] = CTRL('U');
    termio.c_cc[VMIN] = 1;
    termio.c_cc[VQUIT] = CTRL('\\');
    termio.c_cc[VSTART] = CTRL('Q');
    termio.c_cc[VSTOP] = CTRL('S');
    termio.c_cc[VSUSP] = CTRL('Z');
    termio.c_cc[VTIME] = 0;

    numEof = 0;
    mutex = KTHREAD_MUTEX_INITIALIZER;
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

void TerminalBuffer::write(char c)
{
    while ((writeIndex + 1) % CIRCULAR_BUFFER_SIZE == readIndex) {
    } /* 写到读指针时停止写 */
    circularBuffer[writeIndex] = c;
    writeIndex = (writeIndex + 1) % CIRCULAR_BUFFER_SIZE;
}

char TerminalBuffer::read()
{
    char result = circularBuffer[readIndex];
    readIndex = (readIndex + 1) % CIRCULAR_BUFFER_SIZE;
    return result;
}

void TerminalBuffer::endLine() {
    lineIndex = writeIndex;
}

bool TerminalBuffer::hasIncompleteLine() {
    return lineIndex != writeIndex;
}

void Terminal::handleCharacter(char c)
{
    if (termio.c_lflag & ICANON) { // 规范模式
        if (c == termio.c_cc[VEOF]) {
            if (terminalBuffer.hasIncompleteLine()) {
                terminalBuffer.endLine();
            } else {
                numEof++;
            }
        } else if (c == termio.c_cc[VEOL]) {

        } else if (c == termio.c_cc[VERASE]) {
            if (terminalBuffer.backspace() && (termio.c_lflag & ECHO)) {
                VgaTerminal::backspace();
            }
        } else if (c == termio.c_cc[VINTR]) {

        } else if (c == termio.c_cc[VKILL]) {

        } else if (c == termio.c_cc[VQUIT]) {

        } else if (c == termio.c_cc[VSTART]) {

        } else if (c == termio.c_cc[VSTOP]) {

        } else if (c == termio.c_cc[VSUSP]) {

        } else {
            if (termio.c_lflag & ECHO) {
                VgaTerminal::printCharacterRaw(c);
            }
            terminalBuffer.write(c);
            if (c == '\n') {
                terminalBuffer.endLine();
            }
        }
    } else { // 非规范模式
        if (termio.c_lflag & ECHO) { 
            VgaTerminal::printCharacterRaw(c);
        }
        terminalBuffer.write(c);
        terminalBuffer.endLine();
    }
}

void Terminal::handleSequence(const char *sequence)
{
    // 非规范模式下才将输入序列发送到终端
    if (!(termio.c_lflag & ICANON)) {
        while (*sequence) {
            if (termio.c_lflag & ECHO) {
                VgaTerminal::printCharacterRaw(*sequence);
            }
            terminalBuffer.write(*sequence++);
        }
        terminalBuffer.endLine();
    }
}

void Terminal::onKeyboardEvent(int key)
{
    char c = Keyboard::getCharFromKey(key);
    if (c) {
        handleCharacter(c);
    } else {
        const char *sequence = Keyboard::getSequenceFromkey(key);
        if (sequence) {
            handleSequence(sequence);
        }
    }

    VgaTerminal::updateCursorPosition();
}

ssize_t Terminal::read(void *buffer, size_t size)
{
    char *buf = (char *)buffer;
    size_t readSize = 0;
    while (readSize < size) {
        while (!terminalBuffer.available() && !numEof) {
            if (termio.c_lflag & ICANON) {
                if (readSize) {
                    return readSize;
                }
            } else {
                if (readSize >= termio.c_cc[VMIN]) {
                    return readSize;
                }
            }
            sched_yield();
        }
        if (numEof) {
            if (readSize) {
                return readSize;
            }
            numEof--;
            return 0;
        }
        char c = terminalBuffer.read();
        buf[readSize] = c;
        readSize++;
        if ((termio.c_lflag & ICANON) && c == '\n') {
            break;
        }
    }
    return readSize;
}

ssize_t Terminal::write(const void *buffer, size_t size)
{
    ScopedLock lock(&mutex);
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
        numEof = 0;
    }
    return 0;
}

void Terminal::initTerminal()
{
    VgaTerminal::init();
}

void Terminal::warnTerminal()
{
    VgaTerminal::warnTerminal();
}
