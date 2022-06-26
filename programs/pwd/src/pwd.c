#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static bool isAbsolutePath(const char* path) {
    if (*path != '/') return false;
    while (*path) {
        if (path[0] == '.' && (path[1] == '/' || path[1] == '\0' ||
                (path[1] == '.' && (path[2] == '/' || path[2] == '\0')))) {
            return false;
        }

        while (*path && *path++ != '/');
    }
    return true;
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    bool logical = true;

    const char* pwd = getenv("PWD");
    if (logical && pwd && isAbsolutePath(pwd)) {
        puts(pwd);
    } else {
        pwd = getcwd(NULL, 0);
        if (!pwd) perror("getcwd");
        puts(pwd);
    }
}
