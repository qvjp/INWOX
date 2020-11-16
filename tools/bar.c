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
 * tools/bar.c
 * 测试模块，打开文件，关闭文件，读文件
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;
    printf("Hello %s from userspace!\n", "World");
    FILE *inwox = fopen("/bin/inwox", "r");
    if (!inwox) {
        printf("Failed to open /bin/inwox\n");
        return -1;
    }
    char buffer[96];
    while (fgets(buffer, sizeof(buffer), inwox)) {
        fputs(buffer, stdout);
    }
    fclose(inwox);
    while (1) {
        printf("open some file: ");
        fgets(buffer, sizeof(buffer), stdin);

        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }
        if (strcmp(buffer, "exit") == 0) {
            puts("Exiting.");
            return 22;
        }
        if (strcmp(buffer, "test") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execl("/bin/foo", "hello", "inwox", NULL);
                printf("test failed\n");
            }
        }
        if (strcmp(buffer, "test2") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                char *const args[] = {"hello", "shenme", NULL};
                execv("/bin/foo", args);
                printf("test failed\n");
            }
        }
        if (strncmp(buffer, "/bin/", 5) == 0) {
            printf("Run %s\n", buffer);
            pid_t pid = fork();
            if (pid == -1) {
                printf("fork() failed\n");
            } else if (pid == 0) {
                printf("Exec new process\n");
                char *const args[] = {NULL};
                if (execv(buffer, args) == -1) {
                    printf("execv() failed: %d\n", errno);
                }
            } else {
                printf("New process has pid %d, waiting...\n", pid);
                int status = 0;
                waitpid(pid, &status, 0);
                if (WEXITSTATUS(status)) {
                    printf("Child process exited with status %u\n", status);
                }
            }
            continue;
        }
        FILE* file = fopen(buffer, "r");
        if (!file) {
            printf("Failed to open file '%s'\n", buffer);
            continue;
        }
        while (fgets(buffer, sizeof(buffer), file)) {
            fputs(buffer, stdout);
        }
        fclose(file);
    }
}
