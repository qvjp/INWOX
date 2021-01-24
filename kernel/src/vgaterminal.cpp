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

/* kernel/src/vgaterminal.cpp
 * vga terminal

 * 每个VGA buffer都是BBBBFFFFCCCCCCCC的结构
 * 如下图：
 *
 * |----------------|----------------|--------------------------------|
 * |background color|Foreground color|          Character             |
 * |     4 bit      |     4 bit      |             8 bit              |
 * |----------------|----------------|--------------------------------|
 * |15---------------------------------------------------------------0|

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

#include <string.h>
#include <inwox/kernel/inwox.h>
#include <inwox/kernel/vgaterminal.h>

#define HEIGHT 25
#define WIDTH 80
#define VIDEO_SIZE 2 * WIDTH * HEIGHT
#define MAX_PARAMS 16
#define DEFAULT_BACKGROUND_COLOR 0x8
#define DEFAULT_FOREGROUND_COLOR 0x3
#define DEFAULT_COLOR ((DEFAULT_BACKGROUND_COLOR << 4) | DEFAULT_FOREGROUND_COLOR)

// 0xC0000000是视频内存首地址
static char* const video = (char*) 0xC0000000;
static color_t fontColor = DEFAULT_COLOR;

static unsigned int cursorPosX = 0;
static unsigned int cursorPosY = 0;

static unsigned int savedX = 0;
static unsigned int savedY = 0;

static unsigned int params[MAX_PARAMS];
static bool paramSpecified[MAX_PARAMS];
static size_t paramIndex;

enum TerminalStatus{
    NORMAL,
    ESCAPED,
    CSI,    // CSI序列由ESC [、若干个（包括0个）“参数字节”、若干个“中间字节”，以及一个“最终字节”组成
};
static TerminalStatus status = NORMAL;

static inline char* videoOffset(unsigned int line, unsigned int column)
{
    return video + 2 * line * WIDTH + 2 * column;
}

void VgaTerminal::backspace() {
    if (cursorPosX == 0 && cursorPosY > 0) {
        cursorPosX = WIDTH - 1;
        cursorPosY--;
    } else {
        cursorPosX--;
    }
    *videoOffset(cursorPosY, cursorPosX) = 0;
    *(videoOffset(cursorPosY, cursorPosX) + 1) = fontColor;
}

/**
 * @brief 处理SGR参数（未完整支持）
 * 代码     作用                   是否实现
 * 0       重置/正常               1
 * 1       粗体或增加强度           1
 * 2       弱化（降低强度）
 * 3       斜体
 * 4       下划线
 * 5       缓慢闪烁
 * 6       快速闪烁
 * 7       反显
 * 8       隐藏
 * 9       划除
 * 10      主要（默认）字体
 * 11–19   替代字体
 * 20      尖角体
 * 21      关闭粗体或双下划线
 * 22      正常颜色或强度           1
 * 23      非斜体、非尖角体
 * 24      关闭下划线
 * 25      关闭闪烁
 * 27      关闭反显
 * 28      关闭隐藏
 * 29      关闭划除
 * 30–37   设置前景色               1
 * 38      设置前景色，下一个参数是5;n或2;r;g;b
 * 39      默认前景色               1
 * 40–47   设置背景色               1
 * 48      设置背景色，下一个参数是5;n或2;r;g;b
 * 49      默认背景色               1
 * 51      Framed
 * 52      Encircled
 * 53      上划线
 * 54      Not framed or encircled
 * 55      关闭上划线
 * 60      表意文字下划线或右边线
 * 61      表意文字双下划线或双右边线
 * 62      表意文字上划线或左边线
 * 63      表意文字双上划线或双左边线
 * 64      表意文字着重标志
 * 65      表意文字属性关闭
 * 90–97   设置明亮的前景色
 * 100–107 设置明亮的背景色
 */
