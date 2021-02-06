#include <stdio.h>
#include <sys/utsname.h>

int needSpace = 0;

static void putInfo(char *str) {
    if (needSpace) {
        fputc(' ', stdout);
    }
    needSpace = 1;
    fputs(str, stdout);
}

int main(int argc, char *argv[])
{
    // TODO: 参数解析
    (void)argc;
    (void)argv;
    struct utsname u;
    uname(&u);
    putInfo(u.sysname);
    putInfo(u.nodename);
    putInfo(u.release);
    putInfo(u.version);
    putInfo(u.machine);
    fputc('\n', stdout);
    return 0;
}
