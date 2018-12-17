#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <check.h>

#include "../sxbp/sxbp.h"

#include "test_suites.h"

// global (for this compilation unit) data useful for some tests
static const char* test_data_filename = "test_buffer_from_file.bin";
static const uint8_t sample_data[] = {
    0x33, 0x13, 0x98, 0x44, 0xf1, 0xf7, 0x7f, 0x6a,
};

// setup function for creating a test file, useful for a couple of test cases
static void setup(void) {
    // create a file and populate it with some data
    FILE* test_data = fopen(test_data_filename, "wb");
    if (test_data == NULL) {
        // if we can't open the file, then abort
        ck_abort_msg("Unable to create test file");
    }
    fwrite(
        sample_data,
        sizeof(uint8_t),
        sizeof(sample_data) / sizeof(uint8_t),
        test_data
    );
    // be a good person, close the file!
    fclose(test_data);
}

static void tear_down(void) {
    // delete the test file
    remove(test_data_filename);
}

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

START_TEST(test_buffer_from_file) {
    // open file in read mode, aborting if failure occurs
    FILE* temp_file = fopen(test_data_filename, "rb");
    if (temp_file == NULL) {
        ck_abort_msg("Unable to open test file in read mode");
    }
    // create a buffer to read the file into
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    // try and read the file into the buffer
    sxbp_result_t result = sxbp_buffer_from_file(temp_file, &buffer);

    // assert that the operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // check that what was read into the buffer was the actual data we created
    for (size_t i = 0; i < sizeof(sample_data) / sizeof(uint8_t); i++) {
        ck_assert(buffer.bytes[i] == sample_data[i]);
    }
} END_TEST

START_TEST(test_buffer_from_file_file_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_buffer_from_file(NULL, &buffer);

    // result should be precondition failure
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
    // buffer should be empty and unallocated
    ck_assert(buffer.size == 0);
    ck_assert_ptr_null(buffer.bytes);
} END_TEST

START_TEST(test_buffer_from_file_buffer_null) {
    // open file in read mode, aborting if failure occurs
    FILE* temp_file = fopen(test_data_filename, "rb");
    if (temp_file == NULL) {
        ck_abort_msg("Unable to open test file in read mode");
    }
    sxbp_result_t result = sxbp_buffer_from_file(temp_file, NULL);

    // result should be precondition failure
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
    // cleanup: close the file
    fclose(temp_file);
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

    TCase* buffer_from_file = tcase_create(
        "Buffer can be populated from the contents of an open file"
    );
    tcase_add_checked_fixture(buffer_from_file, setup, tear_down);
    tcase_add_test(buffer_from_file, test_buffer_from_file);
    suite_add_tcase(test_suite, buffer_from_file);

    TCase* buffer_from_file_file_null = tcase_create(
        "Buffer from file returns appropriate error code when file is NULL"
    );
    tcase_add_test(buffer_from_file_file_null, test_buffer_from_file_file_null);
    suite_add_tcase(test_suite, buffer_from_file_file_null);

    TCase* buffer_from_file_buffer_null = tcase_create(
        "Buffer from file returns appropriate error code when buffer is NULL"
    );
    tcase_add_checked_fixture(buffer_from_file_buffer_null, setup, tear_down);
    tcase_add_test(buffer_from_file_buffer_null, test_buffer_from_file_buffer_null);
    suite_add_tcase(test_suite, buffer_from_file_buffer_null);

    return test_suite;
}
