/**
 * shell
 * CS 241 - Fall 2020
 */
#include "format.h"
#include "shell.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>
#include <sstring.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
typedef struct process {
    char *command;
    pid_t pid;
} process;

static vector* all_process;

vector *read_history(FILE *history_file) {
    vector *history = string_vector_create();
    char *line = NULL;
    size_t len = 0;
    if (history_file) {
        ssize_t bytesRead;
        while (true) { // parse the txt file
            bytesRead = getline(&line, &len, history_file);
            if (bytesRead == -1) {
                break;
            } else if (line[bytesRead - 1] == '\n') {
                line[bytesRead - 1] = 0;
            }
            vector_push_back(history, line);
            free(line);
            line = NULL;
        }
        free(line);
    }
    return history;
}

char *join(vector *v) {// combine the command into a string to store into the process vec
    if (vector_empty(v)) {
        return NULL;
    }
    sstring *del = cstr_to_sstring(" ");
    sstring *s = cstr_to_sstring("");
    size_t i = 0;
    for (; i < vector_size(v) - 1; i++) {
        sstring *temp = cstr_to_sstring(vector_get(v, i));
        sstring_append(s, temp);
        sstring_append(s, del);
        sstring_destroy(temp);
    }
    sstring *end = cstr_to_sstring(vector_get(v, vector_size(v)-1));
    sstring_append(s, end);
    sstring_destroy(end);
    sstring_destroy(del);
    char *res = sstring_to_cstr(s);
    sstring_destroy(s);
    return res;
}

void write_history(FILE *file, vector *history, char *line) {
    //write to history vector
    vector_push_back(history, line);
    if (file) {
        fprintf(file, "%s\n", line);
        fflush(file);
    }
    return;
}

void pid_erase(vector* v, pid_t pid) {
    int i = vector_size(v) - 1;
    for(; i >= 0; i--) {
        process* p = vector_get(v, i);
        if (p -> pid == pid) {
            free(p -> command);
            free(p);
            vector_erase(all_process, i);
            return;
        }
    }
}

void handle_sigint() {
    size_t i = 0;
    for (; i < vector_size(all_process); i++) {
    process *p = (process *) vector_get(all_process, i);
    if (p -> pid != getpgid(p -> pid) ){
        kill(p -> pid, SIGKILL);
        pid_t pid = p -> pid;
        pid_erase(all_process, pid);
    }
  }
}

