#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    printf("\e[2J\e[H");
    return 0;
}
