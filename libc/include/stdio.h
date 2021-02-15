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
 * lib/include/stdio.h
 * 标准输入输出定义
 */

#ifndef STDIO_H
#define STDIO_H

#define __need___va_list
#include <stdarg.h>
#define __need_FILE
#define __need_fpos_t
#define __need_NULL
#define __need_size_t
#define __need_ssize_t
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct __FILE {
    int fd;
    int flags;
};

#define FILE_FLAG_EOF (1 << 0)
#define FILE_FLAG_ERROR (1 << 1)

/* 标准流 */
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#define stdin  stdin
#define stdout stdout
#define stderr stderr

/* fseek的第三个参数 */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
/* 默认buffer大小，setbuf函数设置这个量 */
#define BUFSIZ 4096
/* fget等函数返回的文件结束标识 */
#define EOF (-1)
/* setvbuf使用的宏 */
/* 当缓冲区为空时，从流读入数据。或者当缓冲区满时，向流写入数据 */
#define _IOFBF 0
/* 每次从流中读入一行数据或向流中写入一行数据 */
#define _IOLBF 1
/* 直接从流中读入数据或直接向流中写入数据，而没有缓冲区 */
#define _IONBF 2
/* 保证能够同时打开的文件数量 */
#define FOPEN_MAX 16
/* 保证支持的最长文件名的字符数 */
#define FILENAME_MAX 4096
/* 存放tmpnam函数生成的临时文件名的char数组的大小 */
#define L_tmpnam 20
/* 能生成的临时文件名个数 */
#define TMP_MAX 238328

/* begin - 直接输入输出 */
size_t fread(void *__restrict ptr, size_t size, size_t nmemb, FILE *__restrict stream);
size_t fwrite(const void *__restrict ptr, size_t size, size_t nmemb, FILE *__restrict stream);
/* end - 直接输入输出 */

/* begin - 文件操作 */
int remove(const char *filename);
int rename(const char *old_filename, const char *new_filename);
FILE *tmpfile(void);
char *tmpnam(char *s);
int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char *__restrict filename, const char *__restrict mode);
FILE *freopen(const char *__restrict filename, const char *__restrict mode, FILE *__restrict stream);
void setbuf(FILE *__restrict stream, char *__restrict buf);
int setvbuf(FILE *__restrict stream, char *__restrict buf, int mode, size_t size);
/* end - 文件操作 */

/* begin - 格式化I/O */
/* 下边五个函数格式化输出字节流到stdout/stream/file/buffer */
int printf(const char *__restrict format, ...);
int dprintf(int fd, const char *__restrict format, ...);
int fprintf(FILE *__restrict stream, const char *__restrict format, ...);
int sprintf(char *__restrict s, const char *__restrict format, ...);
int snprintf(char *__restrict s, size_t n, const char *__restrict format, ...);
/* 下边五个函数使用可变参数列表，格式化输出字节流到stdout/stream/file/buffer */
int vprintf(const char *__restrict format, __gnuc_va_list vl);
int vdprintf(int fd, const char *__restrict format, __gnuc_va_list vl);
int vfprintf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list vl);
int vsprintf(char *__restrict s, const char *__restrict format, __gnuc_va_list vl);
int vsnprintf(char *__restrict s, size_t n, const char *__restrict format, __gnuc_va_list vl);
/* 下边四个函数从stdout/file/buffer格式化读取字节流 */
int scanf(const char *__restrict format, ...);
int fscanf(FILE *__restrict stream, const char *__restrict format);
int sscanf(const char *__restrict s, const char *__restrict format, ...);
/* 下边四个函数使用可变参数列表从stdout/file/buffer格式化读取字节流 */
int vscanf(const char *__restrict format, __gnuc_va_list vl);
int vfscanf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list vl);
int vsscanf(const char *__restrict s, const char *__restrict format, __gnuc_va_list vl);

/* 将当前错误码写到stderr */
void perror(const char *s);
/* end - 格式化I/O */

/* begin - 非格式化I/O */
/* 从文件读取字节流 */
int fgetc(FILE *stream);
int getc(FILE *stream);
/* 从文件中读取一行字节流 */
char *fgets(char *__restrict s, int n, FILE *__restrict stream);
/* 写一个字节到流 */
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
/* 写一个字符串到流 */
int fputs(const char *__restrict s, FILE *__restrict stream);
/* stdin/stdout中读取/写入一个字符 */
int getchar(void);
int putchar(int c);
/* 写一个字符串到stdout */
int puts(const char *s);
/* 将读到的字符回退到输入流中 */
int ungetc(int c, FILE *stream);
/* end - 非格式化I/O */

/* begin - 文件内定位 */
/* 获取当前文件指针位置 */
int fgetpos(FILE *__restrict stream, fpos_t *__restrict pos);
/* 将文件指针移动到指定位置 */
int fseek(FILE *stream, long int offset, int whence);
/* 将文件指针移动到指定位置 */
int fsetpos(FILE *stream, const fpos_t *pos);
/* 返回文件当前位置指针偏移字节 */
long int ftell(FILE *stream);
/* 将位置指针移到文件开头 */
void rewind(FILE *stream);
/* end - 文件内定位 */

/* begin - 错误处理 */
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
/* end - 错误处理 */

#if __USE_INWOX || __USE_POSIX
FILE *fdopen(int, const char *);
int vcbprintf(void *, size_t (*)(void *, const char *, size_t), const char *, __gnuc_va_list);
#endif /* __USE_INWOX || __USE_POSIX */

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
