/**
 * mapreduce
 * partners: chenlei2, xinyhe4, yinmeng2, minhaoj2
 * CS 241 - Fall 2020
 */
#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


/* 
 * input format:
 * ./mapreduce <input_file> <output_file> <mapper_executable> <reducer_executable> <mapper_count>
 */
int main(int argc, char **argv) {
    if(argc != 6) {
        print_usage();
        return 0;
    }
    // Create an input pipe for each mapper.
    int count = atoi(argv[5]);
    printf("count: %d\n", count);
    int** mappers = (int**)calloc(count, sizeof(int*));
    for(int i = 0; i < count; i++) {
        mappers[i] = (int*)calloc(2, sizeof(int));
        if(pipe(mappers[i]) == -1) return 1;
    }

    // Create one input pipe for the reducer.
    int* reducers = (int*)calloc(2, sizeof(int));
    if(pipe(reducers) == -1) return 1;

    // Open the output file.
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);

    // Start a splitter process for each mapper.
    pid_t* s_pids = (pid_t *)calloc(count, sizeof(pid_t));
    for(int i = 0; i < count; i++) {
        s_pids[i] = fork();
        if(s_pids[i] == 0) { /* child process */
            char arg[12];
            snprintf(arg, 12, "%d", (int)i);
            close(mappers[i][0]);
            if(dup2(mappers[i][1], 1) == -1) return 1;
            /* ./splitter inputfile 3 0 | my_mapper */
            int error = execl("./splitter", "./splitter", argv[1], argv[5], arg, NULL);
            exit(error);
        }
    }

    // Start all the mapper processes.
    // read from mapper, then write to reduce
    pid_t* m_pids = (pid_t *)calloc(count, sizeof(pid_t));
    for(int i = 0; i < count; i++) {
        close(mappers[i][1]);
        m_pids[i] = fork();
        if(m_pids[i] == 0) {
            close(reducers[0]);
            if(dup2(mappers[i][0], 0) == -1) return 1;
            if(dup2(reducers[1], 1) == -1) return 1;
            /* mapper_function func */
            int error = execl(argv[3], argv[3], NULL);
            exit(error);
        }
    }

    // Start the reducer process.
    // read from reducer and write to ouput
    close(reducers[1]); // close the write end!!
    pid_t r_pid = fork();
    if(r_pid == 0) {
        if(dup2(reducers[0], 0) == -1) return 1;
        if(dup2(output_fd, 1) == -1) return 1;
        int error = execl(argv[4], argv[4], NULL);
        exit(error);
    }
    
    // Wait for the reducer to finish.
    // Print nonzero subprocess exit codes.

    
    for(int i = 0; i < count; i++){
        int status;
        waitpid(s_pids[i], &status, 0);
    }

    for(int i = 0; i < count; i++){
        int status;
        waitpid(m_pids[i], &status, 0);
        if(status) print_nonzero_exit_status(argv[3], status);
    }

    int r_status;
    waitpid(r_pid, &r_status, 0);
    if(r_status) print_nonzero_exit_status(argv[4], r_status);

    // Count the number of lines in the output file.
    print_num_lines(argv[2]);

    // free
    for(int i = 0; i < count; i++) {
        free(mappers[i]);
    }

    free(reducers);
    free(s_pids);
    free(m_pids);
    return 0;
}