static void setGraphicsRendition()
{
    for (size_t i = 0; i < MAX_PARAMS; i++)
    {
        if (!paramSpecified[i])
        {
            continue;
        }
        switch (params[i])
        {
        case 0: // 关闭所有属性
            fontColor = DEFAULT_COLOR;
            break;
        case 1: // 粗体或增加亮度
            fontColor |= 0x08;
            break;
        case 22: // 正常粗细和亮度
            fontColor &= ~0x08;
            break;
        // 前景色
        case 30: // 黑
            fontColor = (fontColor & 0xF8) | 0x00;
            break;
        case 31: // 红
            fontColor = (fontColor & 0xF8) | 0x04;
            break;
        case 32: // 绿
            fontColor = (fontColor & 0xF8) | 0x02;
            break;
        case 33: // 黄
            fontColor = (fontColor & 0xF8) | 0x06;
            break;
        case 34: // 蓝
            fontColor = (fontColor & 0xF8) | 0x01;
            break;
        case 35: // 品红
            fontColor = (fontColor & 0xF8) | 0x05;
            break;
        case 36: // 青
            fontColor = (fontColor & 0xF8) | 0x03;
            break;
        case 37: // 白
            fontColor = (fontColor & 0xF8) | 0x07;
            break;
        case 39: // 恢复默认前景色
            fontColor = (fontColor & 0xF8) | DEFAULT_FOREGROUND_COLOR;
            break;
        // 背景色
        case 40: // 黑
            fontColor = (fontColor & 0x0F) | 0x00;
            break;
        case 41: // 红
            fontColor = (fontColor & 0x0F) | 0x40;
            break;
        case 42: // 绿
            fontColor = (fontColor & 0x0F) | 0x20;
            break;
        case 43: // 黄
            fontColor = (fontColor & 0x0F) | 0x60;
            break;
        case 44: // 蓝
            fontColor = (fontColor & 0x0F) | 0x10;
            break;
        case 45: // 品红
            fontColor = (fontColor & 0x0F) | 0x50;
            break;
        case 46: // 青
            fontColor = (fontColor & 0x0F) | 0x30;
            break;
        case 47: // 白
            fontColor = (fontColor & 0x0F) | 0x70;
            break;
        case 49: // 恢复默认背景色
            fontColor = (fontColor & 0x0F) | DEFAULT_COLOR;
            break;

        default:
            // 忽略未实现参数
            break;
        }
    }
}

/**
 * @brief 打印ANSI转义序列（未完整支持）
 * 代码     |  名称                                         作用
 * CSI n A | CUU – 光标上移（Cursor Up）
 * CSI n B | CUD – 光标下移（Cursor Down）                   光标向指定的方向移动 n（默认1）格 如果光标已在屏幕边缘，则无效
 * CSI n C | CUF – 光标前移（Cursor Forward）
 * CSI n D | CUB – 光标后移（Cursor Back）
 * CSI n E | CNL – 光标移到下一行（Cursor Next Line）          光标移动到下面第 n（默认1）行的开头
 * CSI n F | CPL – 光标移到上一行（Cursor Previous Line）      光标移动到上面第 n（默认1）行的开头
 * CSI n G | CHA – 光标水平绝对（Cursor Horizontal Absolute）  光标移动到第 n（默认1）列
 * CSI n ; m H |  CUP – 光标位置（Cursor Position）           光标移动到第 n行、第m列 值从1开始，且默认为1（左上角）
 * CSI n J | ED – 擦除显示（Erase in Display）                清除屏幕的部分区域
 *                                                          如果 n是0（或缺失），则清除从光标位置到屏幕末尾的部分
 *                                                          如果 n是1，则清除从光标位置到屏幕开头的部分
 *                                                          如果 n是2，则清除整个屏幕
 *                                                          如果 n是3，则清除整个屏幕，并删除回滚缓存区中的所有行
 * CSI n K | EL – 擦除行（Erase in Line）                     清除行内的部分区域
 *                                                          如果 n是0（或缺失），清除从光标位置到该行末尾的部分
 *                                                          如果 n是1，清除从光标位置到该行开头的部分
 *                                                          如果 n是2，清除整行 光标位置不变
 * CSI n S | SU – 向上滚动（Scroll Up）                       整页向上滚动 n（默认1）行 新行添加到底部
 * CSI n T | SD – 向下滚动（Scroll Down）                     整页向下滚动 n（默认1）行 新行添加到顶部
 * CSI n ; m f |  HVP – 水平垂直位置（Horizontal Vertical Position）  同CUP
 * CSI n m | SGR – 选择图形再现（Select Graphic Rendition）    设置SGR参数，包括文字颜色 CSI后可以是0或者更多参数，用分号分隔 如果没有参数，则视为CSI 0 m
 * CSI s   | SCP – 保存光标位置（Save Cursor Position）        保存光标的当前位置
 * CSI u   | RCP – 恢复光标位置（Restore Cursor Position）     恢复保存的光标位置
 * @param c 
 */