int builtInCommand(vector* command) {
    // TODO: Handle Built in command.
    if (vector_size(command) == 0) {
        return 0;
    }
    int background = 0;
    size_t size = vector_size(command);
    if (strcmp((char*) vector_get(command, vector_size(command) - 1), "&") == 0) {
        vector_pop_back(command);
        size = size - 1;
        background = 1;
    } else {
        char* last = (char*) vector_get(command, size - 1);
        if (last[strlen(last) - 1] == '&') {
            last[strlen(last) - 1] = 0;
            background = 1;
        }
    }
    char* command_string[size + 1];
    size_t i = 0;
    for (; i < size; i++) {
        command_string[i] = vector_get(command, i);
    }
    command_string[size] = NULL;
    if (strcmp(command_string[0], "cd") == 0) {
        if (size == 1) {
            print_no_directory("");
            return 0;
        } else {
            int res = chdir(vector_get(command, 1));
            if (res == -1) {
                print_no_directory(vector_get(command, 1));
                return 0;
            }
            return 1;
        }
    }
    process* p = malloc(sizeof(process));
    p -> command = join(command);
    vector_push_back(all_process, p);
    fflush(stdout);
    fflush(stdin);
    pid_t pid = fork();
    if (pid < 0) {
        print_fork_failed();
        exit(1);
    } else if (pid > 0) { // parent
        print_command_executed(pid);
        p -> pid = pid;
        if (background == 1) {
            if(setpgid(pid, pid) == -1) {
                print_setpgid_failed();
                exit(1);
            }
        } else {
            int status = 0;
            if (waitpid(pid, &status, 0) == -1) {
                print_wait_failed();
                exit(1);
            }
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                pid_erase(all_process, pid);
                return 1;
            } else {
                pid_erase(all_process, pid);
            }
        }
    } else {
        char* redirection = NULL;
        size_t i = 0;
        char** temp = (char**) calloc(size - 2, sizeof(char*));
        char* fileName = NULL;
        for (; i < size - 1; i++) {
            if (strcmp(command_string[i], ">") == 0 || strcmp(command_string[i], ">>") == 0 || strcmp(command_string[i], "<") == 0) {
                redirection = command_string[i];
                fileName = command_string[i + 1];
                break;
            }
            temp[i] = command_string[i];
        }
        if (redirection == NULL) {
            execvp(command_string[0], command_string);
            print_exec_failed(command_string[0]);
            exit(1);
        } else {
            fflush(stdout);
            fflush(stdin);
            if (strcmp(redirection, ">") == 0) {
                mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP;
                int file = open(fileName, O_TRUNC | O_WRONLY, mode);
                if (file < 0) {
                    print_script_file_error();
                    exit(1);
                }
                dup2(file, STDOUT_FILENO);
                close(file);
                execvp(temp[0], temp);
                print_exec_failed(temp[0]);
                exit(1);
            } else if (strcmp(redirection, ">>") == 0) {
                mode_t mode = S_IRWXU;
                int file = open(fileName, O_CREAT | O_APPEND | O_WRONLY, mode);
                dup2(file, STDOUT_FILENO);
                close(file);
                execvp(temp[0], temp);
                print_exec_failed(temp[0]);
                exit(1);
            } else if (strcmp(redirection, "<") == 0) {
                mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP;
                int file = open(fileName, O_RDONLY, mode);
                if (file < 0) {
                    print_script_file_error();
                    exit(1);
                }
                dup2(file, STDIN_FILENO);
                close(file);
                execvp(temp[0], temp);
                print_exec_failed(temp[0]);
                exit(1);
            }
        }
    }
    return 0;
}

int exec_command(vector* command) {
    vector* temp = string_vector_create();
    size_t i = 0;
    for(; i < vector_size(command); i++) {
        if (strcmp(vector_get(command, i), "&&") == 0) {
            int res = builtInCommand(temp);
            if (res == 1) {
                vector_clear(temp);
            } else {
                vector_destroy(temp);
                return 1;
            }
        } else if (strcmp(vector_get(command, i), "||") == 0) {
            if (builtInCommand(temp) == 1) {
                vector_destroy(temp);
                return 0;
            } else {
                vector_clear(temp);
            }
        } else if (((char *)vector_get(command, i))[strlen(vector_get(command, i)) - 1] == ';') {
            char *t = (char *)vector_get(command, i);
            t[strlen(t) - 1] = 0;
            if (strlen(t) > 0)
                vector_push_back(temp, t);
            t[strlen(t)] = ';';
            builtInCommand(temp);
            vector_clear(temp);
            continue;
        } else {
            vector_push_back(temp, vector_get(command, i));
        }
    }
    int res = builtInCommand(temp);
    vector_destroy(temp);
    return res;
}

