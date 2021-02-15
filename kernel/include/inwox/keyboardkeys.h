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

/* kernel/include/inwox/keyboradkeys.h
 * 键盘键码
 */

#ifndef INWOX_KEYBOARDKEYS_H_
#define INWOX_KEYBOARDKEYS_H_

/**
 * @brief 以下为PS/2扫描码集1定义, S1表示SET1
 */
#define S1_KB_ESC 0x01
#define S1_KB_1 0x02
#define S1_KB_2 0x03
#define S1_KB_3 0x04
#define S1_KB_4 0x05
#define S1_KB_5 0x06
#define S1_KB_6 0x07
#define S1_KB_7 0x08
#define S1_KB_8 0x09
#define S1_KB_9 0x0A
#define S1_KB_0 0x0B
#define S1_KB_SYMBOL1 0x0C /* - */
#define S1_KB_SYMBOL2 0x0D /* = */
#define S1_KB_BACKSPACE 0x0E
#define S1_KB_TAB 0x0F
#define S1_KB_Q 0x10
#define S1_KB_W 0x11
#define S1_KB_E 0x12
#define S1_KB_R 0x13
#define S1_KB_T 0x14
#define S1_KB_Y 0x15
#define S1_KB_U 0x16
#define S1_KB_I 0x17
#define S1_KB_O 0x18
#define S1_KB_P 0x19
#define S1_KB_SYMBOL3 0x1A /* [ */
#define S1_KB_SYMBOL4 0x1B /* ] */
#define S1_KB_ENTER 0x1C
#define S1_KB_LCONTROL 0x1D
#define S1_KB_A 0x1E
#define S1_KB_S 0x1F
#define S1_KB_D 0x20
#define S1_KB_F 0x21
#define S1_KB_G 0x22
#define S1_KB_H 0x23
#define S1_KB_J 0x24
#define S1_KB_K 0x25
#define S1_KB_L 0x26
#define S1_KB_SYMBOL5 0x27 /* ; */
#define S1_KB_SYMBOL6 0x28 /* ' */
#define S1_KB_SYMBOL7 0x29 /* ` */
#define S1_KB_LSHIFT 0x2A
#define S1_KB_SYMBOL8 0x2B /* \ */
#define S1_KB_Z 0x2C
#define S1_KB_X 0x2D
#define S1_KB_C 0x2E
#define S1_KB_V 0x2F
#define S1_KB_B 0x30
#define S1_KB_N 0x31
#define S1_KB_M 0x32
#define S1_KB_SYMBOL9 0x33 /* , */
#define S1_KB_SYMBOL10 0x34 /* . */
#define S1_KB_DEV 0x35
#define S1_KB_RSHIFT 0x36
#define S1_KB_NUMPAD_MULT 0x37
#define S1_KB_LALT 0x38
#define S1_KB_SPACE 0x39
#define S1_KB_CAPSLOCK 0x3A
#define S1_KB_F1 0x3B
#define S1_KB_F2 0x3C
#define S1_KB_F3 0x3D
#define S1_KB_F4 0x3E
#define S1_KB_F5 0x3F
#define S1_KB_F6 0x40
#define S1_KB_F7 0x41
#define S1_KB_F8 0x42
#define S1_KB_F9 0x43
#define S1_KB_F10 0x44
#define S1_KB_NUMLOCK 0x45
#define S1_KB_SCROLLLOCK 0x46
#define S1_KB_NUMPAD7 0x47
#define S1_KB_NUMPAD8 0x48
#define S1_KB_NUMPAD9 0x49
#define S1_KB_NUMPAD_MINUS 0x4A
#define S1_KB_NUMPAD4 0x4B
#define S1_KB_NUMPAD5 0x4C
#define S1_KB_NUMPAD6 0x4D
#define S1_KB_NUMPAD_PLUS 0x4E
#define S1_KB_NUMPAD1 0x4F
#define S1_KB_NUMPAD2 0x50
#define S1_KB_NUMPAD3 0x51
#define S1_KB_NUMPAD0 0x52
#define S1_KB_NUMPAD_DOT 0x53

#define S1_KB_SYMBOL11 0x56
#define S1_KB_F11 0x57
#define S1_KB_F12 0x58

#define S1_KB_NUMPAD_ENTER 0x9C
#define S1_KB_RCONTROL 0x9D
#define S1_KB_NUMPAD_DIV 0xB5
#define S1_KB_ALTGR 0xB8
#define S1_KB_HOME 0xC7
#define S1_KB_UP 0xC8
#define S1_KB_PAGEUP 0xC9
#define S1_KB_LEFT 0xCB
#define S1_KB_RIGHT 0xCD
#define S1_KB_END 0xCF
#define S1_KB_DOWN 0xD0
#define S1_KB_PAGEDOWN 0xD1
#define S1_KB_INSERT 0xD2
#define S1_KB_DELETE 0xD3


#endif /* INWOX_KEYBOARDKEYS_H_ */
