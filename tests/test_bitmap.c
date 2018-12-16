#include <assert.h>
#include <stdlib.h>

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
     * allocate the bitmap -we use regular assert here to check failure,
     * because this test case is not testing the init function
     */
    assert(sxbp_init_bitmap(&bitmap) == SXBP_RESULT_OK);

    // calling the freeing function on this allocated bitmap should free memory
    bool needed_free = sxbp_free_bitmap(&bitmap);

    // no memory should be pointed to
    ck_assert_ptr_null(bitmap.pixels);
    // the function should return true to tell us it needed to free
    ck_assert(needed_free);
} END_TEST

Suite* make_bitmap_suite(void) {
    // Test cases for bitmap data type
    Suite* s = suite_create("Bitmap");

    TCase* blank_bitmap = tcase_create("Create blank Bitmap");
    tcase_add_test(blank_bitmap, test_blank_bitmap);
    suite_add_tcase(s, blank_bitmap);

    TCase* init_bitmap = tcase_create("Allocate a Bitmap");
    tcase_add_test(init_bitmap, test_init_bitmap);
    suite_add_tcase(s, init_bitmap);

    TCase* free_bitmap_unallocated = tcase_create("Free an unallocated Bitmap");
    tcase_add_test(free_bitmap_unallocated, test_free_bitmap_unallocated);
    suite_add_tcase(s, free_bitmap_unallocated);

    TCase* free_bitmap_allocated = tcase_create("Free an allocated Bitmap");
    tcase_add_test(free_bitmap_allocated, test_free_bitmap_allocated);
    suite_add_tcase(s, free_bitmap_allocated);

    return s;
}
