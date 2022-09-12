/**
 * utilities_unleashed
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "format.h"
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        print_env_usage();
    }
    int count = 0;
    char** temp = argv;
    while (count < argc && strcmp(*temp, "--")) {
       	count++;
        temp++;
    }
    if (count >= argc) {
        print_env_usage();
    }
    pid_t child = fork();
    if (child < 0) {
        print_fork_failed();
        exit(1);
    } else if (child > 0) {
        int status = 0;
        waitpid(child, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        char** temp = argv;
        temp = temp + 1;
        while (strcmp(*temp, "--") != 0) {
            int hasEqual = 0;
            char *t = *temp;
            while (*t) {
                if ((*t) == '=') {
                    hasEqual = 1;
                    break;
                }
                t++;
            }
            if (!hasEqual) {
                print_env_usage();
            } else {
                char* key = strtok(*temp, "=");
                char* value = strtok(NULL, "=");
                if (value[0] == '%') {
                    value = getenv(value + 1);
                }
                int res = setenv(key, value, 1);
                if (res == -1) {
                    print_environment_change_failed();
                }
            }
            temp++;
        }
        temp = temp + 1;
        execvp(*temp, temp);
        print_exec_failed();
        exit(1);
        return 0;
    }
    return 0;
}
