/**
 * mini_memcheck
 * CS 241 - Fall 2020
 */
#include <stdlib.h>
int main() {
    void *p1 = malloc(30);
    void *p2 = malloc(40);
    void *p3 = malloc(50);
    free(p2);
    return 0;
}