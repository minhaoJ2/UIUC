/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

ssize_t read_from_socket(int sock_fd, char* buffer, size_t count) {
    size_t total = 0;
    while (total < count) {
      ssize_t res = read(sock_fd, buffer + total, count - total);
      if (res == 0) {
        break;
      }
      if (res == -1 && errno == EINTR) {
        continue;
      }
      if (res == -1) {
        perror("Read from socket failed\n");
        return -1;
      }
      total += res;
    }
    return total;
}

ssize_t write_to_socket(int sock_fd, char* buffer, size_t count) {
    size_t total = 0;
    while (total < count) {
      ssize_t res = write(sock_fd, buffer + total, count - total);
      if (res == 0) {
        break;
      }
      if (res == -1 && errno == EINTR) {
        continue;
      }
      if (res == -1) {
        fprintf(stderr, "Write to socket failed\n");
        return -1;
      }
      total += res;
    }
    return total;
}

size_t read_from_until_newline(int sock_fd, char* buffer, size_t count) {
    size_t bytes_recd = 0;
	  while (bytes_recd < count) {
		  ssize_t ret = read(sock_fd, buffer + bytes_recd, 1);
		  if (ret == 0 || buffer[strlen(buffer)-1] == '\n') {
			  break;
		  }
		  if (ret == -1 && errno == EINTR) {
			  continue;
		  }
		  if (ret == -1) {
			  perror("Read from socket until newline failed ()");
		  }
		  bytes_recd += ret;
	  }
	  return bytes_recd;
}