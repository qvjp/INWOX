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
 * tools/shell.c
 * 简单shell
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int cd(int argc, char *argv[])
{
    const char *newCwd;
    if (argc >= 2) {
        newCwd = argv[1];
    } else {
        newCwd = getenv("HOME");
        if (!newCwd) {
            fputs("HOME not set\n", stderr);
            return 1;
        }
    }
    if (chdir(newCwd) == -1) {
        fputs("Error: chdir failed\n", stderr);
        return 1;
    }
    return 0;
}

static const char *getExecutablePath(const char *command)
{
    size_t commandLength = strlen(command);
    const char *path = getenv("PATH");

    while (*path) {
        size_t length = strcspn(path, ":");
        char *buffer = malloc(commandLength + length + 2);

        memcpy(buffer, path, length);
        buffer[length] = '/';
        memcpy(buffer + length + 1, command, commandLength);
        buffer[commandLength + length + 1] = '\0';

        if (access(buffer, X_OK) == 0) {
            return buffer;
        }

        free(buffer);
        path += length + 1;
    }
    return NULL;
}

static int executeCommand(int argc, char *arguments[])
{
    const char *command = arguments[0];
    // shell 内置命令
    if (strcmp(command, "exit") == 0) {
        exit(0);
    }
    if (strcmp(command, "cd") == 0) {
        return cd(argc, arguments);
    }

    pid_t pid = fork();
    if (pid < 0) {
        fputs("fork() failed\n", stderr);
        return -1;
    } else if (pid == 0) {
        if (!strchr(command, '/')) {
            command = getExecutablePath(command);
        }
        if (command) {
            if (execv(command, arguments) == -1) {
                printf("%s: %s\n", command, strerror(errno));
                _Exit(127);
            }
        }
        fputs("Bad command\n", stderr);
        _Exit(127);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    return -1;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    while (true) {
        fputs("$ ", stdout);
        char buffer[81];

        fgets(buffer, sizeof(buffer), stdin);
        size_t length = strlen(buffer);
        if (length == 1 && buffer[0] == '\n') {
            continue;
        }
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }

        // 处理参数
        size_t argumentCount = 1;
        for (size_t i = 0; buffer[i]; i++) {
            if (buffer[i] == ' ') {
                argumentCount++;
            }
        }
        char **arguments = malloc((argumentCount + 1) * sizeof(char *));
        char *token = strtok(buffer, " ");
        size_t argCount = 0;
        while (token) {
            arguments[argCount++] = token;
            token = strtok(NULL, " ");
        }
        
        arguments[argCount] = NULL;

        executeCommand(argCount, arguments);
        free(arguments);
    }
}