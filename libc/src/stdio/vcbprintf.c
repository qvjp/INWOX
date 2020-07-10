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

/* libc/src/stdio/vcbprintf.c
 * 格式化输出具体实现函数
 */

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * printf("%[parameter][flags][width][.precision][length]type", x);
 *
 * parameter n$，n是数字，表示第n个参数，这使得一个参数可以输出多次，
 *           如果任意一个占位符使用了parameter，那么其他占位符都需要使用
 * flags     控制符号显示、对齐、小数显示等
 * width     给出显示数值的最小宽度，实际输出字符的个数不足域宽，则根据左对齐或右对齐进行填充
 * precision 通常指明输出的最大长度，依赖于特定的格式化类型
 * length    字段表示整形参数或浮点型参数的长度
 */

/**
 * printf参数中"width"可取值情况如下
 */
enum {
    LENGTH_CHAR_HH,        /* hh 将相应参数按signed或unsigned char输出 */
    LENGTH_SHORT_H,        /* h  将相应参数按signed或unsigned short输出 */
    LENGTH_INT,            /*    默认情况 */
    LENGTH_LONG_L,         /* l  将相应参数按signed或unsigned long输出 */
    LENGTH_LONG_LONG_LL,   /* ll 将相应参数按signed或unsigned long long输出*/
    LENGTH_INTMAX_J,       /* j  intmax_t或uintmax_t */
    LENGTH_SIZE_Z,         /* z  size_t */
    LENGTH_PTRDIFF_T,      /* t  ptrdiff_t */
    LENGTH_LONG_DOUBLE_L   /* L  long double */
};

enum {
    FLAG_LEFT_JUSTIFIED = 1 << 1,      /* - 左对齐输出 */
    FLAG_PLUS = 1 << 2,                /* + 总是显示带符号数的正负号 */
    FLAG_SPACE = 1 << 3,               /* （空格）  如果第一个字符不是正负号， 则在前边加上空格 */
    FLAG_ALTERNATIVE = 1 << 4,         /* # 指定另一种转换形式（例如十六进制加0x前缀 */
    FLAG_LEADING_ZEROS = 1 << 5        /* 0 显示前导零 */
};

static size_t noop_callback(void* param, const char* s, size_t length){
    (void) param; (void) s;
    return length;
}

static size_t convert_integer(char* destination, uintmax_t value,
                              uintmax_t base, const char* digits)
{
    uintmax_t i = value;
    int length = 0;
    while (i > 0) {
        i /= base;
        length++;
    }
    destination += length;
    do {
        *--destination = digits[value % base];
        value /= base;
    } while (value);
    return length;
}

static int printInteger(void* param,
        size_t (*callback)(void*, const char*, size_t), char specifier,
        uintmax_t value, int fieldWidth, int precision, int flags) {
    bool negative = false;
    if (specifier == 'd' || specifier == 'i') {
        intmax_t signedValue = (intmax_t) value;
        if (signedValue < 0) {
            negative = true;
            value = (uintmax_t) -signedValue;
        }
    } else {
        flags &= ~(FLAG_PLUS | FLAG_SPACE);
    }

    char buffer[(sizeof(uintmax_t) * 3)];
    const char* digits = specifier == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
    unsigned int base = 10;

    if (specifier == 'o') {
        base = 8;
    } else if (specifier == 'x' || specifier == 'X') {
        base = 16;
    }

    int stringLength = convert_integer(buffer, value, base, digits);

    if (flags & FLAG_ALTERNATIVE && specifier == 'o' &&
            stringLength >= precision) {
        if (__builtin_add_overflow(stringLength, 1, &precision)) {
            errno = EOVERFLOW;
            return -1;
        }
    }

    int unpaddedLength = stringLength >= precision ? stringLength : precision;
    if (negative || flags & (FLAG_PLUS | FLAG_SPACE)) {
        if (__builtin_add_overflow(unpaddedLength, 1, &unpaddedLength)) {
            errno = EOVERFLOW;
            return -1;
        }
    }
    if (flags & FLAG_ALTERNATIVE && (specifier == 'x' || specifier == 'X') &&
            value != 0) {
        if (__builtin_add_overflow(unpaddedLength, 2, &unpaddedLength)) {
            errno = EOVERFLOW;
            return -1;
        }
    }

    if (!(flags & (FLAG_LEFT_JUSTIFIED | FLAG_LEADING_ZEROS))) {
        for (int i = unpaddedLength; i < fieldWidth; i++) {
            if (callback(param, " ", 1) != 1) return -1;
        }
    }

    if (negative || flags & (FLAG_PLUS | FLAG_SPACE)) {
        char sign = negative ? '-' : (flags & FLAG_PLUS) ? '+' : ' ';
        if (callback(param, &sign, 1) != 1) return -1;
    }

    if (flags & FLAG_ALTERNATIVE) {
        if (specifier == 'x' && value != 0) {
            if (callback(param, "0x", 2) != 2) return -1;
        } else if (specifier == 'X' && value != 0) {
            if (callback(param, "0X", 2) != 2) return -1;
        }
    }

    if (!(flags & FLAG_LEFT_JUSTIFIED) && flags & FLAG_LEADING_ZEROS) {
        for (int i = unpaddedLength; i < fieldWidth; i++) {
            if (callback(param, "0", 1) != 1) return -1;
        }
    }

    for (int i = stringLength; i < precision; i++) {
        if (callback(param, "0", 1) != 1) return -1;
    }

    if (callback(param, buffer, stringLength) != (size_t) stringLength) {
        return -1;
    }

    if (flags & FLAG_LEFT_JUSTIFIED) {
        for (int i = unpaddedLength; i < fieldWidth; i++) {
            if (callback(param, " ", 1) != 1) return -1;
        }
    }

    return unpaddedLength >= fieldWidth ? unpaddedLength : fieldWidth;
}

