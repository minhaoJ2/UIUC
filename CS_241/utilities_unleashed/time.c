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

#include <math.h>

int main(int argc, char *argv[]) {
    struct timespec begin;
    struct timespec end;
    if (argc <= 1) {
        print_time_usage();
        exit(1);
    } else {
        pid_t child = fork();
        if (child < 0) {
            print_fork_failed();
        } else if (child > 0) {
            clock_gettime(CLOCK_MONOTONIC, &begin);
            int status = 0;
            waitpid(child, &status, 0);
            clock_gettime(CLOCK_MONOTONIC, &end);
            double time_elapse = (end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec) * pow(10, -9);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                display_results(argv, time_elapse);
                return 0;
            }
        } else {
            execvp(argv[1], (argv + 1));
            print_exec_failed();
            exit(1);
        }
    }
    return 0;
}
