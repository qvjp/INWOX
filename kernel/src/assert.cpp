/**
 * kernel/src/assert.cpp
 * 断言
 */
#include <assert.h>
#include <inwox/kernel/print.h> /* printf() */

/**
 * 断言，调用此函数打印诊断信息并终止执行
 */
extern "C" void __assert(const char* e, const char* file, unsigned int line, const char* function)
{
    Print::warnTerminal();
    Print::printf("Assertion failed: (%s), function %s, file %s, line %u.\n", e, function, file, line);
    while (1)
        __asm__ __volatile("cli; hlt");
}
