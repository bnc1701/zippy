#ifndef ZIPPY_TEST_UTILS_H
#define ZIPPY_TEST_UTILS_H

#include <stdio.h>

// tracks pass and fail counts across a test run

extern int tests_run;
extern int tests_failed;

#define CHECK(condition, message) \
    do { \
        tests_run++; \
        if (!(condition)) { \
            tests_failed++; \
            fprintf(stderr, "fail: %s (%s:%d)\n", message, __FILE__, __LINE__); \
        } \
    } while (0)

#endif
