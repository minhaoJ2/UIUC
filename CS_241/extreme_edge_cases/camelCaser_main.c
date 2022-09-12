/**
 * extreme_edge_cases
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <stdlib.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

int main() {
    // Feel free to add more test cases of your own!
    // char* str = "a\nb.";
    // char** output = camel_caser(str);
    // char** temp = output;
    // while (*temp) {
    //     printf("Result is: %s\n", *temp);
    //     temp++;
    // }

    if (test_camelCaser(&camel_caser, &destroy)) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }
    return 0;
}
