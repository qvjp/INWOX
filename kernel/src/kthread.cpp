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

/* kernel/src/kthread.cpp
 * 内核线程的实用工具和同步机制
 */

#include <sched.h>
#include <inwox/kernel/kthread.h>

static kthread_mutex_t heapLock = KTHREAD_MUTEX_INITIALIZER;

int kthread_mutex_lock(kthread_mutex_t *mutex)
{
    // gcc内置函数__atomic_test_and_set有两种语义：
    // 1. test表示先测试（读取mutex）并返回这个结果
    // 2. set标识将mutex设置为指定值（有限制）
    // 也就是说，该函数的返回值同set的结果没有关系，返回值只标识调用该函数前mutex的状态
    // 使用该机制即可实现mutex_lock和mutex_unlock，
    // 即先调用mutex_lock的线程会直接返回初始值false，继续向下执行，然后给mutex设置
    // 一个非0值，后续进入此函数的线程便会在while循环中等待，直到mutex被unlock
    while (__atomic_test_and_set(mutex, __ATOMIC_ACQUIRE)) {
        sched_yield();
    }
    return 0;
}

int kthread_mutex_unlock(kthread_mutex_t *mutex)
{
    __atomic_clear(mutex, __ATOMIC_RELEASE);
    return 0;
}

extern "C" {

void __lockHeap(void) {
    kthread_mutex_lock(&heapLock);
}

extern "C" void __unlockHeap(void) {
    kthread_mutex_unlock(&heapLock);
}

} /* extern "C" */
