/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This unit provides a simple test harness for libsxbp's unit tests
 *
 *
 * Copyright (C) 2017, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "harness.h"


#ifdef __cplusplus
extern "C"{
#endif

// returns a blank test suite
sxbp_test_suite_t sxbp_init_test_suite(void) {
    return (sxbp_test_suite_t) {
        .tests = NULL, .test_count = 0, .result = true
    };
}

// tears down and free()s a test suite
void sxbp_free_test_suite(sxbp_test_suite_t suite) {
    if(suite.test_count > 0) {
        free(suite.tests);
        suite.test_count = 0;
    }
}

/*
 * adds a function as a test case to a test suite
 * function must return a test_result_t struct and take no arguments
 */
void sxbp_add_test_case(
    sxbp_test_result_t(* function)(void), sxbp_test_suite_t * suite
) {
    // increment test count
    suite->test_count++;
    // allocate or re-allocate memory for adding test case
    if(suite->tests == NULL) {
        // first time allocation
        suite->tests = (sxbp_test_case_t*) malloc(
            sizeof(sxbp_test_case_t)
        );
    } else {
        // re-allocate
        suite->tests = (sxbp_test_case_t*) realloc(
            suite->tests, sizeof(sxbp_test_case_t) * suite->test_count
        );
    }
    // assign function pointer to latest test case
    suite->tests[suite->test_count - 1].function = function;
}

// returns string for test result code
static const char* sxbp_test_status_string(sxbp_test_status_t status) {
    switch(status) {
        case SXBP_TEST_UNKNOWN:
            return "NO RESULT";
        case SXBP_TEST_FAIL:
            return "FAIL";
        case SXBP_TEST_ERROR:
            return "ERROR";
        case SXBP_TEST_SUCCESS:
            return "SUCCESS";
        default:
            return "UNEXPECTED RESULT";
    }
}

// runs all test cases in a test suite and stores result success / failure
void sxbp_run_test_suite(sxbp_test_suite_t * suite) {
    // iterate over every test case in array
    for(size_t i = 0; i < suite->test_count; i++) {
        // run the current test case and store result
        sxbp_test_result_t result = suite->tests[i].function();
        // print out the test case name and return code
        printf(
            "%s\t%s\n", result.name, sxbp_test_status_string(result.result)
        );
        // combine with current stored result in test suite
        suite->result = (
            (result.result == SXBP_TEST_SUCCESS) ? true : false
        ) && suite->result;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
