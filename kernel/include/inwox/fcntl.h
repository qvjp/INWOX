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
/* kernel/include/inwox/fcntl.h
 * file control 文件控制头文件
 */

#ifndef INWOX_FCNTL_H__
#define INWOX_FCNTL_H__

#define AT_FDCWD (-1)               // 在当前工作目录获取路径名

// 作为open等的flags参数出现，下列五个只可以出现一个
#define O_EXEC (1 << 0)             // 以可执行方式打开文件（非目录）
#define O_RDONLY (1 << 1)           // 以只读方式打开
#define O_WRONLY (1 << 2)           // 以只写方式打开
#define O_RDWR (O_RDONLY | O_WRONLY)// 以读写方式打开
#define O_SEARCH O_EXEC             // 以可搜索方式打开目录（非文件），目的在于打开目录时验证其搜索权限，后续对目录的文件描述符的操作就不需要再验证了

// 作为open等的flags参数出现，可以出现多个
#define O_APPEND (1 << 3)           // 设置每次写时都追加的末尾
#define O_CLOEXEC (1 << 4)          // 把FD_CLOEXEC设置为文件描述符标志
#define O_CREAT (1 << 5)            // 如果不存在，则创建文件
#define O_DIRECTORY (1 << 6)        // 如果不是目录则返回失败
#define O_EXCL (1 << 7)             // 排他性标识，如果同时使用O_CREAT，但文件存在，则返回失败
#define O_NOCTTY (1 << 8)           // 如果要操作的path是终端设备，则不将该终端分配作为此进程的控制终端
#define O_NOFOLLOW (1 << 9)         // 如果操作的是符号链接则返回错误
#define O_NONBLOCK (1 << 10)        // 非阻塞方式操作
#define O_SYNC (1 << 11)            // 每次write等待物理IO完成，包括由该write引起的文件属性更新所需的IO
#define O_TRUNC (1 << 12)           // 如果此文件存在，而且为只读或读写成功打开，则将其截断为0
#define O_TTY_INIT (1 << 13)        // 如果打开一个还未打开的终端设备，设置非标准termios参数值
#define O_DSYNC (1 << 14)           // 每次write操作都等待物理IO完成，但是如果该写操作并不影响读取刚写入的数据，则不需要等待文件属性被更新
#define O_RSYNC (1 << 15)           // 使每一个以文件描述符为参数的read操作等待，直到所有对文件同一部分的写操作完成

#endif /* INWOX_FCNTL_H__ */
