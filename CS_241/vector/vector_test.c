/**
 * vector
 * CS 241 - Fall 2020
 */
#include "vector.h"
#include <stdio.h>
int main() {
    // Write your test cases here
    vector* v = char_vector_create();
    char* test_str = "ello World";
    char* temp = test_str;
    vector_insert(v, 0, "H");
    while (*temp) {
        vector_push_back(v, temp);
        printf("Back is: %c\n", *(char*) *vector_back(v));
        vector_pop_back(v);
        printf("Back after pop is: %c\n", *(char*) *vector_back(v));
        vector_push_back(v, temp);
        temp++;
    }
    vector_set(v, 0, "A");
    printf("Current capacity is: %zu\n", vector_capacity(v));
    printf("Current size is: %zi\n", vector_size(v));
    vector_reserve(v, 5);
    printf("Current capacity after reserve is: %zu\n", vector_capacity(v));
    printf("Current size after reserve is: %zi\n", vector_size(v));
    vector_reserve(v, 33);
    vector_set(v, 1, "Char");
    printf("Current capacity after reserve is: %zu\n", vector_capacity(v));
    printf("Current size after reserve is: %zi\n", vector_size(v));
    vector_resize(v, 8);
    printf("Current size after resize is: %zi\n", vector_size(v));
    vector_resize(v, 9);
    vector_resize(v, 8);
    vector_destroy(v);
    return 0;
}
