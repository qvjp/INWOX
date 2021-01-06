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

/* kernel/include/inwox/kernel/vnode.h
 * Vnode class.
 *
 * Vnode 是个啥？
 * Vnode是一个抽象，提供统一的操作接口，是UNIX世界的经典概念。而一切皆文件是UNIX的另一个支柱性理念，
 * 这也正好可以统一起来，在Vnode中定义文件的多种通用操作方式，具体到每种设备/文件只要继承Vnode，实
 * 现自己的具体功能就可以。另外要注意，Vnode和Inode是不同的两个概念，Inode是描述具体文件信息的结构。
 *
 */

#ifndef KERNEL_VNODE_H_
#define KERNEL_VNODE_H_

#define __need_size_t
#define __need_ssize_t
#define __need_off_t
#define __need_mode_t

#include <inwox/stat.h>
#include <sys/types.h>

class Vnode {
public:
    virtual bool isSeekable();
    virtual Vnode *openat(const char *path, int flags, mode_t mode);
    virtual ssize_t pread(void *buffer, size_t size, off_t offset); /* pread的`p`是positional，从指定位置读 */
    virtual ssize_t write(const void *buffer, size_t size);
    virtual ssize_t read(void *buffer, size_t size);
    virtual ssize_t readdir(unsigned long offset, void *buffer, size_t size);
    virtual int stat(struct stat *result);
    virtual int tcgetattr(struct termios *termios);
    virtual int tcsetattr(int flags, const struct termios *termio);
    virtual ~Vnode() {}

public:
    mode_t mode;

protected:
    Vnode(mode_t mode);
};

#endif /* KERNEL_VNODE_H_ */
