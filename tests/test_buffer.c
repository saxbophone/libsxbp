#include <stddef.h>

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

    return test_suite;
}
