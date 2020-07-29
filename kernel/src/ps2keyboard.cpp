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
 * kernel/src/process.cpp
 * PS2键盘驱动
 */

#include <inwox/kernel/print.h>
#include <inwox/kernel/port.h>
#include <inwox/kernel/ps2keyboard.h>

#define KEYBOARD_ACK 0xFA
#define KEYBOARD_RESEND 0xFE

#define ESCAPED_FLAG 0xE0

#define KEYBOARD_SET_LED 0xED
#define KEYBOARD_SET_SCANCODE 0xF0
#define KEYBOARD_SET_RATE_AND_DELAY 0xF3
#define KEYBOARD_ENABLE_SCANNING 0xF4
#define KEYBOARD_DISABLE_SCANNING 0xF5
#define KEYBOARD_SET_DEFAULT_PARAMETERS 0xF6

#define LED_SCROLLLOCK 1
#define LED_NUMBERLOCK_MASK 2
#define LED_CAPSLOCK 4

static void sendKeyboardCommand(uint8_t command);
static void sendKeyboardCommand(uint8_t command, uint8_t data);

PS2Keyboard::PS2Keyboard() {
    listener = nullptr;
    sendKeyboardCommand(KEYBOARD_ENABLE_SCANNING);
    Print::printf("Keyboard enable");
}

enum {
    STATE_NORMAL,
    STATE_ESCAPED,
};

static int state = STATE_NORMAL;
static uint8_t ledState = 0;

void PS2Keyboard::irqHandler() {
    uint8_t data = Hardwarecommunication::inportb(0x60);
    int keycode;

    if (data == KEYBOARD_ACK || data == KEYBOARD_RESEND) {
        return;
    } else if (data == ESCAPED_FLAG) {
        state = STATE_ESCAPED;
    } else {
        if (state == STATE_NORMAL) {
            keycode = data & 0x7F;
        } else {
            keycode = data | 0x80;
            state = STATE_NORMAL;
        }
        bool released = data & 0x80;

        handleKey(released ? -keycode : keycode);
    }
}

void PS2Keyboard::handleKey(int keycode) {
    uint8_t newLed = ledState;
    if (keycode == 0x45) { // 
        newLed ^= LED_NUMBERLOCK_MASK;
    } else if (keycode == 0x3A) {
        newLed ^= LED_CAPSLOCK;
    } else if (keycode == 0x46) {
        newLed ^= LED_SCROLLLOCK;
    }
    if (newLed != ledState) {
        ledState = newLed;

        sendKeyboardCommand(KEYBOARD_SET_LED, ledState);
    }
    if (listener)
    {
        listener->onKeyboardEvent(keycode);
    }
}

static void sendKeyboardCommand(uint8_t command) {
    while (Hardwarecommunication::inportb(0x64) & 2);
    Hardwarecommunication::outportb(0x60, command);
}

static void sendKeyboardCommand(uint8_t command, uint8_t data) {
    while (Hardwarecommunication::inportb(0x64) & 2);
    Hardwarecommunication::outportb(0x60, command);
    while (Hardwarecommunication::inportb(0x64) & 2);
    Hardwarecommunication::outportb(0x60, data);
}