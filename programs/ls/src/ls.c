#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static void listDirectory(const char* path);

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        listDirectory(".");
    }
    for (int i = 1; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) < 0) continue;

        if (S_ISDIR(st.st_mode)) {
            listDirectory(argv[i]);
        } else {
            puts(argv[i]);
        }
    }
}

static void listDirectory(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) return;

    struct dirent* entry = readdir(dir);

    while (entry) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            puts(entry->d_name);
        }

        entry = readdir(dir);
    }

    closedir(dir);
}
