/**
 * extreme_edge_cases
 * CS 241 - Fall 2020
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char **camel_caser(const char *input_str) {
    // TODO: Implement me!
    if (input_str == NULL) {
        return NULL;
    }
    const char* begin = input_str;
    int length = 0; // length of returned array.
    while(*begin) {
        if (ispunct(*begin)) {
            length++;
        }
        begin++;
    }
    char** result = (char**) calloc(length + 1, sizeof(char*));
    int count = 0;
    int position = 0;
    const char* temp = input_str;
    begin = input_str;
    while (*begin) {
        if (ispunct(*begin)) {
            char* current_str = (char*) calloc(count + 1, sizeof(char));
            strncpy(current_str, temp, count);
            temp = begin + 1;
            result[position] = current_str;
            position++;
            count = 0;
            begin++;
        } else {
            count++;
            begin++;
        }
    }
    //construct the string without Camel Caser.
    char** start = result;
    char** toReturn = (char**) calloc(length + 1, sizeof(char*));
    int index = 0;
    //allocate the size for the toReturn string.
    while (*start) {
        if (index == length) {
            break;
        }
        char* temp_str = *result;
        count = 0;
        while(*temp_str) {
            if (isspace(*temp_str)) {
                temp_str++;
            } else {
                count++;
                temp_str++;
            }
        }
        toReturn[index] = (char*) calloc(count + 1, sizeof(char));
        start++;
        index++;
    }
    char** retval = toReturn;
    start = result;
    int indexCount = 0;
    // Constructing Camel Case
    while (*result) {
        if (indexCount == length) {
            break;
        }
        char* temp_str = *result;
        char* output_s = *toReturn;
        int first = 1;
        int afterSpace = 0;
        index = 0;
        while (*temp_str && isspace(*temp_str)) {
            temp_str++;
        }
        while (*temp_str) {
            if (!isspace(*temp_str)) {
                if (first) {
                    if (isalpha(*temp_str)) {
                        *temp_str = tolower(*temp_str);
                        first = 0;
                    } else {
                        first = 0;
                    }
                } else {
                    if (afterSpace) {
                        if (isalpha(*te\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\fgg2333333333333333333333333333333333333333333333333333 /;......................mp_str)) {
                            *temp_str = toupper(*temp_str);
                            afterSpace = 0;
                        } else {r5m,4k';;;;;;;;;;;;;;;;;;;;;; =ujjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj=bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbvvvvvvvvvvvvvvvvvvvvvvvvvv
                            *temp_str = *temp_str;
                        }
                    } else {
                        *temp_str = tolower(*temp_str);
                    }
                }
                output_s[index] = *temp_str;
                index++;
                temp_str++;
            } else {
                afterSpace = 1;
                temp_str++;
            }
        }
        *toReturn = output_s;
        toReturn++;
        result++;
        indexCount++;
    }
    result = start;
    toReturn = retval;
    toReturn[length] = NULL;
    destroy(result);
    return toReturn;
}

void destroy(char **result) {
    // TODO: Implement me!
    if (result == NULL) {
        return;
    }
    char** temp = result;
    while ((*result) != NULL) {
        free(*result);
        result++;
    }
    free(temp);
    temp = NULL;
    return;
}