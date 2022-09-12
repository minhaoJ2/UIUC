/**
 * Extreme Edge Cases
 * CS 241 - Spring 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

// return 0 if they are different; return 1 means they are same.
int checkSame(char** actual, char* expected[]){
	if (actual == NULL) {
		return 0;
	}
	if(expected == NULL){
        if (actual != NULL) {
            return 0;
        }
	} else {
        if (actual == NULL) {
            return 0;
        }
	}
	int i = 0;
    while(((*actual) != NULL) && (expected[i] != NULL)) {
        if (strcmp(expected[i], (*actual)) != 0) {
            return 0;
        }
        actual++;
        i++;
    }
    if ((*actual) != NULL || expected[i] != NULL) {
		return 0;
	}
	return 1;
}

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Implement me!
	char** case0 = (*camelCaser)(NULL);
    if (case0 != NULL) {
		return 0;
	}

	//case1: No punctuation, should return NULL.
	char** actual1 = (*camelCaser)("Michaelblabla");
	char* expected1[] = {NULL};
	char** temp1 = actual1;
	if (!checkSame(temp1, expected1)) {
		(*destroy)(temp1);
		return 0;
	}
	printf("Passing test case 1.\n");
	(*destroy)(temp1);

	//case2: Space combined with consecutive punctuations.
	char** actual2 = (*camelCaser)("\t\nMichaelhhh LOL.....\t\n 11 a. \n\t  22B  34c.");
	char* expected2[] = {"michaelhhhLol", "", "", "", "", "11A", "22b34C", NULL};
	char** temp2 = actual2;
	if (!checkSame(temp2, expected2)) {
		(*destroy)(temp2);
		return 0;
	}
	printf("Passing test case 2.\n");
	(*destroy)(temp2);

	//case3: punctuations with special character.
	char** actual3 = (*camelCaser)("\4a+++m456f ms4 k45 r| 000qwer <> R|vs2, 1 r.r3, \56 56 \3.f\31|");
	char* expected3[] = {"\4a", "", "", "m456fMs4K45R", "000qwer", "", "r", "vs2", "1R", "r3", "", "56\3", "f\31", NULL};
	char** temp3 = actual3;
	if (!checkSame(temp3, expected3)) {
		(*destroy)(temp3);
		return 0;
	}
	printf("Passing test case 3.\n");
	(*destroy)(temp3);

	//case4: different punctuation testing.
	char** actual4 = (*camelCaser)("@$^**^blablamichael.");
	char* expected4[] = {"","","","","","","blablamichael",NULL};
	char** temp4 = actual4;
	if (!checkSame(temp4, expected4)) {
		(*destroy)(temp4);
		return 0;
	}
	printf("Passing test case 4.\n");
	(*destroy)(temp4);

	//case5
	char** actual5 = (*camelCaser)("\tBlalalala\n Biulala\tBiubIU}\n \tLAJILAJIHAHAai?");
	char* expected5[] = {"blalalalaBiulalaBiubiu", "lajilajihahaai", NULL};
	char** temp5 = actual5;
	if (!checkSame(temp5, expected5)) {
		(*destroy)(temp5);
		return 0;
	}
	printf("Passing test case 5.\n");
	(*destroy)(temp5);

	//case6: Extended ASCII table.
	char** actual6 = (*camelCaser)("kk\a\1 ?7t8\ncc. \t\naá.\tñ.");
	char* expected6[] = {"kk\a\1",
        "7t8Cc",
        "aá",
        "ñ",
        NULL};
	char** temp6 = actual6;
	if (!checkSame(temp6, expected6)) {
		(*destroy)(temp6);
		return 0;
	}
	printf("Passing test case 6.\n");
	(*destroy)(temp6);

	//case7: All punctuations.
	char** actual7 = (*camelCaser)("^&*()_+-=[]{} ./<>?`~!@#$");
	char* expected7[] = {"","","","","","","","","","","","","","","","","","","","","","","","",NULL};
	char** temp7 = actual7;
	if (!checkSame(temp7, expected7)) {
		(*destroy)(temp7);
		return 0;
	}
	printf("Passing test case 7.\n");
	(*destroy)(temp7);

	//case8: Consecutive ASCII number.
	char** actual8 = (*camelCaser)("\a\b . \1\3\4\5\6\7A aB ?.");
	char* expected8[] = {"\a\b", "\1\3\4\5\6\7aAb", "", NULL};
	char** temp8 = actual8;
	if (!checkSame(temp8, expected8)) {
		(*destroy)(temp8);
		return 0;
	}
	printf("Passing test case 8.\n");
	(*destroy)(temp8);

	//case9: Empty string.
	char** actual9 = (*camelCaser)("");
	char* expected9[] = {NULL};
	char** temp9 = actual9;
	if (!checkSame(temp9, expected9)) {
		(*destroy)(temp9);
		return 0;
	}
	printf("Passing test case 9.\n");
	(*destroy)(temp9);

	//case10: Random test
	char** actual10 = (*camelCaser)("1233\t\n  a[ 22 2b: 44T 56g{@#$%^&*_=");
	char* expected10[] = {"1233A", "222B", "44t56G","","","","","","","","","", NULL};
	char** temp10 = actual10;
	if (!checkSame(temp10, expected10)) {
		(*destroy)(temp10);
		return 0;
	}
	printf("Passing test case 10.\n");
	(*destroy)(temp10);

	//case11: Ending with no punctuation.
	char** actual11 = (*camelCaser)("There is a guy, His name is Michael. He is stupid");
	char* expected11[] = {"thereIsAGuy", "hisNameIsMichael", NULL};
	char** temp11 = actual11;
	if (!checkSame(temp11, expected11)) {
		(*destroy)(temp11);
		return 0;
	}
	printf("Passing test case 11.\n");
	(*destroy)(temp11);

	//case12: Random boring test.
	char** actual12 = (*camelCaser)("Z.B<C,D>E.F/G?H:I;J'K)L(M+N-O_P|Q.1/1.1/2/4{5}.6.9*9&8^7% 3 r@ 4B b:88888");
	char* expected12[] = {"z","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","1","1","1","2","4", "5","", "6", "9", "9", "8", "7", "3R", "4bB", NULL};
	char** temp12 = actual12;
	if (!checkSame(temp12, expected12)) {
		(*destroy)(temp12);
		return 0;
	}
	printf("Passing test case 12.\n");
	(*destroy)(temp12);

	//case13: Simple weird case.
	char** actual13 = (*camelCaser)("\a\b. \1\3\4\5\6\7A aB ?");
	char* expected13[] = {"\a\b","\1\3\4\5\6\7aAb", NULL};
	char** temp13 = actual13;
	if (!checkSame(temp13, expected13)) {
		(*destroy)(temp13);
		return 0;
	}
	printf("Passing test case 13.\n");
	(*destroy)(temp13);

	//case14: Last case.
	char** actual14 = (*camelCaser)("as\t\n5ksbb}33aA\t9{K kIkjs.lll\\A\n444g_P_");
	char* expected14[] = {"as5Ksbb",
        "33aa9",
        "kKikjs",
        "lll",
        "a444G",
        "p",
        NULL};
	char** temp14 = actual14;
	if (!checkSame(temp14, expected14)) {
		(*destroy)(temp14);
		return 0;
	}
	printf("Passing test case 14.\n");
	(*destroy)(temp14);

    return 1;
}
