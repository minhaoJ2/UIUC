/**
 * critical_concurrency
 * CS 241 - Fall 2020
 * lab partner : jingz15, hangel2, minhaoj2
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    queue *res = malloc(sizeof(struct queue));
    res->head = NULL;
    res->tail = NULL;
    res->size = 0;
    res->max_size = max_size;
    pthread_mutex_init(&res->m, NULL);
    pthread_cond_init(&res->cv, NULL);

    return res;
}

void queue_destroy(queue *this) {
    /* Your code here */
    queue_node *ptr = this->head;
    while (ptr) {
        queue_node *tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
        
    pthread_cond_destroy(&(this->cv));
    pthread_mutex_destroy(&(this->m));
    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    pthread_mutex_lock(&(this->m));
    // if max size is positive and the queue is full, wait
    while (this->max_size > 0 && this->size == this->max_size)
        pthread_cond_wait(&this->cv, &this->m);
    
    queue_node *newNode = malloc(sizeof(queue_node));
    newNode->data = data;
    newNode->next = NULL;
    
    if (this->head == NULL) { // if empty queue
        this->head = newNode;
    } else { // otherwise, append to the tail
        this->tail->next = newNode;
    }
    this->tail = newNode;
    this->size++;

    // broadcast and unlock
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
}

void *queue_pull(queue *this) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    while (this->size == 0)
        pthread_cond_wait(&this->cv, &this->m);

    // pull from the head
    queue_node *temp = this->head;
    this->head = temp->next;

    this->size--;
    if (this->size == 0) {
        this->tail = NULL;
    }

    // broadcast and unlock
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
    
    // get data and return
    void *data = temp->data;
    free(temp);
    return data;
}
