/**
 * charming_chatroom
 * CS 241 - Fall 2020
 * partner: chenlei2, yunzhez2, minhaoj2
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
    uint32_t w_bytes = htonl(size);
    return write_all_to_socket(socket, (char*)&w_bytes, MESSAGE_SIZE_DIGITS);
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    size_t r_bytes = 0;
    ssize_t res;
    while(r_bytes < count && (res = read(socket, buffer + r_bytes, count - r_bytes))) {
        if(res == -1 && EINTR == errno) {
            continue;
        }
        if(res == -1) {
            return -1;
        }
        r_bytes += res;
    }
    return r_bytes;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    size_t w_bytes = 0;
    ssize_t res;
    while(w_bytes < count && (res = write(socket, buffer + w_bytes, count - w_bytes))) {
        if(res == -1 && EINTR == errno) {
            continue;
        }
        if(res == -1) {
            return -1;
        }
        w_bytes += res;
    }
    return w_bytes;
}