void ps() {
    print_process_info_header();
    size_t i = 0;
    for (; i < vector_size(all_process); i++) {
        char buffer[100];
        process* p = vector_get(all_process, i);
        sprintf(buffer, "/proc/%d/stat", p -> pid);
        FILE* file = fopen(buffer, "r");
        if (file == NULL) {
            print_script_file_error();
            exit(1);
        }
        process_info info;
        info.pid = p -> pid;
        info.command = p -> command;
        unsigned long utime;
        unsigned long stime;
        unsigned long long starttime;
        fscanf(file, "%*d %*s %c %*d %*d %*d %*d %*d %*u %*lu ", &(info.state));
        fscanf(file, "%*lu %*lu %*lu %lu %lu %*ld %*ld %*ld %*ld %ld ", &utime, &stime, &(info.nthreads));
        fscanf(file, "%*ld %llu %lu", &starttime, &(info.vsize));
        info.vsize = info.vsize / 1024;
        char t_buffer[6];
        FILE* f = fopen("/proc/uptime", "r");
        unsigned long long uptime;
        fscanf(f, "%llu", &uptime);
        fclose(f);
        time_t totaltime = starttime / sysconf(_SC_CLK_TCK) + time(NULL) - uptime;
        struct tm* tm_info = localtime(&totaltime);
        char start_buffer[20];
        if (time_struct_to_string(start_buffer, 20, tm_info) == 0) {
            exit(1);
        }
        info.start_str = calloc(strlen(start_buffer) + 1, sizeof(char));
        strcpy(info.start_str, start_buffer);
        size_t cpu_time = (utime + stime) / sysconf(_SC_CLK_TCK);
        execution_time_to_string(t_buffer, 6, cpu_time / 60, cpu_time % 60);
        info.time_str = t_buffer;
        print_process_info(&info);
        fclose(file);
    }
}

void kill_all_process() {
    size_t i = 0;
    for (; i < vector_size(all_process); i++) {
        process* p = vector_get(all_process, i);
        kill(p -> pid, SIGKILL);
    }
    size_t j = 0;
    for(; j < vector_size(all_process); j++) {
        process* p = (process*) vector_get(all_process, j);
        free(p -> command);
        free(p);
    }
    vector_destroy(all_process);
}

void kill_process(char* pidString, char* command) {
    pid_t pid;
    if (pidString == NULL || strlen(pidString) == 0) {
        print_invalid_command(command);
        return;
    }
    int res = sscanf(pidString, "%d", &pid);
    if (res == 0) {
        print_invalid_command(command);
        return;
    }
    int result = kill(pid, SIGKILL);
    if (result == 0) {
        print_killed_process(pid, command);
        pid_erase(all_process, pid);
    } else {
        print_no_process_found(pid);
    }
}

void stop_process(char* pidString, char* command) {
    pid_t pid;
    if (pidString == NULL || strlen(pidString) == 0) {
        print_invalid_command(command);
        return;
    }
    int res = sscanf(pidString, "%d", &pid);
    if (res == 0) {
        print_invalid_command(command);
        return;
    }
    int result = kill(pid, SIGSTOP);
    if (result == 0) {
        print_killed_process(pid, command);
    } else {
        print_no_process_found(pid);
    }
}

