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
 * libc/src/stdlib/strtol.c
 * 将string转换成long
 */

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef STRTOL_NAME
#define STRTOL_NAME       strtol
#define STRTOL_RESULT     long
#define STRTOL_RESULT_MAX LONG_MAX
#define STRTOL_RESULT_MIN LONG_MIN
#define STRTOL_UNSIGNED   false
#endif

static STRTOL_RESULT getDigitValue(unsigned char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'z') {
        return 10 + c - 'a';
    } else if (c >= 'A' && c <= 'Z') {
        return 10 + c - 'A';
    } else {
        return STRTOL_RESULT_MAX;
    }
}

STRTOL_RESULT STRTOL_NAME(const char *restrict string, char **restrict end, int base)
{
    if (base < 0 || base == 1 || base > 36) {
        errno = EINVAL;
        return 0;
    }

    const char *str = string;

    /* Ignore any leading white space */
    while (isspace(*str)) {
        str++;
    }

    bool minus = false;

    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        minus = true;
        str++;
    }

    bool negative = minus && !STRTOL_UNSIGNED;

    /* Autodetect the base if necessary */
    if (base == 0) {
        if (*str == '0') {
            if (str[1] == 'x' || str[1] == 'X') {
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }

    if (base == 16 && *str == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    STRTOL_RESULT result = 0;
    bool overflow = false;
    bool numberFound = false;

    while (true) {
        STRTOL_RESULT digit = getDigitValue(*str);

        if (digit >= (STRTOL_RESULT)base) {
            break;
        }

        numberFound = true;

        if (__builtin_mul_overflow(result, base, &result)) {
            overflow = true;
        }

        if (!negative && STRTOL_RESULT_MAX - result >= digit) {
            result += digit;
        } else if (negative && result - STRTOL_RESULT_MIN >= digit) {
            result -= digit;
        } else {
            overflow = true;
        }

        str++;
    }

    if (overflow) {
        if (STRTOL_UNSIGNED || !negative) {
            result = STRTOL_RESULT_MAX;
        } else {
            result = STRTOL_RESULT_MIN;
        }
        errno = ERANGE;

    } else if (minus && STRTOL_UNSIGNED) {
        result = -result;
    }

    if (end) {
        *end = (char *)(numberFound ? str : string);
    }

    return result;
}
