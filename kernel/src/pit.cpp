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
 * kernel/src/pit.cpp
 * 初始化Programmable Interval Timer
 */

#include <inwox/kernel/interrupt.h>
#include <inwox/kernel/pit.h>
#include <inwox/kernel/port.h>
#include <inwox/kernel/print.h>

#define PIT_FREQUENCY 1193182 // Hz
#define HZ            1000    // 每秒嘀嗒次数

#define PIT_PORT_CHANNEL0 0x40
#define PIT_PORT_MODE     0x43

#define PIT_MODE_RATE_GENERATOR 0x4
#define PIT_MODE_LOBYTE_HIBYTE  0x30

static const uint16_t frequency = PIT_FREQUENCY / HZ;
static const unsigned long nanoseconds = 1000000000L / PIT_FREQUENCY * frequency;

#define NUM_TIMERS 20
static Timer *timers[NUM_TIMERS] = {0};

/**
 * @brief 从开机起嘀嗒次数
 * 18446744073709551615(UINT64_MAX) / (86400 * 365 * 1000(HZ)) = 584,942,417（年）
 */
static int64_t pit_ticker = 0;

static void irqHandler(struct context *)
{
    pit_ticker++;
    for (size_t i = 0; i < NUM_TIMERS; i++) {
        if (timers[i]) {
            timers[i]->advance(nanoseconds);
        }
    }
}

void Pit::deregisterTimer(size_t index)
{
    timers[index] = nullptr;
}

size_t Pit::registerTimer(Timer *timer)
{
    for (size_t i = 0; i < NUM_TIMERS; i++) {
        if (!timers[i]) {
            timers[i] = timer;
            return i;
        }
    }
    Print::printf("Error: Too many timers\n");
    return -1;
}

void Pit::initialize()
{
    Interrupt::isrInstallHandler(32, irqHandler);
    Hardwarecommunication::outportb(PIT_PORT_MODE, PIT_MODE_RATE_GENERATOR | PIT_MODE_LOBYTE_HIBYTE);
    Hardwarecommunication::outportb(PIT_PORT_CHANNEL0, frequency & 0xFF);
    Hardwarecommunication::outportb(PIT_PORT_CHANNEL0, (frequency >> 8) & 0xFF);
}