void cont_process(char* pidString, char* command) {
    pid_t pid;
    if (pidString == NULL || strlen(pidString) == 0) {
        print_invalid_command(command);
        return;
    }
    int res = sscanf(pidString, "%d", &pid);
    if (res == 0) {
        print_invalid_command(command);
        return;
    }
    int result = kill(pid, SIGCONT);
    if (result != 0) {
        print_no_process_found(pid);
    }
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    // signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, handle_sigint);
    char* history_file = NULL;
    char* script_file = NULL;

    all_process = shallow_vector_create();
    process p;
    p.command = argv[0];
    p.pid = getpid();
    vector_push_back(all_process, &p);
    int count = 1;
    int optres;
    while((optres = getopt(argc, argv, "h:f:"))!= -1) {
        if (optres == 'h') {
            history_file = optarg;
            count += 2;
            break;
        } else if (optres == 'f') {
            script_file = optarg;
            count += 2;
            break;
        }
    }
    if (count != argc) {
        print_usage();
        exit(1);
    }
    FILE* file_h = NULL;
    char* history_path;
    if (history_file != NULL) {
        // history mode
        history_path = get_full_path(history_file);
        file_h = fopen(history_path, "a+");
        if (file_h == NULL) {
            print_history_file_error();
            exit(1);
        }
    }
    vector* history = read_history(file_h);
    FILE* file;
    if (script_file == NULL) {
        file = stdin;
    } else {
        file = fopen(script_file, "r");
        if (file == NULL) {
            print_script_file_error();
            exit(1);
        }
    }
    char* buffer_h = NULL;
    size_t file_size = 0;
    ssize_t bytesRead;
    char full_path[1000];
    print_prompt(getcwd(full_path, 1000), getpid());
    fflush(stdout);
    while (true) {
        bytesRead = getline(&buffer_h, &file_size, file);
        if (bytesRead < 0) {
            break;
        } else if (buffer_h[bytesRead - 1] == '\n') {
            buffer_h[bytesRead - 1]  = 0;
            if (file != stdin) {
                print_command(buffer_h);
                fflush(stdout);
            }
        }
        //get the first word, which is the command.
        vector* temp;
        char* t = buffer_h;
        sstring* buffer_string = cstr_to_sstring(t);
        temp = sstring_split(buffer_string, ' ');
        sstring_destroy(buffer_string);
        size_t i = 0;
        for (; i < vector_size(temp); i++) {
            if (strcmp(vector_get(temp, i), "") == 0) {
                vector_erase(temp, i);
            }
        }
        char* command = (char*) vector_get(temp, 0);
        //Command cases
        if (vector_size(temp) > 0) {
            if (strcmp(command, "cd") == 0) {
                write_history(file_h, history, buffer_h);
                sstring* cd = cstr_to_sstring(buffer_h);
                vector* t = sstring_split(cd, ' ');
                exec_command(t);
            } else if (strcmp(command, "!history") == 0) {
                size_t i = 0;
                for (; i < vector_size(history); i++) {
                    print_history_line(i, vector_get(history, i));
                }
            } else if (strcmp(command, "exit") == 0) {
                break;
            } else if (command[0] == '!') {// find the latest command with the same prefix
                int found_index = -1; // -1 means no found.
                char* prefix = buffer_h + 1;
                int i = vector_size(history) - 1;
                for (; i >= 0; i--) {
                    if (strlen(prefix) <= strlen(vector_get(history, i))) {
                        if (strncmp(vector_get(history, i), prefix, strlen(prefix)) == 0) {
                            found_index = i;
                            break;
                        }
                    }
                }
                if (found_index == -1) { // no found
                    print_no_history_match();
                } else {
                    print_command(vector_get(history, found_index));
                    write_history(file_h, history, vector_get(history, found_index));
                    sstring* t1 = cstr_to_sstring(vector_get(history, found_index));
                    vector* temp1 = sstring_split(t1, ' ');
                    exec_command(temp1);
                    fflush(stdout);
                    fflush(stdin);
                    sstring_destroy(t1);
                    vector_destroy(temp1);
                }
            } else if (command[0] == '#') {//query the history vector, execute the nth command
                if (strlen(command) <= 1) {
                    print_invalid_index();
                } else {
                    int index = atoi(strtok(buffer_h, "#"));
                    if (index < 0 || index > (int) vector_size(history)) {
                        print_invalid_index();
                    } else {
                        print_command(vector_get(history, index));
                        write_history(file_h, history, vector_get(history, index));
                        sstring* t2 = cstr_to_sstring(vector_get(history, index));
                        vector* temp2 = sstring_split(t2, ' ');
                        fflush(stdout);
                        fflush(stdin);
                        exec_command(temp2);
                        sstring_destroy(t2);
                        vector_destroy(temp2);
                    }
                }
            } else if(strcmp(command, "kill") == 0) {
                kill_process(vector_get(temp, 1), buffer_h);
            } else if (strcmp(command, "stop") == 0) {
                stop_process(vector_get(temp, 1), buffer_h);
            } else if (strcmp(command, "cont") == 0) {
                cont_process(vector_get(temp, 1), buffer_h);
            } else if (strcmp(command, "ps") == 0) {
                ps();
            } else {
                write_history(file_h, history, buffer_h);
                fflush(stdout);
                fflush(stdin);
                exec_command(temp);
            }
        }
        vector_destroy(temp);
        print_prompt(getcwd(buffer_h, 1000), getpid());
        free(buffer_h);
        buffer_h = NULL;
    }
    free(buffer_h);
    buffer_h = NULL;
    vector_destroy(history);
    return 0;
}
