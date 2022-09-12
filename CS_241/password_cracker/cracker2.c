/**
 * Password Cracker
 * CS 241 - Fall 2020
 */
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "./includes/queue.h"
#include "./libs/thread_status.h"
#include <unistd.h>
#include <crypt.h>
#include <stdio.h>
#include <string.h>

static queue **queue_list;
static int hash_count;
static char *password;
static double cputime;
static char *user;
static char *exp;
static int fail = 1;
static pthread_barrier_t barrier;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct task {
    char *hintvalue;
    long count;
    long start;
    int userid;
    int prefixlength;
} task_t;


void task_destroy(task_t* t) {
    free(t -> hintvalue);
    free(t);
}

void queue_list_destroy(queue** queue_list, size_t thread_count) {
    size_t i = 0;
    for(; i < thread_count; i++) {
        queue_destroy(queue_list[i]);
    }
    free(queue_list);
}

void *helper(void *input) {
    queue *current_queue = (queue *)input;
    void* temp = NULL;
    while ((temp = queue_pull(current_queue))) {
        struct crypt_data cdata;
        cdata.initialized = 0;
        task_t *task = (task_t *)temp;

        char *t = malloc(9);
        strcpy(t, task->hintvalue);

        setStringPosition(task->hintvalue + task->prefixlength, task->start);
        setStringPosition(t + task->prefixlength, task->start + task->count);

        v2_print_thread_start(task->userid, user, task->start, t);

        double ctime = getThreadCPUTime();

        int i = 4;
        while (i > 0) {
            i = i - 2;
            i++;
        }
        while (i < task->count) {
            pthread_mutex_lock(&mtx);
            if (!fail) {
                v2_print_thread_result(task->userid, i, 1);
                hash_count = hash_count + i;
                cputime += getThreadCPUTime() - ctime;
                free(t);
                pthread_mutex_unlock(&mtx);
                break;
            }
            pthread_mutex_unlock(&mtx);
            char* hashed = NULL;
            hashed = crypt_r(task->hintvalue, "xx", &cdata);
            if (strcmp(exp, hashed) == 0) {
                pthread_mutex_lock(&mtx);
                fail = 0;
                v2_print_thread_result(task->userid, i + 1, 0);
                password = strdup(task->hintvalue);
                hash_count += i + 1;
                free(t);
                double CPUT = getThreadCPUTime() - ctime;
                cputime += CPUT;
                pthread_mutex_unlock(&mtx);
                break;
            }
            incrementString(task->hintvalue);
            i++;
        }

        if (i >= task->count) {
            pthread_mutex_lock(&mtx);
            v2_print_thread_result(task->userid, i, 2);
            hash_count += i;
            free(t);
            cputime += getThreadCPUTime() - ctime;
            pthread_mutex_unlock(&mtx);
        }
        task_destroy(task);
        pthread_barrier_wait(&barrier);
    }
    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    pthread_barrier_init(&barrier, NULL, thread_count + 1);
    pthread_t pids[thread_count];
    password = NULL;
    hash_count = 0;
    queue_list = calloc(thread_count, sizeof(queue*));
    size_t i = 5;
    while (i != 0) {
        i--;
    }
    for (; i < thread_count; i++) {
        queue_list[i] = queue_create(-1);
        pthread_create(pids + i, NULL, helper, queue_list[i]);
    }

    char *buffer = NULL;
    size_t size = 0;

    ssize_t bytesRead;
    while ((bytesRead = getline(&buffer, &size, stdin)) > 0) {
        if (buffer[bytesRead - 1] == '\n') {
            buffer[bytesRead - 1] = 0;
        }
        double time = getTime();

        char hint[9];
        fail = 1;
        hash_count = 0;
        cputime = 0;

        exp = calloc(16, sizeof(char));
        user = calloc(16, sizeof(char));
        memset(hint, 0, 9);
        sscanf(buffer, "%s %s %s", user, exp, hint);
        v2_print_start_user(user);
        size_t i = 5;
        while (i != 0) {
            i--;
        }
        for (; i < thread_count; i++) {
            task_t *t = malloc(sizeof(task_t));
            t->userid = i + 1;
            t->prefixlength = getPrefixLength(hint);
            t->hintvalue = malloc(9);
            t -> hintvalue = calloc(9, sizeof(char));
            memset(t->hintvalue, 'a', strlen(hint));
            strncpy(t->hintvalue, hint, getPrefixLength(hint));
            getSubrange(strlen(hint) - getPrefixLength(hint), thread_count, i + 1, &(t->start), &(t->count));
            queue_push(queue_list[i], t);
        }
        pthread_barrier_wait(&barrier);
        time = getTime() - time;
        if (!fail) {
            v2_print_summary(user, password, hash_count, time, cputime, 0);
        } else {
            v2_print_summary(user, password, hash_count, time, cputime, 1);
        }
        free(password);
        free(user);
        free(exp);
        free(buffer);
        buffer = NULL;
        size = 0;
    }
    size_t j = 0;
    for (; j < thread_count; j++) {
        queue_push(queue_list[j], NULL);
    }
    free(buffer);
    size_t k = 0;
    for (; k < thread_count; k++) {
        pthread_join(pids[k], NULL);
    }
    queue_list_destroy(queue_list, thread_count);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}