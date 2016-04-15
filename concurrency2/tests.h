#ifndef TESTS_H
#define TESTS_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define RED "\x1B[0;31m"
#define GREEN "\x1B[0;32m"
#define NOCOLOR "\x1B[0m"

int RETURN_VALUE = 0;
int __passed;
int __temp_left;
int __temp_right;

#define PRINTF_FORMAT(x) "%d"

#define DIAGNOSTICS(a, b, v1, v2) \
        printf("Expected "); \
        printf("%s: ", #b); \
        printf(PRINTF_FORMAT(v2), (v2)); \
        printf("\n");\
        printf("Actual "); \
        printf("%s: ", #a); \
        printf(PRINTF_FORMAT(v1), (v1)); \
        printf("\n"); \
        printf("Function %s, file %s, line %d.\n", __FUNCTION__, __FILE__, \
                __LINE__); \

#define EXPECT_EQ(a, b); printf("Test %s == %s ", (#a), (#b));\
__temp_left = (a);\
__temp_right = (b);\
if (__temp_left == __temp_right) {\
    printf(" %sPassed%s\n", GREEN, NOCOLOR);\
}\
else {\
    printf(" %sFailed%s\n", RED, NOCOLOR);\
    DIAGNOSTICS(a, b, __temp_left, __temp_right); \
    RETURN_VALUE = EXIT_FAILURE;\
}


#define EXPECT_NEQ(a, b); printf("Test %s != %s ", (#a), (#b));\
__temp_left = (a);\
__temp_right = (b);\
if (__temp_left == __temp_right) {\
    printf(" %sPassed%s\n", GREEN, NOCOLOR);\
}\
else {\
    printf(" %sFailed%s\n", RED, NOCOLOR);\
    DIAGNOSTICS(a, b, __temp_left, __temp_right); \
    RETURN_VALUE = EXIT_FAILURE;\
}

#endif
