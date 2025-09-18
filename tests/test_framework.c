#include "test_framework.h"

TestResult global_test_result = {0, 0, 0};

void test_summary(void) {
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", global_test_result.total_tests);
    printf("Passed: %d\n", global_test_result.passed_tests);
    printf("Failed: %d\n", global_test_result.failed_tests);

    if (global_test_result.failed_tests == 0) {
        printf("All tests PASSED!\n");
    } else {
        printf("Some tests FAILED!\n");
    }

    double pass_rate = 0.0;
    if (global_test_result.total_tests > 0) {
        pass_rate = (double)global_test_result.passed_tests / global_test_result.total_tests * 100.0;
    }
    printf("Pass rate: %.1f%%\n", pass_rate);
    printf("==================\n");
}