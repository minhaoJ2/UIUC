/**
 * Vector Lab
 * CS 241 - Fall 2020
 */

#include "sstring.h"
#include <string.h>

int main() {
    // TODO create some tests
    sstring* test1 = cstr_to_sstring("Hello World");
    char* cstr1 = sstring_to_cstr(test1);
    printf("Transition function: %s\n", cstr1);
    sstring_destroy(test1);
    free(cstr1);

    sstring* test2 = cstr_to_sstring("My name is Michael.");
    char* slice2 = sstring_slice(test2, 5, 9);
    printf("Slice result is: %s\n", slice2);
    sstring_destroy(test2);

    sstring* test3 = cstr_to_sstring("My name is Michael.");
    vector* split3 = sstring_split(test3, ' ');
    size_t i = 0;
    for (; i < vector_size(split3); i++) {
        printf("Split result is: %s\n", (char*) vector_get(split3, i));
    }

    sstring *replace_me = cstr_to_sstring("This is a {} day, {  }");
    sstring_substitute(replace_me, 15, "}", "friend");
    char* res1 = sstring_to_cstr(replace_me); // == "This is a {} day, friend  }!"
    printf("Current string is: %s\n", res1);
    sstring_substitute(replace_me, 0, "Ths", "good");
    char* res2 = sstring_to_cstr(replace_me); // == "This is a {good day, friend!"
    printf("Current string is: %s\n", res2);
    sstring_substitute(replace_me, 0, "is", "a");
    char* res3 = sstring_to_cstr(replace_me);
    printf("Current string is: %s\n", res3);
    free(res1);
    free(res2);
    free(res3);


    sstring_destroy(test3);
    sstring_destroy(replace_me);
    vector_destroy(split3);
    return 0;
}