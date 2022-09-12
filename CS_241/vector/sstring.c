/**
 * vector
 * CS 241 - Fall 2020
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    // Anything you want
    vector* v;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring* str = calloc(1, sizeof(sstring));
    str -> v = char_vector_create();
    const char* temp = input;
    while (*temp) {
        vector_push_back(str -> v, (void* )temp);
        temp++;
    }
    return str;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    char* toReturn = calloc(vector_size(input -> v) + 1, sizeof(char));
    size_t i = 0;
    for (; i < vector_size(input -> v); i++) {
        toReturn[i] = *(char*) vector_get(input -> v, i);
    }
    return toReturn;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    size_t i = 0;
    while(i < vector_size(addition -> v)) {
        vector_push_back(this -> v, vector_get(addition -> v,i));
        i++;
    }
    return vector_size(this -> v);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    vector* toReturn = string_vector_create();
    sstring* tempString = calloc(1, sizeof(sstring));
    tempString -> v = char_vector_create();
    size_t i = 0;
    for (; i < vector_size(this -> v); i++) {
        char* currentChar = (char*) vector_get(this -> v, i);
        if (*currentChar != delimiter) {
            vector_push_back(tempString ->v, currentChar);
        } else {//delimiter
            vector_push_back(toReturn, sstring_to_cstr(tempString));
            vector_clear(tempString -> v);
        }
    }
    vector_push_back(toReturn, sstring_to_cstr(tempString));
    sstring_destroy(tempString);
    return toReturn;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    size_t vectorSize = vector_size(this -> v);
    if (offset > vectorSize) {
        return -1;
    }
    char* temp = sstring_to_cstr(this);
    if (strstr(temp + offset, target) == NULL) {
        free(temp);
        return -1;
    } else {
        char* position = strstr(temp + offset, target);
        size_t i = 0;
        for (; i < strlen(target); i++) {
            vector_erase(this -> v, position - temp);
        }
        int j = (int)strlen(substitution) - 1;
        for (; j >= 0; j--) {
            vector_insert(this -> v, position - temp, substitution + j);
        }
        free(temp);
        return 0;
    }
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    if (start >= end || start < 0 || end > (int)vector_size(this -> v)) {
        return NULL;
    }
    sstring * tempString = malloc(sizeof(sstring));
    tempString -> v = char_vector_create();
    int i = start;
    for (; i < end; i++) {
        vector_push_back(tempString -> v, vector_get(this -> v, i));
    }
    char* result = sstring_to_cstr(tempString);
    sstring_destroy(tempString);
    return result;
}

void sstring_destroy(sstring *this) {
    // your code goes here
    vector_destroy(this -> v);
    free(this);
    return;
}
