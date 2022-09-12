/**
 * password_cracker
 * CS 241 - Fall 2020
 */
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

static queue *q;
int numSuccessful = 0;
int numFailed = 0;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void *myfunc(void *index) {
    struct crypt_data data;
    data.initialized = 0;
    int threadId = *(int*) index;
    while (1) {
        char* input = (char*) queue_pull(q);
        if (input == NULL) {
            break;
        }
        char username[16];
        char password_hash[16];
        char password_key[9];
        memset(password_key, 0, 9);
        sscanf(input, "%s %s %s", username, password_hash, password_key);
        v1_print_thread_start(threadId, username);
        double start_time = getThreadCPUTime();
        int hashCount = 0;
        int fail = 1;
        char* temp = calloc(9, sizeof(char));
        strcpy(temp, password_key);
        setStringPosition(temp + getPrefixLength(temp), 0);
        char* hashed = NULL;
        while (1) {
            hashed = crypt_r(temp, "xx", &data);
            hashCount++;
            if (strcmp(hashed, password_hash) == 0) {
                double current_time = getThreadCPUTime();
                v1_print_thread_result(threadId, username, temp, hashCount, current_time - start_time, 0);
                fail = 0;
                pthread_mutex_lock(&m1);
                numSuccessful++;
                pthread_mutex_unlock(&m1);
                break;
            }
            incrementString(temp);
            if (strncmp(temp, password_key, getPrefixLength(password_key)) != 0) {
                break;
            }
        }
        if (fail) {
            double current_time = getThreadCPUTime();
            v1_print_thread_result(threadId, username, temp, hashCount, current_time - start_time, 1);
            pthread_mutex_lock(&m2);
            numFailed++;
            pthread_mutex_unlock(&m2);
        }
        free(input);
        free(temp);
    }
    queue_push(q, NULL);
    return NULL;
}
int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    q = queue_create(-1);
    pthread_t pids[thread_count];
    int index[thread_count];
    char* buffer = NULL;
    size_t size = 0;
    ssize_t bytesRead;
    while (1) {
        bytesRead = getline(&buffer, &size, stdin);
        if (bytesRead == -1) {
            break;
        }
        char* temp = calloc(41, sizeof(char));
        strcpy(temp, buffer);
        if (bytesRead > 0 && temp[bytesRead - 1] == '\n') {
            temp[bytesRead - 1] = '\0';
        }
        queue_push(q, temp);
    }
    queue_push(q, NULL);
    free(buffer);
    size_t i = 0;
    for (; i < thread_count; i++) {
        index[i] = i + 1;
        pthread_create(pids + i, NULL, myfunc, index + i);
    }
    size_t j = 0;
    for (; j < thread_count; j++) {
        pthread_join(pids[j], NULL);
    }
    queue_destroy(q);
    v1_print_summary(numSuccessful, numFailed);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&m2);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
