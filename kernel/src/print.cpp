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
 * kernel/src/print.cpp
 * 格式化输出至屏幕
 */

#include <stdarg.h> /* va_* */
#include <stddef.h> /* size_t */
#include <stdint.h> /* uint8_t uint32_t */
#include <inwox/kernel/print.h>

// 0xC0000000是视频内存首地址
static uint8_t *video = (uint8_t*) 0xC0000000;
static uint8_t cursorPostX = 0;
static uint8_t cursorPostY = 0;
static uint8_t FontColor = 0x83; /* 灰底青字 */

static void printChar(char c);
static void printString(const char* s);
static void printUNum(unsigned u, int base);
static void printSNum(int u, int base);

void Print::printf(const char* format, ...)
{
    /*
     * 在这里写一下C语言中可变参数的基本用法
     * 
     * 其中va是指variable-argument(可变参数)
     * va_list是一个arg_ptr,指向参数的指针
     * va_start()初始化va_list对象
     * va_arg()返回参数指针当前指向的值
     * va_end()和va_start()对应，调用过va_start后就必须调用va_end
     */
    va_list ap;
    va_start(ap, format);

    int32_t d;
    uint32_t u;
    uint8_t c;
    const char* s;
    while (*format)
    {
        if(*format != '%')
        {
            printChar(*format);
        }
        else
        {
            switch(*++format)
            {
                case 'd':
                        d = va_arg(ap, int);
                        printSNum(d, 10);
                        break;
                case 'u':
                        u = va_arg(ap, unsigned);
                        printUNum(u, 10);
                        break;
                case 'x':
                        u = va_arg(ap, unsigned);
                        printUNum(u, 16);
                        break;
                case 'c':
                        c = (unsigned char) va_arg(ap, int);
                        printChar(c);
                        break;
                case 's':
                        s = va_arg(ap, char*);
                        printString(s);
                        break;
                case '%':
                        printChar('%');
                        break;
                case '\0':
                        va_end(ap);
                        return;
                default:
                        printChar('%');
                        printChar(*format);
            }
        }
        format++;
    }
    va_end(ap);
}

/**
 * 将屏幕输出设为警告色
 */
void Print::warnTerminal()
{
    FontColor = 0xCF; /* 红底白字 */
    for (size_t i = 0; i < 25; i++)
    for (size_t j = 0; j < 80; j++)
    {
        video[2 * (80 * i + j)] = ' ';
        video[2 * (80 * i + j) + 1] = FontColor;
    }
}

/**
 * 初始化终端
 * 将屏幕背景设置为灰色
 */
void Print::initTerminal()
{
    FontColor = 0x83; /* 灰底青字 */
    for (size_t i = 0; i < 25; i++)
        for (size_t j = 0; j < 80; j++)
        {
            video[2 * (80 * i + j)] = ' ';
            video[2 * (80 * i + j) + 1] = FontColor;
        }
}

/*
 * 每个VGA buffer都是BBBBFFFFCCCCCCCC的结构
 * 如下图：
 *
 * |----------------|----------------|--------------------------------|
 * |background color|Foreground color|          Character             |
 * |     4 bit      |     4 bit      |             8 bit              |
 * |----------------|----------------|--------------------------------|
 * |15---------------------------------------------------------------0|
 */

/**
 * 文本模式颜色常量
 * |+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|
 * |    BLACK    |    BLUE     |    CREEN    |    CYAN     |     RED     |   MAGENTA   |
 * |-------------|-------------|-------------|-------------|-------------|-------------|
 * |     0       |      1      |      2      |      3      |      4      |      5      |
 * |+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|
 * |    BROWN    |  LIGHT_GREY |  DARK_GREY  |  LIGHT_BLUE | LIGHT_GREEN |  LIGHT_CYAN |
 * |-------------|-------------|-------------|-------------|-------------|-------------|
 * |     6       |      7      |      8      |      9      |      A      |      B      |
 * |+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|
 * |  LIGHT_RED  |LIGHT_MAGENTA| LIGHT_BROWN |    WHITE    |
 * |-------------|-------------|-------------|-------------|
 * |     C       |      D      |      E      |      F      |
 * |+++++++++++++|+++++++++++++|+++++++++++++|+++++++++++++|
 */

/* 打印单个字符 */
static void printChar(char c)
{
    /* 遇到换行符或超出最大列数(79)就换行 */
    if (c == '\n' || cursorPostX > 79)
    {
        cursorPostX = 0;
        cursorPostY++;

        /* 如果当前行是最后一行，则整体向上移动一行 */
        if(cursorPostY > 24)
        {
            for(size_t i = 0; i < 2 * 24 * 80; i++)
            {
                video[i] = video[i +2 * 80];
            }
            /* 最后一行全部置成灰色 */
            for (size_t i = 2 * 24 * 80; i < 2 * 25 * 80; i++)
            {
                video[i] = (FontColor & 0xF0) | (FontColor >> 4);
            }
            cursorPostY = 24;
        }
        if (c == '\n')
                return;
    }
    video[cursorPostY * 2 * 80 + 2 * cursorPostX] = c;
    video[cursorPostY * 2 * 80 + 2 * cursorPostX + 1] = FontColor;

    cursorPostX++;
}

/* 打印字符串 */
static void printString(const char *s)
{
    while (*s)
    {
        printChar(*s++);
    }
}

/* 无符号正数 */
static void printUNum(unsigned u, int base)
{
    static const char* digits = "0123456789abcdef";

    /* 因为可能要输出32位的二进制数，所以这里要定义33位，最后一位存放\0 */
    char buffer[33];
    /* p指向buffer最后一个位置 */
    char* p = buffer + 32;
    *p = '\0';

    do {
        *--p = digits[u % base];
        u /= base;
    } while (u);

    printString(p);
}


/* 有符号正数 */
static void printSNum(int u, int base)
{
    int flag = 0; /* 正 */
    if (u < 0)
    {
        u = -u;
        flag = 1;
    }
    static const char* digits = "0123456789abcdef";

    char buffer[12];
    char* p = buffer + 11;
    *p = '\0';

    do {
        *--p = digits[u % base];
        u /= base;
    } while (u);

    if (!flag)
        printString(p);
    else
    {
        *--p = '-';
        printString(p);        
    }
}