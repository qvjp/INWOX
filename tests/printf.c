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
 * test/printf.c
 * 测试-sprintf()
 */

#include <stdio.h>
#include <string.h>

static int test_count = 0;
static int test_failed = 0;

#define TEST_SPRINTF(buf, ret, fmt, ...)                                                                   \
    do {                                                                                                   \
        test_count++;                                                                                      \
        char lbuf[64] = {0};                                                                               \
        int len = sprintf(lbuf, fmt, ##__VA_ARGS__);                                                       \
        if ((len != ret) || (strcmp(lbuf, buf))) {                                                         \
            test_failed++;                                                                                 \
            printf("* FAIL: line %d, expect: '%s'(%d) actual: '%s'(%d)\n", __LINE__, buf, ret, lbuf, len); \
        }                                                                                                  \
    } while (0);

static void test_print_base()
{
    TEST_SPRINTF("Hello INWOX", 11, "Hello INWOX")
    TEST_SPRINTF("\0", 1, "%c", '\0')
}

static void test_print_type()
{
    TEST_SPRINTF("Hello INWOX", 11, "%s", "Hello INWOX")
    TEST_SPRINTF("1024", 4, "%d", 1024)
    TEST_SPRINTF("-1024", 5, "%d", -1024)
    TEST_SPRINTF("1024", 4, "%i", 1024)
    TEST_SPRINTF("-1024", 5, "%i", -1024)
    TEST_SPRINTF("1024", 4, "%u", 1024u)
    TEST_SPRINTF("4294966272", 10, "%u", -1024u)
    TEST_SPRINTF("777", 3, "%o", 0777u)
    TEST_SPRINTF("37777777001", 11, "%o", -0777u)
    TEST_SPRINTF("1234abcd", 8, "%x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433", 8, "%x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD", 8, "%X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433", 8, "%X", -0x1234abcdu)
    TEST_SPRINTF("x", 1, "%c", 'x')
    TEST_SPRINTF("%", 1, "%%")
}

static void test_print_plus()
{
    TEST_SPRINTF("Hello INWOX", 11, "%+s", "Hello INWOX")
    TEST_SPRINTF("+1024", 5, "%+d", 1024)
    TEST_SPRINTF("-1024", 5, "%+d", -1024)
    TEST_SPRINTF("+1024", 5, "%+i", 1024)
    TEST_SPRINTF("-1024", 5, "%+i", -1024)
    TEST_SPRINTF("1024", 4, "%+u", 1024u)
    TEST_SPRINTF("4294966272", 10, "%+u", -1024u)
    TEST_SPRINTF("777", 3, "%+o", 0777u)
    TEST_SPRINTF("37777777001", 11, "%+o", -0777u)
    TEST_SPRINTF("1234abcd", 8, "%+x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433", 8, "%+x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD", 8, "%+X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433", 8, "%+X", -0x1234abcdu)
    TEST_SPRINTF("x", 1, "%+c", 'x')
}

static void test_print_space()
{
    TEST_SPRINTF("Hello INWOX", 11, "% s", "Hello INWOX")
    TEST_SPRINTF(" 1024", 5, "% d", 1024)
    TEST_SPRINTF("-1024", 5, "% d", -1024)
    TEST_SPRINTF(" 1024", 5, "% i", 1024)
    TEST_SPRINTF("-1024", 5, "% i", -1024)
    TEST_SPRINTF("1024", 4, "% u", 1024u)
    TEST_SPRINTF("4294966272", 10, "% u", -1024u)
    TEST_SPRINTF("777", 3, "% o", 0777u)
    TEST_SPRINTF("37777777001", 11, "% o", -0777u)
    TEST_SPRINTF("1234abcd", 8, "% x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433", 8, "% x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD", 8, "% X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433", 8, "% X", -0x1234abcdu)
    TEST_SPRINTF("x", 1, "% c", 'x')
}

static void test_print_plus_space()
{
    TEST_SPRINTF("Hello INWOX", 11, "%+ s", "Hello INWOX")
    TEST_SPRINTF("+1024", 5, "%+ d", 1024)
    TEST_SPRINTF("-1024", 5, "%+ d", -1024)
    TEST_SPRINTF("+1024", 5, "%+ i", 1024)
    TEST_SPRINTF("-1024", 5, "%+ i", -1024)
    TEST_SPRINTF("1024", 4, "%+ u", 1024u)
    TEST_SPRINTF("4294966272", 10, "%+ u", -1024u)
    TEST_SPRINTF("777", 3, "%+ o", 0777u)
    TEST_SPRINTF("37777777001", 11, "%+ o", -0777u)
    TEST_SPRINTF("1234abcd", 8, "%+ x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433", 8, "%+ x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD", 8, "%+ X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433", 8, "%+ X", -0x1234abcdu)
    TEST_SPRINTF("x", 1, "%+ c", 'x')
}

static void test_print_sharp()
{
    TEST_SPRINTF("0777", 4, "%#o", 0777u)
    TEST_SPRINTF("037777777001", 12, "%#o", -0777u)
    TEST_SPRINTF("0x1234abcd", 10, "%#x", 0x1234abcdu)
    TEST_SPRINTF("0xedcb5433", 10, "%#x", -0x1234abcdu)
    TEST_SPRINTF("0X1234ABCD", 10, "%#X", 0x1234abcdu)
    TEST_SPRINTF("0XEDCB5433", 10, "%#X", -0x1234abcdu)
    TEST_SPRINTF("0", 1, "%#o", 0u)
    TEST_SPRINTF("0", 1, "%#x", 0u)
    TEST_SPRINTF("0", 1, "%#X", 0u)
}

static void test_print_width()
{
    TEST_SPRINTF("Hello INWOX", 11, "%1s", "Hello INWOX")
    TEST_SPRINTF("1024", 4, "%1d", 1024)
    TEST_SPRINTF("-1024", 5, "%1d", -1024)
    TEST_SPRINTF("1024", 4, "%1i", 1024)
    TEST_SPRINTF("-1024", 5, "%1i", -1024)
    TEST_SPRINTF("1024", 4, "%1u", 1024u)
    TEST_SPRINTF("4294966272", 10, "%1u", -1024u)
    TEST_SPRINTF("777", 3, "%1o", 0777u)
    TEST_SPRINTF("37777777001", 11, "%1o", -0777u)
    TEST_SPRINTF("1234abcd", 8, "%1x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433", 8, "%1x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD", 8, "%1X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433", 8, "%1X", -0x1234abcdu)
    TEST_SPRINTF("x", 1, "%1c", 'x')
}

static void test_print_width_2()
{
    TEST_SPRINTF("               Hello", 20, "%20s", "Hello")
    TEST_SPRINTF("                1024", 20, "%20d", 1024)
    TEST_SPRINTF("               -1024", 20, "%20d", -1024)
    TEST_SPRINTF("                1024", 20, "%20i", 1024)
    TEST_SPRINTF("               -1024", 20, "%20i", -1024)
    TEST_SPRINTF("                1024", 20, "%20u", 1024u)
    TEST_SPRINTF("          4294966272", 20, "%20u", -1024u)
    TEST_SPRINTF("                 777", 20, "%20o", 0777u)
    TEST_SPRINTF("         37777777001", 20, "%20o", -0777u)
    TEST_SPRINTF("            1234abcd", 20, "%20x", 0x1234abcdu)
    TEST_SPRINTF("            edcb5433", 20, "%20x", -0x1234abcdu)
    TEST_SPRINTF("            1234ABCD", 20, "%20X", 0x1234abcdu)
    TEST_SPRINTF("            EDCB5433", 20, "%20X", -0x1234abcdu)
    TEST_SPRINTF("                   x", 20, "%20c", 'x')
}

static void test_print_left_width()
{
    TEST_SPRINTF("Hello               ", 20, "%-20s", "Hello")
    TEST_SPRINTF("1024                ", 20, "%-20d", 1024)
    TEST_SPRINTF("-1024               ", 20, "%-20d", -1024)
    TEST_SPRINTF("1024                ", 20, "%-20i", 1024)
    TEST_SPRINTF("-1024               ", 20, "%-20i", -1024)
    TEST_SPRINTF("1024                ", 20, "%-20u", 1024u)
    TEST_SPRINTF("4294966272          ", 20, "%-20u", -1024u)
    TEST_SPRINTF("777                 ", 20, "%-20o", 0777u)
    TEST_SPRINTF("37777777001         ", 20, "%-20o", -0777u)
    TEST_SPRINTF("1234abcd            ", 20, "%-20x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433            ", 20, "%-20x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD            ", 20, "%-20X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433            ", 20, "%-20X", -0x1234abcdu)
    TEST_SPRINTF("x                   ", 20, "%-20c", 'x')
}

static void test_print_zero_width()
{
    TEST_SPRINTF("00000000000000001024", 20, "%020d", 1024)
    TEST_SPRINTF("-0000000000000001024", 20, "%020d", -1024)
    TEST_SPRINTF("00000000000000001024", 20, "%020i", 1024)
    TEST_SPRINTF("-0000000000000001024", 20, "%020i", -1024)
    TEST_SPRINTF("00000000000000001024", 20, "%020u", 1024u)
    TEST_SPRINTF("00000000004294966272", 20, "%020u", -1024u)
    TEST_SPRINTF("00000000000000000777", 20, "%020o", 0777u)
    TEST_SPRINTF("00000000037777777001", 20, "%020o", -0777u)
    TEST_SPRINTF("0000000000001234abcd", 20, "%020x", 0x1234abcdu)
    TEST_SPRINTF("000000000000edcb5433", 20, "%020x", -0x1234abcdu)
    TEST_SPRINTF("0000000000001234ABCD", 20, "%020X", 0x1234abcdu)
    TEST_SPRINTF("000000000000EDCB5433", 20, "%020X", -0x1234abcdu)
}

static void test_print_sharp_width()
{
    TEST_SPRINTF("                0777", 20, "%#20o", 0777u)
    TEST_SPRINTF("        037777777001", 20, "%#20o", -0777u)
    TEST_SPRINTF("          0x1234abcd", 20, "%#20x", 0x1234abcdu)
    TEST_SPRINTF("          0xedcb5433", 20, "%#20x", -0x1234abcdu)
    TEST_SPRINTF("          0X1234ABCD", 20, "%#20X", 0x1234abcdu)
    TEST_SPRINTF("          0XEDCB5433", 20, "%#20X", -0x1234abcdu)
}

static void test_print_sharp_zero_width()
{
    TEST_SPRINTF("00000000000000000777", 20, "%#020o", 0777u)
    TEST_SPRINTF("00000000037777777001", 20, "%#020o", -0777u)
    TEST_SPRINTF("0x00000000001234abcd", 20, "%#020x", 0x1234abcdu)
    TEST_SPRINTF("0x0000000000edcb5433", 20, "%#020x", -0x1234abcdu)
    TEST_SPRINTF("0X00000000001234ABCD", 20, "%#020X", 0x1234abcdu)
    TEST_SPRINTF("0X0000000000EDCB5433", 20, "%#020X", -0x1234abcdu)
}

static void test_print_zero_left_width()
{
    TEST_SPRINTF("Hello               ", 20, "%0-20s", "Hello")
    TEST_SPRINTF("1024                ", 20, "%0-20d", 1024)
    TEST_SPRINTF("-1024               ", 20, "%0-20d", -1024)
    TEST_SPRINTF("1024                ", 20, "%0-20i", 1024)
    TEST_SPRINTF("-1024               ", 20, "%0-20i", -1024)
    TEST_SPRINTF("1024                ", 20, "%0-20u", 1024u)
    TEST_SPRINTF("4294966272          ", 20, "%0-20u", -1024u)
    TEST_SPRINTF("777                 ", 20, "%-020o", 0777u)
    TEST_SPRINTF("37777777001         ", 20, "%-020o", -0777u)
    TEST_SPRINTF("1234abcd            ", 20, "%-020x", 0x1234abcdu)
    TEST_SPRINTF("edcb5433            ", 20, "%-020x", -0x1234abcdu)
    TEST_SPRINTF("1234ABCD            ", 20, "%-020X", 0x1234abcdu)
    TEST_SPRINTF("EDCB5433            ", 20, "%-020X", -0x1234abcdu)
    TEST_SPRINTF("x                   ", 20, "%-020c", 'x')
}

static void test_print_start()
{
    TEST_SPRINTF("               Hello", 20, "%*s", 20, "Hello")
    TEST_SPRINTF("                1024", 20, "%*d", 20, 1024)
    TEST_SPRINTF("               -1024", 20, "%*d", 20, -1024)
    TEST_SPRINTF("                1024", 20, "%*i", 20, 1024)
    TEST_SPRINTF("               -1024", 20, "%*i", 20, -1024)
    TEST_SPRINTF("                1024", 20, "%*u", 20, 1024u)
    TEST_SPRINTF("          4294966272", 20, "%*u", 20, -1024u)
    TEST_SPRINTF("                 777", 20, "%*o", 20, 0777u)
    TEST_SPRINTF("         37777777001", 20, "%*o", 20, -0777u)
    TEST_SPRINTF("            1234abcd", 20, "%*x", 20, 0x1234abcdu)
    TEST_SPRINTF("            edcb5433", 20, "%*x", 20, -0x1234abcdu)
    TEST_SPRINTF("            1234ABCD", 20, "%*X", 20, 0x1234abcdu)
    TEST_SPRINTF("            EDCB5433", 20, "%*X", 20, -0x1234abcdu)
    TEST_SPRINTF("                   x", 20, "%*c", 20, 'x')
}

static void test_print_precision()
{
    TEST_SPRINTF("Hello INWOX", 11, "%.20s", "Hello INWOX")
    TEST_SPRINTF("00000000000000001024", 20, "%.20d", 1024)
    TEST_SPRINTF("-00000000000000001024", 21, "%.20d", -1024)
    TEST_SPRINTF("00000000000000001024", 20, "%.20i", 1024)
    TEST_SPRINTF("-00000000000000001024", 21, "%.20i", -1024)
    TEST_SPRINTF("00000000000000001024", 20, "%.20u", 1024u)
    TEST_SPRINTF("00000000004294966272", 20, "%.20u", -1024u)
    TEST_SPRINTF("00000000000000000777", 20, "%.20o", 0777u)
    TEST_SPRINTF("00000000037777777001", 20, "%.20o", -0777u)
    TEST_SPRINTF("0000000000001234abcd", 20, "%.20x", 0x1234abcdu)
    TEST_SPRINTF("000000000000edcb5433", 20, "%.20x", -0x1234abcdu)
    TEST_SPRINTF("0000000000001234ABCD", 20, "%.20X", 0x1234abcdu)
    TEST_SPRINTF("000000000000EDCB5433", 20, "%.20X", -0x1234abcdu)
}

static void test_print_width_precision()
{
    TEST_SPRINTF("               Hello", 20, "%20.5s", "Hello INWOX")
    TEST_SPRINTF("               01024", 20, "%20.5d", 1024)
    TEST_SPRINTF("              -01024", 20, "%20.5d", -1024)
    TEST_SPRINTF("               01024", 20, "%20.5i", 1024)
    TEST_SPRINTF("              -01024", 20, "%20.5i", -1024)
    TEST_SPRINTF("               01024", 20, "%20.5u", 1024u)
    TEST_SPRINTF("          4294966272", 20, "%20.5u", -1024u)
    TEST_SPRINTF("               00777", 20, "%20.5o", 0777u)
    TEST_SPRINTF("         37777777001", 20, "%20.5o", -0777u)
    TEST_SPRINTF("            1234abcd", 20, "%20.5x", 0x1234abcdu)
    TEST_SPRINTF("          00edcb5433", 20, "%20.10x", -0x1234abcdu)
    TEST_SPRINTF("            1234ABCD", 20, "%20.5X", 0x1234abcdu)
    TEST_SPRINTF("          00EDCB5433", 20, "%20.10X", -0x1234abcdu)
}

static void test_print_zero_width_precision()
{
    TEST_SPRINTF("               Hello", 20, "%020.5s", "Hello INWOX")
    TEST_SPRINTF("               01024", 20, "%020.5d", 1024)
    TEST_SPRINTF("              -01024", 20, "%020.5d", -1024)
    TEST_SPRINTF("               01024", 20, "%020.5i", 1024)
    TEST_SPRINTF("              -01024", 20, "%020.5i", -1024)
    TEST_SPRINTF("               01024", 20, "%020.5u", 1024u)
    TEST_SPRINTF("          4294966272", 20, "%020.5u", -1024u)
    TEST_SPRINTF("               00777", 20, "%020.5o", 0777u)
    TEST_SPRINTF("         37777777001", 20, "%020.5o", -0777u)
    TEST_SPRINTF("            1234abcd", 20, "%020.5x", 0x1234abcdu)
    TEST_SPRINTF("          00edcb5433", 20, "%020.10x", -0x1234abcdu)
    TEST_SPRINTF("            1234ABCD", 20, "%020.5X", 0x1234abcdu)
    TEST_SPRINTF("          00EDCB5433", 20, "%020.10X", -0x1234abcdu)
}

static void test_print_width_precision_2()
{
    TEST_SPRINTF("", 0, "%.0s", "Hello INWOX")
    TEST_SPRINTF("                    ", 20, "%20.0s", "Hello INWOX")
    TEST_SPRINTF("", 0, "%.s", "Hello INWOX")
    TEST_SPRINTF("                    ", 20, "%20.s", "Hello INWOX")
    TEST_SPRINTF("                1024", 20, "%20.0d", 1024)
    TEST_SPRINTF("               -1024", 20, "%20.d", -1024)
    TEST_SPRINTF("                    ", 20, "%20.d", 0)
    TEST_SPRINTF("                1024", 20, "%20.0i", 1024)
    TEST_SPRINTF("               -1024", 20, "%20.i", -1024)
    TEST_SPRINTF("                    ", 20, "%20.i", 0)
    TEST_SPRINTF("                1024", 20, "%20.u", 1024u)
    TEST_SPRINTF("          4294966272", 20, "%20.0u", -1024u)
    TEST_SPRINTF("                    ", 20, "%20.u", 0u)
    TEST_SPRINTF("                 777", 20, "%20.o", 0777u)
    TEST_SPRINTF("         37777777001", 20, "%20.0o", -0777u)
    TEST_SPRINTF("                    ", 20, "%20.o", 0u)
    TEST_SPRINTF("            1234abcd", 20, "%20.x", 0x1234abcdu)
    TEST_SPRINTF("            edcb5433", 20, "%20.0x", -0x1234abcdu)
    TEST_SPRINTF("                    ", 20, "%20.x", 0u)
    TEST_SPRINTF("            1234ABCD", 20, "%20.X", 0x1234abcdu)
    TEST_SPRINTF("            EDCB5433", 20, "%20.0X", -0x1234abcdu)
    TEST_SPRINTF("                    ", 20, "%20.X", 0u)
}

static void test_print_inwox()
{
    TEST_SPRINTF("Hello               ", 20, "% -0+*.*s", 20, 5, "Hello INWOX")
    TEST_SPRINTF("+01024              ", 20, "% -0+*.*d", 20, 5, 1024)
    TEST_SPRINTF("-01024              ", 20, "% -0+*.*d", 20, 5, -1024)
    TEST_SPRINTF("+01024              ", 20, "% -0+*.*i", 20, 5, 1024)
    TEST_SPRINTF("-01024              ", 20, "% 0-+*.*i", 20, 5, -1024)
    TEST_SPRINTF("01024               ", 20, "% 0-+*.*u", 20, 5, 1024u)
    TEST_SPRINTF("4294966272          ", 20, "% 0-+*.*u", 20, 5, -1024u)
    TEST_SPRINTF("00777               ", 20, "%+ -0*.*o", 20, 5, 0777u)
    TEST_SPRINTF("37777777001         ", 20, "%+ -0*.*o", 20, 5, -0777u)
    TEST_SPRINTF("1234abcd            ", 20, "%+ -0*.*x", 20, 5, 0x1234abcdu)
    TEST_SPRINTF("00edcb5433          ", 20, "%+ -0*.*x", 20, 10, -0x1234abcdu)
    TEST_SPRINTF("1234ABCD            ", 20, "% -+0*.*X", 20, 5, 0x1234abcdu)
    TEST_SPRINTF("00EDCB5433          ", 20, "% -+0*.*X", 20, 10, -0x1234abcdu)
}

static void test_sprintf()
{
    test_print_base();
    test_print_type();
    test_print_plus();
    test_print_space();
    test_print_plus_space();
    test_print_sharp();
    test_print_width();
    test_print_width_2();
    test_print_left_width();
    test_print_zero_width();
    test_print_sharp_width();
    test_print_sharp_zero_width();
    test_print_zero_left_width();
    test_print_start();
    test_print_precision();
    test_print_width_precision();
    test_print_zero_width_precision();
    test_print_width_precision_2();
    test_print_inwox();
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    test_sprintf();
    printf("Test: %d/%d\n", test_count - test_failed, test_count);
    return 0;
}
