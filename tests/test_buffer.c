#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <check.h>

#include "../sxbp/sxbp.h"

#include "test_suites.h"


START_TEST(test_blank_buffer) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    // buffer returned should have all fields set to zero/blank values
    ck_assert(buffer.size == 0);
    ck_assert_ptr_null(buffer.bytes);
} END_TEST

START_TEST(test_init_buffer) {
    sxbp_buffer_t buffer = {
        .bytes = NULL,
        .size = 10000,
    };

    sxbp_result_t result = sxbp_init_buffer(&buffer);

    // check memory was allocated
    ck_assert(result == SXBP_RESULT_OK);
    ck_assert_ptr_nonnull(buffer.bytes);
    for (size_t i = 0; i < buffer.size; i++) {
        ck_assert(buffer.bytes[i] == 0);
    }
} END_TEST

START_TEST(test_init_buffer_null) {
    sxbp_result_t result = sxbp_init_buffer(NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_free_buffer_unallocated) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    /*
     * it should be possible to safely call the freeing function on an
     * unallocated buffer
     */
    bool needed_free = sxbp_free_buffer(&buffer);

    // no memory should be pointed to
    ck_assert_ptr_null(buffer.bytes);
    // the function should return false to tell us it didn't need to free
    ck_assert(!needed_free);
} END_TEST

START_TEST(test_free_buffer_allocated) {
    sxbp_buffer_t buffer = { .size = 10000, .bytes = NULL, };
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_buffer(&buffer) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }

    // calling the freeing function on this allocated buffer should free memory
    bool needed_free = sxbp_free_buffer(&buffer);

    // no memory should be pointed to
    ck_assert_ptr_null(buffer.bytes);
    // the function should return true to tell us it needed to free
    ck_assert(needed_free);
} END_TEST

START_TEST(test_copy_buffer) {
    sxbp_buffer_t from = { .size = 10000, .bytes = NULL, };
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_buffer(&from) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // populate the buffer with 'random' bytes
    for (size_t i = 0; i < from.size; i++) {
        from.bytes[i] = rand() / 128;
    }
    // this is the destination buffer to copy to
    sxbp_buffer_t to = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_copy_buffer(&from, &to);

    // check operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // memory should have been allocated
    ck_assert_ptr_nonnull(to.bytes);
    // check that contents are actually identical
    ck_assert(to.size == from.size);
    for (size_t i = 0; i < to.size; i++) {
        ck_assert(to.bytes[i] == from.bytes[i]);
    }
} END_TEST

START_TEST(test_copy_buffer_from_null) {
    sxbp_buffer_t to = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_copy_buffer(NULL, &to);

    // precondition check error should be returned when from is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_buffer_to_null) {
    sxbp_buffer_t from = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_copy_buffer(&from, NULL);

    // precondition check error should be returned when to is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

Suite* make_buffer_suite(void) {
    // Test cases for buffer data type
    Suite* test_suite = suite_create("Buffer");

    TCase* blank_buffer = tcase_create("Create blank Buffer");
    tcase_add_test(blank_buffer, test_blank_buffer);
    suite_add_tcase(test_suite, blank_buffer);

    TCase* init_buffer = tcase_create("Allocate a Buffer");
    tcase_add_test(init_buffer, test_init_buffer);
    suite_add_tcase(test_suite, init_buffer);

    TCase* init_buffer_null = tcase_create(
        "Buffer allocation returns appropriate error code when given NULL pointer"
    );
    tcase_add_test(init_buffer_null, test_init_buffer_null);
    suite_add_tcase(test_suite, init_buffer_null);

    TCase* free_buffer_unallocated = tcase_create("Free an unallocated Buffer");
    tcase_add_test(free_buffer_unallocated, test_free_buffer_unallocated);
    suite_add_tcase(test_suite, free_buffer_unallocated);

    TCase* free_buffer_allocated = tcase_create("Free an allocated Buffer");
    tcase_add_test(free_buffer_allocated, test_free_buffer_allocated);
    suite_add_tcase(test_suite, free_buffer_allocated);

    TCase* copy_buffer = tcase_create("Copy a Buffer");
    tcase_add_test(copy_buffer, test_copy_buffer);
    suite_add_tcase(test_suite, copy_buffer);

    TCase* copy_buffer_from_null = tcase_create(
        "Buffer copying returns appropriate error code when from is NULL"
    );
    tcase_add_test(copy_buffer_from_null, test_copy_buffer_from_null);
    suite_add_tcase(test_suite, copy_buffer_from_null);

    TCase* copy_buffer_to_null = tcase_create(
        "Buffer copying returns appropriate error code when to is NULL"
    );
    tcase_add_test(copy_buffer_to_null, test_copy_buffer_to_null);
    suite_add_tcase(test_suite, copy_buffer_to_null);

    return test_suite;
}
