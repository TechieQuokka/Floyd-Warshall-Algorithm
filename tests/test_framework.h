#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <math.h>
#include "../src/core/constants.h"

/* Test result structure */
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestResult;

extern TestResult global_test_result;

/* Test assertion macros */
#define ASSERT(condition, message) do { \
    global_test_result.total_tests++; \
    if (condition) { \
        global_test_result.passed_tests++; \
        printf("PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("FAIL: %s\n", message); \
    } \
} while(0)

#define ASSERT_EQUAL(expected, actual, message) do { \
    global_test_result.total_tests++; \
    if ((expected) == (actual)) { \
        global_test_result.passed_tests++; \
        printf("PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("FAIL: %s (expected: %d, actual: %d)\n", \
               message, (int)(expected), (int)(actual)); \
    } \
} while(0)

#define ASSERT_DOUBLE_EQUAL(expected, actual, tolerance, message) do { \
    global_test_result.total_tests++; \
    if (fabs((expected) - (actual)) < (tolerance)) { \
        global_test_result.passed_tests++; \
        printf("PASS: %s\n", message); \
    } else { \
        global_test_result.failed_tests++; \
        printf("FAIL: %s (expected: %.6f, actual: %.6f)\n", \
               message, (expected), (actual)); \
    } \
} while(0)

/* Test summary function */
void test_summary(void);

/* Test runner functions */
void run_graph_tests(void);
void run_floyd_warshall_tests(void);
void run_memory_tests(void);
void run_file_io_tests(void);

#endif /* TEST_FRAMEWORK_H */