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

/* kernel/src/tiemr.cpp
 * 定时器
 */

#include <inwox/kernel/pit.h>
#include <inwox/kernel/timer.h>
#include <inwox/kernel/syscall.h>

static inline void minus(struct timespec *time, unsigned long nanoseconds)
{
    time->tv_nsec -= nanoseconds;
    while (time->tv_nsec < 0)
    {
        time->tv_sec--;
        time->tv_nsec += 1000000000L;
    }
    if (time->tv_sec < 0) {
        time->tv_sec = 0;
        time->tv_nsec = 0;
    }
}

static inline bool isZero(struct timespec time)
{
    return (time.tv_sec == 0 && time.tv_nsec == 0);
}

Timer::Timer(struct timespec time)
{
    this->time = time;
    index = 0;
}

void Timer::advance(unsigned long nanosecodes)
{
    minus(&time, nanosecodes);
}

void Timer::start()
{
    index = Pit::registerTimer(this);
}

void Timer::wait()
{
    while (!isZero(time)) {
        __asm__ __volatile__("int $0x31");
        __sync_synchronize();
    }
    Pit::deregisterTimer(index);
}

int Syscall::nanosleep(const struct timespec *request, struct timespec *remaining)
{
    Timer timer(*request);
    timer.start();
    timer.wait();

    if (remaining) { 
        remaining->tv_sec = 0;
        remaining->tv_nsec = 0;
    }
    return 0;
}
