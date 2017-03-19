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
#ifndef SAXBOPHONE_SXBP_TEST_HARNESS_H
#define SAXBOPHONE_SXBP_TEST_HARNESS_H

#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C"{
#endif

// enum for test suite test case results
typedef enum sxbp_test_status_t {
    SXBP_TEST_UNKNOWN = 0,
    SXBP_TEST_FAIL,
    SXBP_TEST_ERROR,
    SXBP_TEST_SUCCESS,
} sxbp_test_status_t;

// struct for test result and test name
typedef struct sxbp_test_result_t {
    sxbp_test_status_t result;
    const char* name;
} sxbp_test_result_t;

/*
 * macro for storing function name as test name
 * usage: sxbp_test_result_t result = SXBP_TEST;
 */
#define SXBP_TEST (sxbp_test_result_t) { .result = SXBP_TEST_UNKNOWN, .name = __func__, }

// struct for representing a test case
typedef struct sxbp_test_case_t {
    // function pointer to function to run for test
    sxbp_test_result_t(* function)(void);
    // test result status
    sxbp_test_result_t result;
} sxbp_test_case_t;

// struct for representing a whole test suite (one per module/test executable)
typedef struct sxbp_test_suite_t {
    // pointer to an array of test cases
    sxbp_test_case_t* tests;
    size_t test_count;
    // test suite fail / pass flag
    bool result;
} sxbp_test_suite_t;

// returns a blank test suite
sxbp_test_suite_t sxbp_init_test_suite(void);

// tears down and free()s a test suite
void sxbp_free_test_suite(sxbp_test_suite_t suite);

/*
 * adds a function as a test case to a test suite
 * function must return a test_result_t struct and take no arguments
 */
void sxbp_add_test_case(
    sxbp_test_result_t(* function)(void), sxbp_test_suite_t* suite
);

// runs all test cases in a test suite and stores result success / failure
void sxbp_run_test_suite(sxbp_test_suite_t* suite);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
