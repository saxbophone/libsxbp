#include <stdbool.h>
#include <stdint.h>

#include <check.h>

#include "../sxbp/sxbp.h"

#include "test_suites.h"


START_TEST(test_blank_bitmap) {
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();

    // bitmap returned should have all fields set to zero/blank values
    ck_assert(bitmap.width == 0);
    ck_assert(bitmap.height == 0);
    ck_assert_ptr_null(bitmap.pixels);
} END_TEST

START_TEST(test_init_bitmap) {
    sxbp_bitmap_t bitmap = { .width = 32, .height = 64, .pixels = NULL, };

    sxbp_result_t result = sxbp_init_bitmap(&bitmap);

    // check memory was allocated
    ck_assert(result == SXBP_RESULT_OK);
    ck_assert_ptr_nonnull(bitmap.pixels);
    // check all columns were allocated
    for (sxbp_figure_size_t x = 0; x < bitmap.width; x++) {
        ck_assert_ptr_nonnull(bitmap.pixels[x]);
        // each pixel should be 'false'
        for (sxbp_figure_size_t y = 0; y < bitmap.height; y++) {
            ck_assert(bitmap.pixels[x][y] == false);
        }
    }
} END_TEST

START_TEST(test_init_bitmap_null_pointer_error) {
    sxbp_result_t result = sxbp_init_bitmap(NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_free_bitmap_unallocated) {
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();

    /*
     * it should be possible to safely call the freeing function on an
     * unallocated bitmap
     */
    bool needed_free = sxbp_free_bitmap(&bitmap);

    // no memory should be pointed to
    ck_assert_ptr_null(bitmap.pixels);
    // the function should return false to tell us it didn't need to free
    ck_assert(!needed_free);
} END_TEST

START_TEST(test_free_bitmap_allocated) {
    sxbp_bitmap_t bitmap = { .width = 32, .height = 64, .pixels = NULL, };
    /*
     * allocate the bitmap -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_bitmap(&bitmap) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate bitmap");
    }

    // calling the freeing function on this allocated bitmap should free memory
    bool needed_free = sxbp_free_bitmap(&bitmap);

    // no memory should be pointed to
    ck_assert_ptr_null(bitmap.pixels);
    // the function should return true to tell us it needed to free
    ck_assert(needed_free);
} END_TEST

START_TEST(test_copy_bitmap) {
    sxbp_bitmap_t from = { .width = 32, .height = 64, .pixels = NULL, };
    /*
     * allocate the bitmap -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_bitmap(&from) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate bitmap");
    }
    // populate the bitmap with a chessboard pattern
    for (sxbp_figure_size_t x = 0; x < from.width; x++) {
        for (sxbp_figure_size_t y = 0; y < from.height; y++) {
            from.pixels[x][y] = (x + y) % 2;
        }
    }
    // this is the destination bitmap to copy to
    sxbp_bitmap_t to = sxbp_blank_bitmap();

    sxbp_result_t result = sxbp_copy_bitmap(&from, &to);

    // check operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // memory should have been allocated
    ck_assert_ptr_nonnull(to.pixels);
    // check that contents are actually identical
    for (sxbp_figure_size_t x = 0; x < to.width; x++) {
        for (sxbp_figure_size_t y = 0; y < to.height; y++) {
            ck_assert(to.pixels[x][y] == from.pixels[x][y]);
        }
    }
} END_TEST

START_TEST(test_copy_bitmap_from_null) {
    sxbp_bitmap_t to = sxbp_blank_bitmap();

    sxbp_result_t result = sxbp_copy_bitmap(NULL, &to);

    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_bitmap_to_null) {
    sxbp_bitmap_t from = sxbp_blank_bitmap();

    sxbp_result_t result = sxbp_copy_bitmap(&from, NULL);

    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

Suite* make_bitmap_suite(void) {
    // Test cases for bitmap data type
    Suite* test_suite = suite_create("Bitmap");

    TCase* blank_bitmap = tcase_create("Create blank Bitmap");
    tcase_add_test(blank_bitmap, test_blank_bitmap);
    suite_add_tcase(test_suite, blank_bitmap);

    TCase* init_bitmap = tcase_create("Allocate a Bitmap");
    tcase_add_test(init_bitmap, test_init_bitmap);
    suite_add_tcase(test_suite, init_bitmap);

    TCase* init_bitmap_null_pointer_error = tcase_create(
        "Bitmap allocation returns appropriate error code when given NULL pointer"
    );
    tcase_add_test(
        init_bitmap_null_pointer_error,
        test_init_bitmap_null_pointer_error
    );
    suite_add_tcase(test_suite, init_bitmap_null_pointer_error);

    TCase* free_bitmap_unallocated = tcase_create("Free an unallocated Bitmap");
    tcase_add_test(free_bitmap_unallocated, test_free_bitmap_unallocated);
    suite_add_tcase(test_suite, free_bitmap_unallocated);

    TCase* free_bitmap_allocated = tcase_create("Free an allocated Bitmap");
    tcase_add_test(free_bitmap_allocated, test_free_bitmap_allocated);
    suite_add_tcase(test_suite, free_bitmap_allocated);

    TCase* copy_bitmap = tcase_create("Copy a Bitmap");
    tcase_add_test(copy_bitmap, test_copy_bitmap);
    suite_add_tcase(test_suite, copy_bitmap);

    TCase* copy_bitmap_from_null = tcase_create(
        "Bitmap copying returns appropriate error code when from is NULL"
    );
    tcase_add_test(copy_bitmap_from_null, test_copy_bitmap_from_null);
    suite_add_tcase(test_suite, copy_bitmap_from_null);

    TCase* copy_bitmap_to_null = tcase_create(
        "Bitmap copying returns appropriate error code when to is NULL"
    );
    tcase_add_test(copy_bitmap_to_null, test_copy_bitmap_to_null);
    suite_add_tcase(test_suite, copy_bitmap_to_null);

    return test_suite;
}
