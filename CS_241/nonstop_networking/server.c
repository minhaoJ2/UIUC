/**
 * Nonstop Networking
 * CS 241 - Fall 2020
 */
#include "common.h"
#include "format.h"
#include "./includes/dictionary.h"
#include "./includes/vector.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 100
#define MAX_EVENTS 100
#define HEADER_SIZE 1024
#define FILENAME_SIZE 255
typedef struct clients {
	int state;
	verb request;
	char filename[FILENAME_SIZE];
	char header[HEADER_SIZE];
	size_t idx_f;
} clients;

void run_server(char *port);
void process_client(int client_fd);
void clean_client(int client_fd);
void err_handler(int client_fd, clients *info);
void process_request(int client_fd, clients *info);
int read_body(int client_fd, clients *info);
void epollout_client(int client_fd);
void read_header(int client_fd, clients *info);
void close_server();
void handle_sigpipe();
static int epoll_fd;
static dictionary  *clients_dict;
static char *dir_name;
static vector *file_list;
static dictionary  *file_size;

int main(int argc, char **argv) {
	// good luck!
	signal(SIGPIPE, handle_sigpipe);
	if (argc != 2) {
			print_server_usage();
			exit(1);
	}
	struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_handler = close_server;
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	clients_dict = int_to_shallow_dictionary_create();
	char template[] = "XXXXXX";
    dir_name = mkdtemp(template);
    print_temp_directory(dir_name);
	file_list = string_vector_create();
	file_size = string_to_unsigned_long_dictionary_create();
	run_server(argv[1]);
}
void handle_sigpipe() {
	char mesg[1000];
    sprintf(mesg, "\n****** SIGPIPE  - no one is listening :-( ******\n");
    write(1, mesg, strlen(mesg));
}
void run_server(char *port) {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(1);
	}
	int optval = 1;
	if ( setsockopt(serverSocket, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval))) {
		perror("setsocketopt");
		exit(1);
	}
	if ( bind(serverSocket, result->ai_addr, result->ai_addrlen) != 0 ) {
		perror("bind");
		exit(1);
	}
	if ( listen(serverSocket, MAX_CLIENTS) != 0 ) {
		perror("listen");
		exit(1);
	}
	freeaddrinfo(result);
	epoll_fd = epoll_create(42);
	if (epoll_fd == -1) {
        perror("epoll_create1()");
        exit(1);
  	}
	struct epoll_event ev = {.events = EPOLLIN, .data.fd = serverSocket};
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &ev);
	struct epoll_event array[MAX_EVENTS];
	while (1) {
		int num_events = epoll_wait(epoll_fd, array, MAX_EVENTS, 10000);
		if (num_events == -1) {
			exit(1);
		}
		if (num_events == 0) continue;
		for (int i = 0; i < num_events; i++) {
			if (array[i].data.fd == serverSocket) {
				int client_fd = accept(serverSocket, NULL, NULL);
				if (client_fd < 0) {
						perror("accept");
						exit(1);
				}
				struct epoll_event ev_c = {.events = EPOLLIN, .data.fd = client_fd};
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_c);
				clients *info = calloc(1, sizeof(clients));
                dictionary_set(clients_dict, &client_fd, info);
			} else {
				process_client(array[i].data.fd);
			}
		}
	}
}
void process_client(int client_fd) {
	clients *info = dictionary_get(clients_dict, &client_fd);
	if (info -> state == 0) {
		read_header(client_fd, info);
	} else if (info -> state == 1) {
		process_request(client_fd, info);
	} else {
		write_to_socket(client_fd, "ERROR\n", 6);
		if (info->state == -1) {
			write_to_socket(client_fd, (char*) err_bad_request, strlen(err_bad_request));
		} else if (info->state == -2) {
			write_to_socket(client_fd, (char*) err_bad_file_size, strlen(err_bad_file_size));
		} else if (info->state == -3) {
			write_to_socket(client_fd, (char*) err_no_such_file, strlen(err_no_such_file));
		}
		clean_client(client_fd);
	}
}
void clean_client(int client_fd) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	free(dictionary_get(clients_dict, &client_fd));
	dictionary_remove(clients_dict, &client_fd);
	shutdown(client_fd, SHUT_RDWR);
	close(client_fd);
}
void process_request(int client_fd, clients *info) {
	if (info -> request == GET) {
		int len = strlen(dir_name) + 1 + strlen(info -> filename) + 1;
		char path[len];
		memset(path,0, len);
		sprintf(path, "%s/%s", dir_name, info->filename);
		FILE *remote = fopen(path, "r");
		if (!remote) {
			info->state = -3;
			return;
		}
		write_to_socket(client_fd, "OK\n", 3);
		size_t size;
		size = *(size_t *) dictionary_get(file_size, info->filename);
		write_to_socket(client_fd, (char*)&size, sizeof(size_t));
		size_t bytes_write = 0;
        while (bytes_write < size) {
            size_t size_hd = (size-bytes_write) > 1024 ? 1024 : (size-bytes_write);
            char buffer[size_hd+1];
            fread(buffer, 1, size_hd, remote);
            write_to_socket(client_fd, buffer, size_hd);
            bytes_write += size_hd;
        }
		fclose(remote);
	} else if (info -> request == PUT) {
		write_to_socket(client_fd, "OK\n", 3);
	} else if (info -> request == DELETE) {
		char path[strlen(dir_name) + 1 + strlen(info->filename) + 1];
		memset(path, 0, strlen(dir_name) + 1 + strlen(info->filename) + 1);
		sprintf(path, "%s/%s", dir_name, info -> filename);
		int res = remove(path);
		if (res < 0) {
			info -> state = -3;
			return;
		}
		//search for the file on server.
		size_t idx = 0;
		VECTOR_FOR_EACH(file_list, name, {
	        if (!strcmp((char *) name, info -> filename)){
	           break;
	        }
			idx++;
	    });
		//no found.
		if (idx == vector_size(file_list)) {
			info -> state = -3;
			return;
		}
		vector_erase(file_list, idx);
		dictionary_remove(file_size, info->filename);
		write_to_socket(client_fd, "OK\n", 3);
	} else if (info -> request == LIST) {
		write_to_socket(client_fd, "OK\n", 3);
		size_t size = 0;
		VECTOR_FOR_EACH(file_list, name, {
	        size += strlen(name) + 1;
	    });
		if (size) size--;
		write_to_socket(client_fd, (char*)&size, sizeof(size_t));
		VECTOR_FOR_EACH(file_list, name, {
		    write_to_socket(client_fd, name, strlen(name));
			if (_it != _iend-1) {
				write_to_socket(client_fd, "\n", 1);
			}
		});
	}
	clean_client(client_fd);
}
int read_body(int client_fd, clients *info) {
	char path[strlen(dir_name) + 1 + strlen(info->filename) + 1];
	memset(path,0, strlen(dir_name) + 1 + strlen(info->filename) + 1);
	sprintf(path, "%s/%s", dir_name, info->filename);
	FILE *overwrite_f = fopen(path, "r");
	FILE *remote = fopen(path, "w");
	if (remote == NULL) {
		perror("Open file failed");
		return 1;
	}
	size_t size;
	read_from_socket(client_fd, (char *)&size, sizeof(size_t));
	size_t bytes_read = 0;
	while (bytes_read < size + 5) {
		size_t size_hd = (size + 5 - bytes_read) > 1024 ? 1024 : (size + 5 - bytes_read);
		char buffer_f[1024 + 1] = {0};
		ssize_t rc = read_from_socket(client_fd, buffer_f, size_hd);
		if (rc == -1) {
			continue;
		}
		fwrite(buffer_f, 1, rc, remote);
		bytes_read += rc;
		if (rc == 0) {
			break;
		}
	}
	fclose(remote);
	if (bytes_read == 0 && bytes_read != size) {
        print_connection_closed();
        remove(dir_name);
        return 1;
    } else if (bytes_read < size) {
        print_too_little_data();
        remove(dir_name);
        return 1;
    } else if (bytes_read > size) {
        print_received_too_much_data();
        remove(dir_name);
        return 1;
    }
	if (overwrite_f == NULL) {
		vector_push_back(file_list, info -> filename);
	} else {
		fclose(overwrite_f);
	}
	dictionary_set(file_size, info->filename, &size);
	return 0;
}
void epollout_client(int client_fd) {
	struct epoll_event ev_c = {.events = EPOLLOUT, .data.fd = client_fd};
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev_c);
}
void read_header(int client_fd, clients *info) {
	size_t rc = read_from_until_newline(client_fd, info->header, HEADER_SIZE);
	if (rc == HEADER_SIZE) {
		info->state = -1;
		epollout_client(client_fd);
		return;
	}
	if (strncmp(info->header, "GET", 3) == 0) {
		info -> request = GET;
		strcpy(info -> filename, info -> header+4);
		info -> filename[strlen(info -> filename) - 1] = '\0';
	} else if (strncmp(info -> header, "PUT", 3) == 0) {
		info -> request = PUT;
		strcpy(info -> filename, info -> header + 4);
		info -> filename[strlen(info -> filename) - 1] = '\0';
		if (read_body(client_fd, info)) {
			info -> state = -2;
			epollout_client(client_fd);
			return;
		}
	} else if (strncmp(info -> header, "DELETE", 6) == 0) {
		info -> request = DELETE;
		strcpy(info -> filename, info -> header + 7);
		info -> filename[strlen(info->filename) - 1] = '\0';
	} else if (strncmp(info -> header, "LIST", 4) == 0) {
		info -> request = LIST;
	} else {
		print_invalid_response();
		write_to_socket(client_fd, "Bad request\n", 12);
		info -> state = -1;
		epollout_client(client_fd);
		return;
	}
	info -> state = 1;
	epollout_client(client_fd);
}
void close_server() {
	close(epoll_fd);
	vector *infos = dictionary_values(clients_dict);
	VECTOR_FOR_EACH(infos, info, {
         free(info);
       });
	vector_destroy(infos);
	dictionary_destroy(clients_dict);
	remove(dir_name);
	vector_destroy(file_list);
	dictionary_destroy(file_size);
	exit(1);
}