static int printString(void* param,
        size_t (*callback)(void*, const char*, size_t), const char* s,
        int length, int fieldWidth, int flags) {
    if (!(flags & FLAG_LEFT_JUSTIFIED)) {
        for (int i = length; i < fieldWidth; i++) {
             if (callback(param, " ", 1) != 1) return -1;
        }
    }
    if (callback(param, s, length) != (size_t) length) return -1;
    if (flags & FLAG_LEFT_JUSTIFIED) {
        for (int i = length; i < fieldWidth; i++) {
            if (callback(param, " ", 1) != 1) return -1;
        }
    }

    return length >= fieldWidth ? length : fieldWidth;
}

int vcbprintf(void* param, size_t (*callback)(void*, const char*, size_t),
        const char* format, va_list vl) {
    if (!callback) {
        callback = noop_callback;
    }

    bool invalidConversion = false;
    int result = 0;
#define INCREMENT_RESULT(x) \
        if (__builtin_add_overflow(result, (x), &result)) goto overflow

    while (*format) {
        if (*format != '%' || invalidConversion) {
            if (callback(param, format, 1) != 1) return -1;
            INCREMENT_RESULT(1);
        } else {
            const char* specifierBegin = format;

            // TODO: parameter还未支持
            /* 开始解析flags */
            int flags = 0;
            while (true) {
                switch (*++format) {
                case '-': flags |= FLAG_LEFT_JUSTIFIED; continue;
                case '+': flags |= FLAG_PLUS; continue;
                case ' ': flags |= FLAG_SPACE; continue;
                case '#': flags |= FLAG_ALTERNATIVE; continue;
                case '0': flags |= FLAG_LEADING_ZEROS; continue;
                }
                break;
            }

            /* 解析待显示数值的最小宽度 */
            int fieldWidth = 0;
            if (*format == '*') {
                fieldWidth = va_arg(vl, int);
                if (fieldWidth < 0) {
                    flags |= FLAG_LEFT_JUSTIFIED;
                    fieldWidth = -fieldWidth;
                }
                format++;
            } else {
                while (*format >= '0' && *format <= '9') {
                    fieldWidth = fieldWidth * 10 + *format - '0';
                    format++;
                }
            }

            /* 解析输出的最大长度 */
            int precision = -1;
            if (*format == '.') {
                format++;
                if (*format == '*') {
                    precision = va_arg(vl, int);
                    format++;
                } else {
                    precision = 0;
                    while (*format >= '0' && *format <= '9') {
                        precision = precision * 10 + *format - '0';
                        format++;
                    }
                }
            }

            /* 解析长度 */
            int length = LENGTH_INT;
            while (true) {
                switch (*format++) {
                case 'h':
                    if (length == LENGTH_SHORT_H) {
                        length = LENGTH_CHAR_HH;
                        break;
                    }
                    length = LENGTH_SHORT_H;
                    continue;
                case 'l':
                    if (length == LENGTH_LONG_L) {
                        length  = LENGTH_LONG_LONG_LL;
                        break;
                    }
                    length = LENGTH_LONG_L;
                    continue;
                case 'j':
                    length = LENGTH_INTMAX_J;
                    break;
                case 'z':
                    length = LENGTH_SIZE_Z;
                    break;
                case 't':
                    length = LENGTH_PTRDIFF_T;
                    break;
                case 'L':
                    length = LENGTH_LONG_DOUBLE_L;
                    break;
                default:
                    format--;
                }
                break;
            }

            /* 解析待打印类型 */
            char specifier = *format;
            uintmax_t value = 0;
            if (specifier == 'o' || specifier == 'u' || specifier == 'x' || specifier == 'X') {
                switch (length) {
                case LENGTH_CHAR_HH:
                    value = (unsigned char) va_arg(vl, unsigned int);
                    break;
                case LENGTH_SHORT_H:
                    value = (unsigned short) va_arg(vl, unsigned int);
                    break;
                case LENGTH_INT:
                    value = va_arg(vl, unsigned int);
                    break;
                case LENGTH_LONG_L:
                    value = va_arg(vl, unsigned long);
                    break;
                case LENGTH_LONG_LONG_LL:
                    value = va_arg(vl, unsigned long long);
                    break;
                case LENGTH_INTMAX_J:
                    value = va_arg(vl, uintmax_t);
                    break;
                case LENGTH_SIZE_Z:
                    value = va_arg(vl, size_t);
                    break;
                case LENGTH_PTRDIFF_T:
                    value = va_arg(vl, ptrdiff_t);
                    break;
                }
            } else if (specifier == 'd' || specifier == 'i') {
                intmax_t signedValue = 0;
                switch (length) {
                case LENGTH_CHAR_HH:
                    signedValue = (signed char) va_arg(vl, int);
                    break;
                case LENGTH_SHORT_H:
                    signedValue = (short) va_arg(vl, int);
                    break;
                case LENGTH_INT:
                    signedValue = va_arg(vl, int);
                    break;
                case LENGTH_LONG_L:
                    signedValue = va_arg(vl, long);
                    break;
                case LENGTH_LONG_LONG_LL:
                    signedValue = va_arg(vl, long long);
                    break;
                case LENGTH_INTMAX_J:
                    signedValue = va_arg(vl, intmax_t);
                    break;
                case LENGTH_SIZE_Z:
                    signedValue = va_arg(vl, ssize_t);
                    break;
                case LENGTH_PTRDIFF_T:
                    signedValue = va_arg(vl, ptrdiff_t);
                    break;
                }

                value = (uintmax_t) signedValue;
            }

            size_t size;
            char c;
            const char* s;
            int written;

            switch (specifier) {
            case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
                if (precision < 0) {
                    precision = 1;
                } else {
                    flags &= ~FLAG_LEADING_ZEROS;
                }

                written = printInteger(param, callback, specifier, value,
                        fieldWidth, precision, flags);
                if (written < 0) return -1;
                INCREMENT_RESULT(written);
                break;
            case 'c':
                c = (char) va_arg(vl, int);
                written = printString(param, callback, &c, 1, fieldWidth,
                        flags);
                if (written < 0) return -1;
                INCREMENT_RESULT(written);
                break;
            case 's':
                s = va_arg(vl, const char*);

                if (precision < 0) {
                    size = strlen(s);
                } else {
                    size = strnlen(s, (size_t) precision);
                }
                if (size > INT_MAX) goto overflow;

                written = printString(param, callback, s, (int) size,
                        fieldWidth, flags);
                if (written < 0) return -1;
                INCREMENT_RESULT(written);
                break;
            case 'p':
                value = (uintptr_t) va_arg(vl, void*);
                written = printInteger(param, callback, 'x', value, 0, 1,
                        FLAG_ALTERNATIVE);
                if (written < 0) return -1;
                INCREMENT_RESULT(written);
                break;
            case 'n':
                *va_arg(vl, int*) = result;
                break;
            case '%':
                if (callback(param, "%", 1) != 1) return -1;
                INCREMENT_RESULT(1);
                break;
            default:
                invalidConversion = true;
                format = specifierBegin;
                continue;
            }
        }
        format++;
    }

    return result;

overflow:
    errno = EOVERFLOW;
    return -1;
}
