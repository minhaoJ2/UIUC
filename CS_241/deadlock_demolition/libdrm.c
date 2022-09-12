/**partner netids: rni4, peiyic2, qiruilu2, minhaoj2 */
/**
 * deadlock_demolition
 * CS 241 - Fall 2020
 *
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
graph* g;
struct drm_t {
    pthread_mutex_t pm;
};
int isCyclic(graph *G, set* is_visited, void* node) {

    if(set_contains(is_visited, node)) {
        return 1;
    }
    set_add(is_visited, node);
    vector* neighbors = graph_neighbors(G, node);
    for (size_t i = 0; i < vector_size(neighbors); i++) {
        if(isCyclic(G, is_visited, vector_get(neighbors,i))) {
            return 1;
        }
    }
    is_visited = shallow_set_create();
    return 0;
}

drm_t *drm_init() {
    drm_t* ret = malloc(sizeof(drm_t));
    pthread_mutex_init(&ret->pm, NULL);
    pthread_mutex_lock(&m);
    if (!g) {
        g = shallow_graph_create();
    }
    graph_add_vertex(g, ret);
    pthread_mutex_unlock(&m);
    return ret;
}


int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    //check if the graph contains the thread.
    pthread_mutex_lock(&m);
    if(!(graph_contains_vertex(g, thread_id) && graph_contains_vertex(g, drm))) {
        pthread_mutex_unlock(&m);
        return 0;
    }
    //check edge exit
    else {
    if(graph_adjacent(g,drm,thread_id)) {
        graph_remove_edge(g,drm,thread_id);
        pthread_mutex_unlock(&(drm->pm));
    }
    pthread_mutex_unlock(&m);
        return 1;
    }
    pthread_mutex_unlock(&m);
    return 0;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);
    if(!graph_contains_vertex(g, thread_id)) {
        graph_add_vertex(g,thread_id);
    }
    if(!graph_adjacent(g,drm,thread_id)) {

        graph_add_edge(g, thread_id,drm);
        set* is_visited_temp = shallow_set_create();
        if(isCyclic(g,is_visited_temp,drm)) {
            graph_remove_edge(g, thread_id, drm);
            pthread_mutex_unlock(&m);
            return 0;
        } else {
            pthread_mutex_unlock(&m);
            pthread_mutex_lock(&drm->pm);
            pthread_mutex_lock(&m);
            graph_remove_edge(g,thread_id,drm);
            graph_add_edge(g, drm, thread_id);
            pthread_mutex_unlock(&m);
            return 1;
        }
    }
    pthread_mutex_unlock(&m);
    return 0;
}

void drm_destroy(drm_t *drm) {
    /* Your code here */
    graph_remove_vertex(g, drm);
    pthread_mutex_destroy(&drm->pm);
    free(drm);
    return;
}
