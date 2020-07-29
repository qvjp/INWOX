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
 * PS2控制器
 */
#include <inwox/kernel/interrupt.h>
#include <inwox/kernel/print.h>
#include <inwox/kernel/port.h>
#include <inwox/kernel/ps2.h>
#include <inwox/kernel/ps2keyboard.h>
#include <inwox/kernel/terminal.h>
#include <assert.h>

#define PS2_DATA_PORT    0x60
#define PS2_STATUS_PORT  0x64
#define PS2_COMMAND_PORT 0x64

#define COMMAND_READ_CONFIG   0x20
#define COMMAND_WRITE_CONFIG  0x60
#define COMMAND_DISABLE_PORT2 0xA7
#define COMMAND_ENABLE_PORT2  0xA8
#define COMMAND_TEST_PORT2    0xA9
#define COMMAND_SELF_TEST     0xAA
#define COMMAND_TEST_PORT1    0xAB
#define COMMAND_DISABLE_PORT1 0xAD
#define COMMAND_ENABLE_PORT1  0xAE

#define PS2_CONFIG_FIRST_INTERRUPT   (1 << 0)
#define PS2_CONFIG_SECOND_INTERRUPT  (1 << 1)
#define PS2_CONFIG_SYSTEM_FLAG       (1 << 2)
#define PS2_CONFIG_ZERO1             (1 << 3)
#define PS2_CONFIG_FIRST_CLOCK       (1 << 4)
#define PS2_CONFIG_SECOND_CLOCK      (1 << 5)
#define PS2_CONFIG_FIRST_TRANSLATION (1 << 6)
#define PS2_CONFIG_ZERO2             (1 << 7)

#define DEVICE_CMD_IDENTIFY         0xF2
#define DEVICE_CMD_DISABLE_SCANNING 0xF5
#define DEVICE_CMD_ENABLE_SCANNING  0xF4
#define DEVICE_CMD_RESET            0xFF
#define DEVICE_ACK                  0xFA
#define DEVICE_RESET_OK             0xAA

#define REG_STATUS_OUTPUT (1 << 0)
#define REG_STATUS_INPUT  (1 << 1)

static uint8_t readDataPort();
static void sendPS2Command(uint8_t command);
static void sendPS2Command(uint8_t command, uint8_t data);
static uint8_t sendPS2CommandWithResponse(uint8_t command);

static PS2Device* ps2Device1;

static void irqHandler(struct regs *r) {
    assert(r->int_no == 33);
    if (ps2Device1)
    {
        ps2Device1->irqHandler();
    }
}

void PS2::initialize() {
    // 禁用设备
    sendPS2Command(COMMAND_DISABLE_PORT1);
    sendPS2Command(COMMAND_DISABLE_PORT2);

    // 刷新输出缓存
    while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_OUTPUT) {
        Hardwarecommunication::inportb(PS2_DATA_PORT);
    }

    // 配置控制器
    uint8_t config = sendPS2CommandWithResponse(COMMAND_READ_CONFIG);
    config &= ~PS2_CONFIG_FIRST_INTERRUPT;
    config &= ~PS2_CONFIG_SECOND_INTERRUPT;
    sendPS2Command(COMMAND_WRITE_CONFIG, config);

    // 控制器自检
    uint8_t test = sendPS2CommandWithResponse(COMMAND_SELF_TEST);
    if (test != 0x55) {
        Print::printf("PS/2 self test failed (response = 0x%x)\n", test);
        return;
    }

    // 确定是否有两个通道
    bool dualChannel = false;

    if (config & (PS2_CONFIG_SECOND_CLOCK)) {
        sendPS2Command(COMMAND_ENABLE_PORT2);
        if (sendPS2CommandWithResponse(COMMAND_READ_CONFIG) & (PS2_CONFIG_SECOND_CLOCK)) {
            dualChannel = true;
        }
    }

    //  接口自检
    bool port1Exists = sendPS2CommandWithResponse(COMMAND_TEST_PORT1) == 0x00;
    bool port2Exists = dualChannel &&
            sendPS2CommandWithResponse(COMMAND_TEST_PORT2) == 0x00;

    if (!port1Exists && !port2Exists) {
        Print::printf("No usable PS/2 port found\n");
    }

    // 设备使能
    config = sendPS2CommandWithResponse(COMMAND_READ_CONFIG);
    if (port1Exists) {
        config |= PS2_CONFIG_FIRST_INTERRUPT;
    }
    if (port2Exists) {
        config |= PS2_CONFIG_SECOND_INTERRUPT;
    }
    sendPS2Command(COMMAND_WRITE_CONFIG, config);
    if (port1Exists) {
        sendPS2Command(COMMAND_ENABLE_PORT1);
    }
    if (port2Exists) {
        sendPS2Command(COMMAND_ENABLE_PORT2);
    }

    // 检测PS/2设备类型
    if (port1Exists) {
        do{
            while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
            Hardwarecommunication::outportb(PS2_DATA_PORT, DEVICE_CMD_RESET);
            if (readDataPort() != DEVICE_ACK)
                break;
            if (readDataPort() != DEVICE_RESET_OK)
                break;

            while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
            Hardwarecommunication::outportb(PS2_DATA_PORT, DEVICE_CMD_DISABLE_SCANNING);
            if (readDataPort() != DEVICE_ACK)
                break;

            while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
            Hardwarecommunication::outportb(PS2_DATA_PORT, DEVICE_CMD_IDENTIFY);
            if (readDataPort() != DEVICE_ACK)
                break;
            uint8_t id = readDataPort();
            if (id == 0xAB) {
                id = Hardwarecommunication::inportb(PS2_DATA_PORT);
                if (id == 0x41 || id == 0xC1 || id == 0x83) {
                    PS2Keyboard* keyboard = new PS2Keyboard();
                    keyboard->listener = &terminal;
                    ps2Device1 = keyboard;
                    Interrupt::isr_install_handler(33, irqHandler);
                }
            }
        } while(0);
    }
}

static uint8_t readDataPort() {
    while (!(Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_OUTPUT));
    return Hardwarecommunication::inportb(PS2_DATA_PORT);
}

static void sendPS2Command(uint8_t command) {
    while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
    Hardwarecommunication::outportb(PS2_COMMAND_PORT, command);
}

static void sendPS2Command(uint8_t command, uint8_t data) {
    while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
    Hardwarecommunication::outportb(PS2_COMMAND_PORT, command);
    while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
    Hardwarecommunication::outportb(PS2_DATA_PORT, data);
}

static uint8_t sendPS2CommandWithResponse(uint8_t command) {
    while (Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_INPUT);
    Hardwarecommunication::outportb(PS2_COMMAND_PORT, command);
    while (!(Hardwarecommunication::inportb(PS2_STATUS_PORT) & REG_STATUS_OUTPUT));
    return Hardwarecommunication::inportb(PS2_DATA_PORT);
}
