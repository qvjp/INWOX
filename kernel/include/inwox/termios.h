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

/**
 * kernel/include/inwox/termios.h
 * Terminal I/O
 */

#ifndef INWOX_TERMIOS_H_
#define INWOX_TERMIOS_H_

// c_lflag
#define ECHO   (1 << 0) // 回显输入字符到终端设备，在规范模式和非规范模式均可使用
#define ICANON (1 << 1) // 规范输入 (canonical mode)，输入字符被装配成行，当一行已经输入后，终端驱动程序返回
                        // 或者遇到以下条件返回：
                        // 1. 所请求的字节数已读到时，读返回。无需读一个完整的行，如果只读了行的一部分，下次读从上一次停止处开始
                        // 2. 当读到一个行定界符时，读返回。
                        // 3. 信号
                        //
                        // 在关闭了规范输入后，系统将在读到指定量的数据或超过给定的时间后返回，包括c_cc中两个变量：VMIN、VTIME

// c_cc
#define VEOF   0  // 文件结束
#define VEOL   1  // 行结束
#define VERASE 2  // 向前擦除字符
#define VINTR  3  // 中断信号
#define VKILL  4  // 擦行
#define VMIN   5  // 读返回前最小的字节数
#define VQUIT  6  // 退出信号
#define VSTART 7  // 恢复输入
#define VSTOP  8  // 停止输入
#define VSUSP  9  // 挂起信号（SIGTSTP）
#define VTIME  10 // 等待数据到达的分秒数（1/10s)
#define NCCS   11

// tcsetattr
#define TCSAFLUSH 0

typedef unsigned char cc_t;
typedef unsigned int tcflag_t;

// 终端设备特性
struct termios {
    tcflag_t c_iflag; // input flags
    tcflag_t c_oflag; // output flags
    tcflag_t c_cflag; // control flags
    tcflag_t c_lflag; // local flags，用于修改驱动程序和用户之间的接口，例如打开回显、可视删除字符等
    cc_t c_cc[NCCS];  // control characters，包含所有可以更改的特殊字符
};

#endif /* INWOX_TERMIOS_H_ */
