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
 * libc/src/unistd/access.c
 * 测试文件是否拥有某权限
 */

#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

int access(const char *path, int mode)
{
    struct stat st;
    if (stat(path, &st) < 0) {
        return -1;
    }
    bool accessible = true;
    if (mode & R_OK) {
        accessible &= (st.st_mode & (S_IRUSR | S_IRGRP | S_IROTH));
    }
    if (mode & W_OK) {
        accessible &= (st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH));
    }
    if (mode & X_OK) {
        accessible &= (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
    }

    return accessible ? 0 : -1;
}
