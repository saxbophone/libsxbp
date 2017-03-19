/*
 * This source file forms part of colrcv
 * colrcv is a C Library for converting Colours between different Colour Models.
 *
 *
 * Copyright (C) 2017, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * The Copyright holder reserves all rights of this work.
 *
 * No copying or reproduction is permitted without the express, written consent
 * of the Copyright holder.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "harness.h"


#ifdef __cplusplus
extern "C"{
#endif

// returns a blank test suite
colrcv_test_suite_t colrcv_init_test_suite(void) {
    return (colrcv_test_suite_t) {
        .tests = NULL, .test_count = 0, .result = true
    };
}

// tears down and free()s a test suite
void colrcv_free_test_suite(colrcv_test_suite_t suite) {
    if(suite.test_count > 0) {
        free(suite.tests);
        suite.test_count = 0;
    }
}

/*
 * adds a function as a test case to a test suite
 * function must return a test_result_t struct and take no arguments
 */
void colrcv_add_test_case(
    colrcv_test_result_t(* function)(void), colrcv_test_suite_t * suite
) {
    // increment test count
    suite->test_count++;
    // allocate or re-allocate memory for adding test case
    if(suite->tests == NULL) {
        // first time allocation
        suite->tests = (colrcv_test_case_t*) malloc(
            sizeof(colrcv_test_case_t)
        );
    } else {
        // re-allocate
        suite->tests = (colrcv_test_case_t*) realloc(
            suite->tests, sizeof(colrcv_test_case_t) * suite->test_count
        );
    }
    // assign function pointer to latest test case
    suite->tests[suite->test_count - 1].function = function;
}

// returns string for test result code
static const char* colrcv_test_status_string(colrcv_test_status_t status) {
    switch(status) {
        case COLRCV_TEST_UNKNOWN:
            return "NO RESULT";
        case COLRCV_TEST_FAIL:
            return "FAIL";
        case COLRCV_TEST_ERROR:
            return "ERROR";
        case COLRCV_TEST_SUCCESS:
            return "SUCCESS";
        default:
            return "UNEXPECTED RESULT";
    }
}

// runs all test cases in a test suite and stores result success / failure
void colrcv_run_test_suite(colrcv_test_suite_t * suite) {
    // iterate over every test case in array
    for(size_t i = 0; i < suite->test_count; i++) {
        // run the current test case and store result
        colrcv_test_result_t result = suite->tests[i].function();
        // print out the test case name and return code
        printf(
            "%s\t%s\n", result.name, colrcv_test_status_string(result.result)
        );
        // combine with current stored result in test suite
        suite->result = (
            (result.result == COLRCV_TEST_SUCCESS) ? true : false
        ) && suite->result;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
