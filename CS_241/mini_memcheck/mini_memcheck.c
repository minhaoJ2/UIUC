/**
 * mini_memcheck
 * CS 241 - Fall 2020
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>


meta_data *head;
size_t total_memory_requested;
size_t total_memory_freed;
size_t invalid_addresses;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (request_size == 0) {
        return NULL;
    }
    meta_data* memory = malloc(sizeof(meta_data) + request_size);
    if (!memory) {
        return NULL;
    }
    memory -> request_size = request_size;
    memory -> filename = filename;
    memory -> instruction = instruction;
    total_memory_requested += memory ->request_size;

    if (!head) {
        memory -> next = NULL;
        head = memory;
    } else {
        memory -> next = head;
        head  = memory;
    }
    void* res = (void*) (memory + 1);
    return res;

}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    size_t request_size = num_elements * element_size;
    void* res = mini_malloc(request_size, filename, instruction);
    res = memset(res, 0, request_size);
    return res;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    if (payload == NULL) {
        if (request_size == 0) {
            return NULL;
        } else {
            return mini_malloc(request_size, filename, instruction);
        }
    } else if (request_size == 0) {
        mini_free(payload);
        return NULL;
    }
    meta_data* temp = NULL;
    meta_data* current = head;
    while ( current + sizeof(meta_data) != payload) {
        temp = current;
        current = current -> next;
    }
    if (current == NULL) {
        invalid_addresses = invalid_addresses + 1;
        return NULL;
    }
    meta_data* memory = (meta_data*) realloc(current, request_size + sizeof(meta_data));
    if (!memory) {
        return NULL;
    }
    memory -> filename = filename;
    memory -> instruction = instruction;
    memory -> request_size = request_size;
    if (current -> request_size < request_size) {
        total_memory_requested += (request_size - current -> request_size);
    } else {
        total_memory_freed +=  (current -> request_size - request_size);
    }
    if (memory != current) {
        temp -> next = memory;
        memory -> next = current -> next;
        free(current);
    }
    return memory + 1;
}

void mini_free(void *payload) {
    // your code here
    if (payload == NULL) {
        return;
    }
    meta_data* start = (meta_data*)payload - 1;
    if (head == start) {
        head = start -> next;
        size_t s = start -> request_size;
        free(start);
        total_memory_freed = total_memory_freed + s;
        return;
    } else {
        meta_data* current = head;
        meta_data* temp = NULL;
        while (current != start) {
            if (!current) {
                invalid_addresses++;
                return;
            } else {
                temp = current;
            }
            current = current -> next;
        }
        size_t s = current -> request_size;
        temp -> next = current -> next;
        free(current);
        total_memory_freed += s;
        return;
    }

}
