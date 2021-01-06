#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fputs("Missing operand.\n", stderr);
        return 1;
    }

    unsigned long time = strtoul(argv[1], NULL, 10);
    sleep(time);
}