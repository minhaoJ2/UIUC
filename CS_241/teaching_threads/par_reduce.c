/**
 * chenlei2, minhaoj2
 * teaching_threads
 * CS 241 - Fall 2020
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct task {
    int *list;
    size_t length;
    reducer reduce_func;
    int base_case;
} mytask;


/* You should create a start routine for your threads. */
void *start_routine(void * input) {
    mytask *cur = input;
    int* res = (int*)calloc(1, sizeof(int));
    *res = reduce(cur->list, cur->length, cur->reduce_func, cur->base_case);
    return (void*)res;
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */

    /* edge case 1 */
    if(num_threads > list_len) {
        num_threads = list_len;
    }

    int loop_len = list_len/num_threads;
    int remain = list_len - num_threads * loop_len;
    /* edge case 2 */
    if(loop_len <= 1) {
        return reduce(list, list_len, reduce_func, base_case);
    }
    mytask *tasks = (mytask*)calloc(num_threads, sizeof(mytask));
    pthread_t tids[num_threads];
    for(size_t i = 0; i < num_threads; i++) {
        tasks[i].list = list + loop_len * i;
        tasks[i].reduce_func = reduce_func;
        tasks[i].base_case = base_case;
        if(i == num_threads - 1) {
            tasks[i].length = loop_len + remain;
        } else {
            tasks[i].length = loop_len;
        }
        pthread_create(tids+i, NULL, start_routine, tasks+i);
    }

    int out[num_threads];
    for(size_t i = 0; i < num_threads; i++) {
        void* temp = NULL;
        pthread_join(*(tids+i), &temp);
        out[i] = *(int*)(temp);
        free(temp);
    }

    int res = reduce(out, num_threads, reduce_func, base_case);
    free(tasks);
    return res;
}
