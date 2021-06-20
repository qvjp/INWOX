/** MIT License
 *
 * Copyright (c) 2021 Qv Junping
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
 * libc/src/stdlib/canonicalize_file_name.c
 * canonicalize_file_name
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char* get_entry_name(DIR* dir, dev_t dev, ino_t ino) {
    struct dirent* dirent = readdir(dir);
    while (dirent) {
        if (dirent->d_dev == dev && dirent->d_ino == ino) {
            return dirent->d_name;
        }

        dirent = readdir(dir);
    }

    return NULL;
}

char* canonicalize_file_name(const char* path)
{
    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    struct stat st;
    if (stat(path, &st) < 0) {
        return NULL;
    }
    int fd;
    if (S_ISDIR(st.st_mode)) {
        int currentFd = open(path, O_DIRECTORY | O_SEARCH);
        if (currentFd < 0) {
            return NULL;
        }
        fd = openat(currentFd, "..", O_DIRECTORY | O_SEARCH);
        close(currentFd);
    } else {
        char* pathCopy = strdup(path);
        if (!pathCopy) {
            return NULL;
        }
        char* slash = strrchr(pathCopy, '/');
        while (slash && !slash[1]) {
            *slash = '\0';
            slash = strrchr(pathCopy, '/');
        }

        if (slash) {
            slash[1] = '\0';
            fd = open(pathCopy, O_DIRECTORY | O_SEARCH);
        } else {
            fd = open(".", O_DIRECTORY | O_SEARCH);
        }
        free(pathCopy);
    }

    if (fd < 0) {
        return NULL;
    }

    char* name = malloc(1);
    if (!name) {
        return NULL;
    }
    *name = '\0';
    size_t length = 0;

    while (1) {
        DIR* dir = fdopendir(fd);
        if (!dir) {
            close(fd);
            free(name);
            return NULL;
        }
        char* filename = get_entry_name(dir, st.st_dev, st.st_ino);
        if (!filename) {
            closedir(dir);
            free(name);
            return NULL;
        }

        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            closedir(dir);
            if (!*name) {
                free(name);
                return strdup("/");
            }
            return name;
        }

        length += strlen(filename) + 1;
        char* newName = malloc(length + 1);
        if (!newName) {
            closedir(dir);
            free(name);
            return NULL;
        }

        stpcpy(stpcpy(stpcpy(newName, "/"), filename), name);
        free(name);
        name = newName;

        if (fstat(fd, &st) < 0) {
            closedir(dir);
            free(name);
            return NULL;
        }

        fd = openat(fd, "..", O_DIRECTORY | O_SEARCH);
        closedir(dir);
        if (fd < 0) {
            free(name);
            return NULL;
        }
    }
}
