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

/* kernel/include/inwox/kernel/kthead.h
 * 内核线程的实用工具和同步机制
 */

#ifndef KERNEL_KTHREAD_H_
#define KERNEL_KTHREAD_H_

#include <stddef.h>

typedef bool kthread_mutex_t;
#define KTHREAD_MUTEX_INITIALIZER false

int kthread_mutex_lock(kthread_mutex_t *mutex);
int kthread_mutex_unlock(kthread_mutex_t *mutex);

/**
 * @brief 利用c++中析构函数机制，实现代码块自动锁
 * 
 */
class ScopedLock {
public:
    ScopedLock(kthread_mutex_t *mutex) {
        this->mutex = mutex;
        if (mutex) {
            kthread_mutex_lock(mutex);
        }
    }
    ~ScopedLock() {
        if (mutex) {
            kthread_mutex_unlock(mutex);
        }
        mutex = NULL;
    }
private:
    kthread_mutex_t *mutex;
};

#endif /* KERNEL_KTHREAD_H_ */
