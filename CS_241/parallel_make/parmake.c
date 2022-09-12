/**
 * Parallel Make
 * CS 241 - Fall 2020
 */

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"
#include "set.h"
#include "dictionary.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

static set* visited = NULL;
static graph* g = NULL;
static queue* task_queue = NULL;

//determine if there is a cycle
bool isCyclic(void* file) {
    if (visited == NULL) {
        visited = shallow_set_create();
    }
    if (set_contains(visited, file)) {
        return true;
    }
    set_add(visited, file);
    vector* neighbors = graph_neighbors(g, file);
    if (vector_size(neighbors) == 0) {
        set_remove(visited, file);
        vector_destroy(neighbors);
        return false;
    } else {
        size_t i = 0;
        for (; i < vector_size(neighbors); i++) {
            if (isCyclic(vector_get(neighbors, i))) {
                vector_destroy(neighbors);
                return true;
            }
        }
    }
    set_remove(visited, file);
    vector_destroy(neighbors);
    return false;
}

//worker function
void* my_func(void* index) {
    rule_t* current_node = NULL;
    while ((current_node = queue_pull(task_queue))) {
        int result = 0;
        size_t total_command = vector_size(current_node -> commands);
        size_t i = 0;
        for (; i < total_command; i++) {
            if (system((char*) vector_get(current_node -> commands, i)) != 0) {
                result = 1;
                break;
            }
        }
        if (result == 1) {
            pthread_mutex_lock(&m1);
            current_node -> state = -1;
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&m1);
        } else {
            pthread_mutex_lock(&m1);
            current_node -> state = 1;
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&m1);
        }
    }
    return NULL;
}

void check_failed(rule_t* rule) {
    struct stat s1;
    struct stat s2;
    pthread_mutex_lock(&m2);
    if (set_contains(visited, rule)) {
        pthread_mutex_unlock(&m2);
        return;
    } else {
        set_add(visited, rule);
    }
    pthread_mutex_unlock(&m2);

    vector* neighbors = graph_neighbors(g, rule -> target);
    size_t i = 0;
    for (; i < vector_size(neighbors); i++) {
        check_failed((rule_t*) graph_get_vertex_value(g, vector_get(neighbors, i)));
    }
    i = 0;
    for (; i < vector_size(neighbors); i++) {
        pthread_mutex_lock(&m1);
        while (((rule_t *)graph_get_vertex_value(g, vector_get(neighbors, i))) ->state == 0) {
            pthread_cond_wait(&cv, &m1);
        }
        pthread_mutex_unlock(&m1);
    }
    i = 0;
    for (; i < vector_size(neighbors); i++) {
        pthread_mutex_lock(&m1);
        if (((rule_t *)graph_get_vertex_value(g, vector_get(neighbors, i))) ->state == -1) {
            rule -> state = -1;
            pthread_cond_broadcast(&cv);
            pthread_mutex_unlock(&m1);
            vector_destroy(neighbors);
            return;
        }
        pthread_mutex_unlock(&m1);
    }
    int file_on_disk = access(rule -> target, F_OK);
    if (file_on_disk != -1) {
        size_t j = 0;
        for (; j < vector_size(neighbors); j++) {
            pthread_mutex_lock(&m1);
            rule_t* temp = (rule_t*) graph_get_vertex_value(g, vector_get(neighbors, j));
            pthread_mutex_unlock(&m1);
            if (access(temp -> target, F_OK) != 0) {
                queue_push(task_queue, temp);
                vector_destroy(neighbors);
                return;
            }

        }
    } else {
        queue_push(task_queue, rule);
        vector_destroy(neighbors);
        return;
    }
    stat(rule -> target, &s1);
    i = 0;
    for (; i < vector_size(neighbors); i++) {
        stat(vector_get(neighbors, i), &s2);
        if (difftime(s1.st_mtime, s2.st_mtime) < 0) {
            queue_push(task_queue, rule);
            vector_destroy(neighbors);
            return;
        }
    }
    pthread_mutex_lock(&m1);
    rule -> state = 1;
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&m1);
    vector_destroy(neighbors);
    return;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    g = parser_parse_makefile(makefile, targets);
    if (visited == NULL) {
        visited = shallow_set_create();
    }
    if (task_queue == NULL) {
        task_queue = queue_create(-1);
    }

    vector *neighbors = graph_neighbors(g, "");
    pthread_t pids[num_threads];
    size_t i = 0;
    for (; i < num_threads; i ++) {
        pthread_create(pids + i, NULL, my_func, (void*)i);
    }
    i = 0;
    size_t goal_size = vector_size(neighbors);
    for (; i < goal_size; i++) {
        void* temp = vector_get(neighbors, i);
        if (isCyclic(temp) == 0) {
            //no cycle detected
            check_failed((rule_t *)graph_get_vertex_value(g, temp));
        } else {
            print_cycle_failure(temp);
        }
        set_destroy(visited);
        visited = NULL;
    }
    vector_destroy(neighbors);
    i = 0;
    for (; i < num_threads; i++) {
        queue_push(task_queue, NULL);
    }
    i = 0;
    for (; i < num_threads; i++) {
        pthread_join(pids[i], NULL);
    }
    graph_destroy(g);
    set_destroy(visited);
    queue_destroy(task_queue);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&m2);
    pthread_cond_destroy(&cv);
    return 0;
}