void VgaTerminal::printEscapeCode(char c)
{
    if (status == NORMAL) {
        status = ESCAPED;
    } else if (status == ESCAPED) {
        if (c == '[') { // CSI - Control Sequence Introducer
            status = CSI;
            for (size_t i = 0; i < MAX_PARAMS; i++) {
                params[i] = 0;
                paramSpecified[i] = false;
            }
            paramIndex = 0;
        } else if (c == 'c') { // RIS - Reset to Initial State 设置到初始状态
            memset(video, 0, VIDEO_SIZE);
            cursorPosX = cursorPosY = 0;
            savedX = savedY = 0;
            fontColor = DEFAULT_COLOR;
            status = NORMAL;
        } else {
            // 其他转义字符忽略
            status = NORMAL;
        }
    } else if (status == CSI) {
        if (c >= '0' && c <= '9') {  // 参数字节
            params[paramIndex] = params[paramIndex] * 10 + c - '0';
            paramSpecified[paramIndex] = true;
        } else if (c == ';') {       // 分隔符
            paramIndex++;
            if (paramIndex >= MAX_PARAMS) {
                status = NORMAL;
            }
        } else {    // 最终字节
            switch (c) {
            case 'A': { // CUU - 光标上移（Cursor Up)
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosY < param) {
                    cursorPosY = 0;
                } else {
                    cursorPosY -= param;
                }
            } break;
            case 'B': { // CUD - 光标下移（Cursor Down）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosY + param >= HEIGHT) {
                    cursorPosY = HEIGHT - 1;
                } else {
                    cursorPosY += param;
                }
            } break;
            case 'C': { // CUF - 光标前移（Cursor Forward）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosX + param >= WIDTH) {
                    cursorPosX = 0;
                } else {
                    cursorPosX += param;
                }
            } break;
            case 'D': { // CUB - 光标后移（Cursor Back）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosX < param) {
                    cursorPosX = 0;
                } else {
                    cursorPosX -= param;
                }
            } break;
            case 'E': { // CNL - 光标移到下一行（Cursor Next Line）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosY + param >= HEIGHT) {
                    cursorPosY = HEIGHT - 1;
                } else {
                    cursorPosY += param;
                }
                cursorPosX = 0;
            } break;
            case 'F': { // CPL - 光标移到上一行（Cursor Previous Line）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (cursorPosY < param) {
                    cursorPosY = 0;
                } else {
                    cursorPosY -= param;
                }
                cursorPosX = 0;
            } break;
            case 'G': { // CHA - 光标移动到指定列（Cursor Horizontal Absolute）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (0 < param && param < WIDTH) {
                    cursorPosX = param - 1;
                }
            } break;
            case 'H':   // CUP – 光标位置（Cursor Position）
            case 'f': { // HVP – 水平垂直位置（Horizontal Vertical Position）
                unsigned int x = paramSpecified[1] ? params[1] : 1;
                unsigned int y = paramSpecified[0] ? params[0] : 1;
                if (0 < x && x < WIDTH && 0 < y && y < HEIGHT) {
                    cursorPosX = x - 1;
                    cursorPosY = y - 1;
                }
            } break;
            case 'J': { // ED – 擦除显示（Erase in Display）
                unsigned int param = paramSpecified[0] ? params[0] : 0;
                if (param == 0) {
                    char* from = videoOffset(cursorPosY, cursorPosX);
                    memset(from, 0, VIDEO_SIZE - (from - video));
                } else if (param == 1) {
                    char* to = videoOffset(cursorPosY, cursorPosX);
                    memset(video, 0, to - video);
                } else if (param == 2) {
                    memset(video, 0, VIDEO_SIZE);
                }
            } break;
            case 'K': { // EL - 擦除行（Erase in Line）
                unsigned int param = paramSpecified[0] ? params[0] : 0;
                if (param == 0) {
                    fillRect(cursorPosX, WIDTH, cursorPosY, cursorPosY, '\0', fontColor);
                } else if (param == 1) {
                    fillRect(0, cursorPosX, cursorPosY, cursorPosY, '\0', fontColor);
                } else if (param == 2) {
                    fillRect(0, WIDTH, cursorPosY, cursorPosY, '\0', fontColor);
                }
            } break;
            case 'S': { // SU – 向上滚动（Scroll Up）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                memmove(video, videoOffset(param, 0), 2 * (HEIGHT - param) * WIDTH);
                fillRect(0, WIDTH, HEIGHT - param, HEIGHT, '\0', fontColor);
            } break;
            case 'T': { // SD - 向下滚动（Scroll Down）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                memmove(videoOffset(param, 0), video, 2 * (HEIGHT - param) * WIDTH);
                fillRect(0, WIDTH, 0, param, '\0', fontColor);
            } break;
            case 'd': { // VPA - 光标移动到指定行（Vertical Line Position Absolute）
                unsigned int param = paramSpecified[0] ? params[0] : 1;
                if (0 < param && param < HEIGHT) {
                    cursorPosY = param - 1;
                }
            } break;
            case 'm': { // SGR - 选择图形再现（Select Graphic Rendition）
                setGraphicsRendition();
            } break;
            case 's': { // SCP - 保存光标位置（Save Cursor Position）
                savedX = cursorPosX;
                savedY = cursorPosY;
            } break;
            case 'u': { // RCP - 恢复光标位置（Restore Cursor Position）  
                cursorPosX = savedX;
                cursorPosY = savedY;
            } break;
            default:
                // 忽略其他参数
                break;
            }
            status = NORMAL;
        }
    }
}

