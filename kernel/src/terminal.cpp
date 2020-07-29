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

/* kernel/src/terminal.cpp
 * Terminal class.
 */

#include <inwox/kernel/terminal.h>

Terminal terminal;
// 0xC0000000是视频内存首地址
static uint8_t *video = (uint8_t*) 0xC0000000;
static uint8_t cursorPostX = 0;
static uint8_t cursorPostY = 0;
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

static color_t FontColor = 0x83; /* 灰底青字 */

static void printChar(char c) {
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

void Terminal::onKeyboardEvent(int key) {
    char c = Keyboard::getCharFromKey(key);

    if (c) {
        write(&c, 1);
    }
}

ssize_t Terminal::write(const void* buffer, size_t size) {
    const char* buf = (const char*) buffer;

    for (size_t i = 0; i < size; i++) {
        printChar(buf[i]);
    }
    return (ssize_t) size;
}

/**
 * 初始化终端
 * 将屏幕背景设置为灰色
 */
void Terminal::initTerminal()
{
    terminal.setFontColor(0x83);/* 灰底青字 */
    for (size_t i = 0; i < 25; i++)
    {
        for (size_t j = 0; j < 80; j++)
        {
            printChar(' ');
        }
    }
    cursorPostX = 0;
    cursorPostY = 0;
}

/**
 * 将屏幕输出设为警告色
 */
void Terminal::warnTerminal()
{
    terminal.setFontColor(0xCF);
    for (size_t i = 0; i < 25; i++)
    {
        for (size_t j = 0; j < 80; j++)
        {
            video[i * 2 * 80 + 2 * j + 1] = FontColor;
        }
    }
}

void Terminal::setFontColor(color_t color)
{
    FontColor = color;
    return;
}

color_t Terminal::getFontColor()
{
    return FontColor;
}
