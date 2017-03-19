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
#ifndef SAXBOPHONE_COLRCV_TEST_HARNESS_H
#define SAXBOPHONE_COLRCV_TEST_HARNESS_H

#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C"{
#endif

// enum for test suite test case results
typedef enum colrcv_test_status_t {
    COLRCV_TEST_UNKNOWN = 0,
    COLRCV_TEST_FAIL,
    COLRCV_TEST_ERROR,
    COLRCV_TEST_SUCCESS,
} colrcv_test_status_t;

// struct for test result and test name
typedef struct colrcv_test_result_t {
    colrcv_test_status_t result;
    const char* name;
} colrcv_test_result_t;

/*
 * macro for storing function name as test name
 * usage: colrcv_test_result_t result = COLRCV_TEST;
 */
#define COLRCV_TEST (colrcv_test_result_t) { .result = COLRCV_TEST_UNKNOWN, .name = __func__, }

// struct for representing a test case
typedef struct colrcv_test_case_t {
    // function pointer to function to run for test
    colrcv_test_result_t(* function)(void);
    // test result status
    colrcv_test_result_t result;
} colrcv_test_case_t;

// struct for representing a whole test suite (one per module/test executable)
typedef struct colrcv_test_suite_t {
    // pointer to an array of test cases
    colrcv_test_case_t* tests;
    size_t test_count;
    // test suite fail / pass flag
    bool result;
} colrcv_test_suite_t;

// returns a blank test suite
colrcv_test_suite_t colrcv_init_test_suite(void);

// tears down and free()s a test suite
void colrcv_free_test_suite(colrcv_test_suite_t suite);

/*
 * adds a function as a test case to a test suite
 * function must return a test_result_t struct and take no arguments
 */
void colrcv_add_test_case(
    colrcv_test_result_t(* function)(void), colrcv_test_suite_t* suite
);

// runs all test cases in a test suite and stores result success / failure
void colrcv_run_test_suite(colrcv_test_suite_t* suite);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