void VgaTerminal::printCharacterRaw(char c)
{
    if (c == '\n' || cursorPosX > WIDTH - 1) {
        cursorPosX = 0;
        cursorPosY++;

        if (cursorPosY > HEIGHT - 1) {
            // 整行上移
            memmove(video, videoOffset(1, 0), 2 * (HEIGHT - 1) * WIDTH);
            /* 最后一行别忘记设置颜色 */
            fillRect(0, WIDTH, HEIGHT-1, HEIGHT-1, '\0', fontColor);
            cursorPosY = HEIGHT - 1;
        }

        if (c == '\n') {
            return;
        }
    }

    *videoOffset(cursorPosY, cursorPosX) = c;
    *(videoOffset(cursorPosY, cursorPosX) + 1) = fontColor;

    cursorPosX++;
}

void VgaTerminal::printCharacter(char c)
{
    if (likely(status == NORMAL && c != '\e')) { // \e==\033==\x1B==27
        printCharacterRaw(c);
        return;
    }
    printEscapeCode(c);
}

/**
 * @brief 将x1/x2/y1/y2指定的矩形区域用ch/color刷新
 * 
 * @param x1 矩形区域x轴开始
 * @param x2 矩形区域x轴结束
 * @param y1 矩形区域y轴开始
 * @param y2 矩形区域y轴结束
 * @param ch 要填充的字符
 * @param color 要填充的颜色
 */
void VgaTerminal::fillRect(size_t x1, size_t x2, size_t y1, size_t y2, char ch, color_t color)
{
    if (x2 > WIDTH || y2 > HEIGHT) {
        return;
    }
    // 至少处理一行/一列
    if (x1 == x2) {
        x2++;
    }
    if (y1 == y2) {
        y2++;
    }
    for (size_t i = y1; i < y2; i++) {
        for (size_t j = x1; j < x2; j++) {
            *videoOffset(i, j) = ch;
            *(videoOffset(i, j) + 1) = color;
        }
    }
}

void VgaTerminal::init()
{
    fillRect(0, WIDTH, 0, HEIGHT, '\0', DEFAULT_COLOR);
    cursorPosX = 0;
    cursorPosY = 0;
}

void VgaTerminal::warnTerminal()
{
    fontColor = 0xCF;
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            *(videoOffset(i, j) + 1) = fontColor;
        }
    }
}
