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

/* kernel/include/inwox/kernel/terminal.h
 * Terminal class.
 */

#ifndef KERNEL_TERMINAL_H_
#define KERNEL_TERMINAL_H_

#include <inwox/termios.h>
#include <inwox/kernel/keyboard.h>
#include <inwox/kernel/kthread.h>
#include <inwox/kernel/vgaterminal.h>
#include <inwox/kernel/vnode.h>
#include <stdint.h>

#define CIRCULAR_BUFFER_SIZE 4096

class TerminalBuffer {
public:
    TerminalBuffer();
    size_t available();
    bool backspace();
    char read();
    void reset();
    void write(char c);
    void endLine();
    bool hasIncompleteLine();

private:
    char circularBuffer[CIRCULAR_BUFFER_SIZE];
    size_t readIndex;
    size_t lineIndex;
    size_t writeIndex;
};

class Terminal : public Vnode, public KeyboardListener {
public:
    Terminal();
    virtual ssize_t read(void *buffer, size_t size);
    virtual ssize_t write(const void *buffer, size_t size);
    virtual void initTerminal();
    virtual void warnTerminal();
    virtual int tcgetattr(struct termios *termios);
    virtual int tcsetattr(int flags, const struct termios *termio);

private:
    void handleCharacter(char c);
    void handleSequence(const char *sequence);
    virtual void onKeyboardEvent(int key);

private:
    TerminalBuffer terminalBuffer;
    struct termios termio;
    unsigned int numEof;
    kthread_mutex_t mutex;
};

extern Terminal terminal;

#endif /* KERNEL_TERMINAL_H_ */
