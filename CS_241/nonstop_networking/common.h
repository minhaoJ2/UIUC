/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#pragma once
#include <stddef.h>
#include <sys/types.h>

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;
ssize_t read_from_socket(int sock_fd, char* buffer, size_t count);
ssize_t write_to_socket(int sock_fd, char* buffer, size_t count);
size_t read_from_until_newline(int sock_fd, char* buffer, size_t count);
