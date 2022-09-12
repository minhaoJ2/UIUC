/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"

char **parse_args(int argc, char **argv);
verb check_args(char **args);

int connect_server(const char* host, const char* port) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(s));
        exit(1);
    }
    int sock_fd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (sock_fd == -1) {
        perror("Socket failed\n");
        exit(1);
    }
    int ok = connect(sock_fd, res -> ai_addr, res -> ai_addrlen);
    if (ok == -1) {
        perror("Connect failed\n");
        exit(1);
    }
    freeaddrinfo(res);
    return sock_fd;
}

void send_request(int sock_fd, verb request, char** args) {
    char* message = NULL;
    if (request == LIST) {
        message = calloc(1, 6);
        sprintf(message,"%s\n","LIST");
    } else if (request == PUT){
        message = calloc(1, strlen(args[3]) + 6);
        sprintf(message,"%s %s\n","PUT",args[3]);
    } else if (request == GET) {
        message = calloc(1,strlen(args[3]) + 6);
        sprintf(message,"%s %s\n","GET",args[3]);
    } else if (request == DELETE) {
        message = calloc(1,strlen(args[3]) + 9);
        sprintf(message,"%s %s\n","DELETE",args[3]);
    } else {
        message = calloc(1, strlen(args[2]) + strlen(args[3]) + 3);
        sprintf(message, "%s %s\n", args[2], args[3]);
    }
    ssize_t length = strlen(message);
    if (write_to_socket(sock_fd, message, length) < length) {
        print_connection_closed();
        free(message);
        exit(1);
    }
    free(message);

    if (request == PUT) {
        FILE* file = fopen(args[4], "r");
        if (file == NULL) {
            fprintf(stdout, "Local file open failed. No such file\n");
            exit(1);
        }
        struct stat buf;
        if (stat(args[4], &buf) == -1) {
            exit(1);
        }
        size_t size = buf.st_size;
        write_to_socket(sock_fd, (char*)&size, sizeof(size_t));
        size_t bytesRead = 0;
        while (bytesRead < size) {
            ssize_t size_r = (size - bytesRead) > 1024 ? 1024 : (size - bytesRead);
            char buffer[size_r + 1];
            fread(buffer, 1, size_r, file);
            if (write_to_socket(sock_fd, buffer, size_r) < size_r) {
                print_connection_closed();
                fclose(file);
                exit(1);
            }
            bytesRead += size_r;
        }
        fclose(file);
    }
}

void read_response(int sock_fd, verb request, char** args) {
    char *buffer = calloc(1, 4);
    size_t bytes_rd = read_from_socket(sock_fd, buffer, 3);
    if (strncmp(buffer, "OK\n", 3) == 0) {
        fprintf(stdout, "%s", buffer);
        if (request == DELETE || request == PUT) {
            print_success();
        } else if (request == GET) {
            FILE *file = fopen(args[4], "w+");
            if (!file) {
                perror(NULL);
                exit(1);
            }
            size_t size;
            read_from_socket(sock_fd, (char *)&size, sizeof(size_t));
            size_t bytes_read = 0;
            while (bytes_read < size + 5) {
                size_t size_hd = (size + 5 - bytes_read) > 1024 ? 1024 : (size + 5-bytes_read);
                char buffer_f[1024+1] = {0};
                size_t rc = read_from_socket(sock_fd, buffer_f, size_hd);
                fwrite(buffer_f, 1, rc, file);
                bytes_read += rc;
                if (rc == 0) {
                    break;
                }
            }
            if (bytes_read == 0 && bytes_read != size) {
                print_connection_closed();
                free(buffer);
                exit(1);
            } else if (bytes_read < size) {
                print_too_little_data();
                free(buffer);
                exit(1);
            } else if (bytes_read > size) {
                print_received_too_much_data();
                free(buffer);
                exit(1);
            }
            fclose(file);
        } else if (request == LIST) {
            size_t size;
            read_from_socket(sock_fd, (char *)&size, sizeof(size_t));
            char buffer_f[size+5+1];
            memset(buffer_f, 0, size+5+1);
            bytes_rd = read_from_socket(sock_fd, buffer_f, size+5);
            if (bytes_rd == 0 && bytes_rd != size) {
                print_connection_closed();
                free(buffer);
                exit(1);
            } else if (bytes_rd < size) {
                print_too_little_data();
                free(buffer);
                exit(1);
            } else if (bytes_rd > size) {
                print_received_too_much_data();
                free(buffer);
                exit(1);
            }
            fprintf(stdout, "%zu %s\n", size, buffer_f);
        }
    } else {
        buffer = realloc(buffer, 7);
        read_from_socket(sock_fd, buffer+bytes_rd, 6- bytes_rd);
        if (strncmp(buffer, "ERROR\n", 6) == 0) {
            fprintf(stdout, "%s", buffer);
            char err[100] = {0};
            if (!read_from_socket(sock_fd, err, 100)) {
                print_connection_closed();
            }
            print_error_message(err);
        } else {
            // printf("%s\n", buffer);
            print_invalid_response();
        }
    }
    free(buffer);
}
int main(int argc, char **argv) {
    // Good luck!
    char** args = parse_args(argc, argv);
    verb request = check_args(args);
    int sock_fd = connect_server(args[0], args[1]);
    send_request(sock_fd, request, args);
    if (shutdown(sock_fd, SHUT_WR) != 0) {
        perror("Shutdown failed");
    }
    read_response(sock_fd, request, args);
    shutdown(sock_fd, SHUT_RD);
    close(sock_fd);
    free(args);
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
