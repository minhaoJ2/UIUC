/**
 * perilous_pointers
 * CS 241 - Fall 2020
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    first_step(81);

    int value1 = 132;
    second_step(&value1);

    int value2 = 8942;
    int* ptr = &value2;
    double_step(&ptr);

    char* value_4 = malloc(5 + sizeof(int));
    *(int*)(value_4 + 5) = 15;
    strange_step(value_4);
    free(value_4);

    char* five = "123";
    empty_step(five);

    char* six = "123u";
    char* s = six;
    two_step(s, six);

    char* seven1 = "123456";
    char* seven2 = seven1 + 2;
    char* seven3 = seven2 + 2;
    three_step(seven1, seven2, seven3);

    char* first = "12345678";
    char second[12];
    second[2] = first[1] + 8;
    char third[5];
    third[3] = second[2] + 8;
    step_step_step(first, second, third);

    char a = 1;
    int b = 1;
    it_may_be_odd(&a, b);

    char tok[] = "bla,CS241";
    tok_step(tok);

    char orange[4];
    for(int i = 0; i < 4; i++) {
        orange[i] = 0;
    }
    orange[0] = 1;
    orange[1] = 8;
    the_end(orange, orange);
    return 0;
}
