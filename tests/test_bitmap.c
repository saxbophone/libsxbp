/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides unit tests for the sxbp_Bitmap data type.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "check_wrapper.h"

#include "../sxbp/sxbp.h"

#include "test_suites.h"


START_TEST(test_blank_bitmap) {
    sxbp_Bitmap bitmap = sxbp_blank_bitmap();

    // bitmap returned should have all fields set to zero/blank values
    ck_assert(bitmap.width == 0);
    ck_assert(bitmap.height == 0);
    ck_assert_ptr_null(bitmap.pixels);
} END_TEST

START_TEST(test_init_bitmap) {
    sxbp_Bitmap bitmap = { .width = 32, .height = 64, .pixels = NULL, };

    sxbp_Result result = sxbp_init_bitmap(&bitmap);

    // check memory was allocated
    ck_assert(result == SXBP_RESULT_OK);
    ck_assert_ptr_nonnull(bitmap.pixels);
    // check all columns were allocated
    for (sxbp_FigureSize x = 0; x < bitmap.width; x++) {
        ck_assert_ptr_nonnull(bitmap.pixels[x]);
        // each pixel should be 'false'
        for (sxbp_FigureSize y = 0; y < bitmap.height; y++) {
            ck_assert(bitmap.pixels[x][y] == false);
        }
    }

    // cleanup
    sxbp_free_bitmap(&bitmap);
} END_TEST

START_TEST(test_init_bitmap_null) {
    sxbp_Result result = sxbp_init_bitmap(NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_init_bitmap_blank) {
    sxbp_Bitmap bitmap = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_init_bitmap(&bitmap);

    // return code should be 'not implemented'
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
    // no memory should have been allocated
    ck_assert_ptr_null(bitmap.pixels);
} END_TEST

START_TEST(test_init_bitmap_width_zero) {
    sxbp_Bitmap bitmap = { .width = 0, .height = 32, .pixels = NULL, };

    sxbp_Result result = sxbp_init_bitmap(&bitmap);

    // return code should be 'not implemented'
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
    // no memory should have been allocated
    ck_assert_ptr_null(bitmap.pixels);
} END_TEST

START_TEST(test_init_bitmap_height_zero) {
    sxbp_Bitmap bitmap = { .width = 32, .height = 0, .pixels = NULL, };

    sxbp_Result result = sxbp_init_bitmap(&bitmap);

    // return code should be 'not implemented'
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
    // no memory should have been allocated
    ck_assert_ptr_null(bitmap.pixels);
} END_TEST

START_TEST(test_free_bitmap_unallocated) {
    sxbp_Bitmap bitmap = sxbp_blank_bitmap();

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
    sxbp_Bitmap bitmap = { .width = 32, .height = 64, .pixels = NULL, };
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
    sxbp_Bitmap from = { .width = 32, .height = 64, .pixels = NULL, };
    /*
     * allocate the bitmap -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_bitmap(&from) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate bitmap");
    }
    // populate the bitmap with random pixels
    for (sxbp_FigureSize x = 0; x < from.width; x++) {
        for (sxbp_FigureSize y = 0; y < from.height; y++) {
            from.pixels[x][y] = rand() % 2;
        }
    }
    // this is the destination bitmap to copy to
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, &to);

    // check operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // memory should have been allocated
    ck_assert_ptr_nonnull(to.pixels);
    // check that contents are actually identical
    ck_assert(to.width == from.width);
    ck_assert(to.height == from.height);
    for (sxbp_FigureSize x = 0; x < to.width; x++) {
        for (sxbp_FigureSize y = 0; y < to.height; y++) {
            ck_assert(to.pixels[x][y] == from.pixels[x][y]);
        }
    }

    // cleanup
    sxbp_free_bitmap(&from);
    sxbp_free_bitmap(&to);
} END_TEST

START_TEST(test_copy_bitmap_from_null) {
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(NULL, &to);

    // precondition check error should be returned when from is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_bitmap_to_null) {
    sxbp_Bitmap from = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, NULL);

    // precondition check error should be returned when to is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_bitmap_blank) {
    sxbp_Bitmap from = sxbp_blank_bitmap();
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, &to);

    /*
     * it should be possible to successfully 'copy' a blank bitmap, with the
     * result being that no memory is allocated for to, and that all fields of
     * to are set to zero/NULL
     */
    ck_assert(result == SXBP_RESULT_OK);
    // check that 'to' is indeed still blank
    ck_assert(to.width == 0);
    ck_assert(to.height == 0);
    ck_assert_ptr_null(to.pixels);
} END_TEST

START_TEST(test_copy_bitmap_pixels_null) {
    sxbp_Bitmap from = {
        .width = 32,
        .height = 32,
        .pixels = NULL,
    };
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, &to);

    /*
     * if the source has non-zero size but pixels are NULL, a precondition
     * failure error should be returned
     */
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
    // just to be safe, also check that to has not been allocated
    ck_assert_ptr_null(to.pixels);
} END_TEST

START_TEST(test_copy_bitmap_width_zero_only) {
    sxbp_Bitmap from = {
        .width = 0,
        .height = 32,
        .pixels = NULL,
    };
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, &to);

    // the return code should be success
    ck_assert(result == SXBP_RESULT_OK);
    // to should not have been allocated
    ck_assert_ptr_null(to.pixels);
} END_TEST

START_TEST(test_copy_bitmap_height_zero_only) {
    sxbp_Bitmap from = {
        .width = 32,
        .height = 0,
        .pixels = NULL,
    };
    sxbp_Bitmap to = sxbp_blank_bitmap();

    sxbp_Result result = sxbp_copy_bitmap(&from, &to);

    // the return code should be success
    ck_assert(result == SXBP_RESULT_OK);
    // to should not have been allocated
    ck_assert_ptr_null(to.pixels);
} END_TEST

START_TEST(test_copy_bitmap_to_itself) {
    sxbp_Bitmap bitmap = { .width = 32, .height = 64, .pixels = NULL, };
    /*
     * allocate the bitmap -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_bitmap(&bitmap) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate bitmap");
    }
    // populate the bitmap with random pixels
    for (sxbp_FigureSize x = 0; x < bitmap.width; x++) {
        for (sxbp_FigureSize y = 0; y < bitmap.height; y++) {
            bitmap.pixels[x][y] = rand() % 2;
        }
    }
    // store pixels pointer for checking later
    bool** pixels = bitmap.pixels;

    // try and copy the bitmap to itself
    sxbp_Result result = sxbp_copy_bitmap(&bitmap, &bitmap);

    // not implemented error code should be returned
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
    // pixels have not been deallocated
    ck_assert_ptr_nonnull(bitmap.pixels);
    // pixels pointer remains unchanged
    ck_assert(bitmap.pixels == pixels);

    // cleanup
    sxbp_free_bitmap(&bitmap);
} END_TEST

Suite* make_bitmap_suite(void) {
    // Test cases for bitmap data type
    Suite* test_suite = suite_create("Bitmap");

    TCase* blank_bitmap = tcase_create("sxbp_blank_bitmap()");
    tcase_add_test(blank_bitmap, test_blank_bitmap);
    suite_add_tcase(test_suite, blank_bitmap);

    TCase* init_bitmap = tcase_create("sxbp_init_bitmap()");
    tcase_add_test(init_bitmap, test_init_bitmap);
    tcase_add_test(init_bitmap, test_init_bitmap_null);
    tcase_add_test(init_bitmap, test_init_bitmap_blank);
    tcase_add_test(init_bitmap, test_init_bitmap_width_zero);
    tcase_add_test(init_bitmap, test_init_bitmap_height_zero);
    suite_add_tcase(test_suite, init_bitmap);

    TCase* free_bitmap = tcase_create("sxbp_free_bitmap()");
    tcase_add_test(free_bitmap, test_free_bitmap_unallocated);
    tcase_add_test(free_bitmap, test_free_bitmap_allocated);
    suite_add_tcase(test_suite, free_bitmap);

    TCase* copy_bitmap = tcase_create("sxbp_copy_bitmap()");
    tcase_add_test(copy_bitmap, test_copy_bitmap);
    tcase_add_test(copy_bitmap, test_copy_bitmap_from_null);
    tcase_add_test(copy_bitmap, test_copy_bitmap_to_null);
    tcase_add_test(copy_bitmap, test_copy_bitmap_blank);
    tcase_add_test(copy_bitmap, test_copy_bitmap_pixels_null);
    tcase_add_test(copy_bitmap, test_copy_bitmap_width_zero_only);
    tcase_add_test(copy_bitmap, test_copy_bitmap_height_zero_only);
    tcase_add_test(copy_bitmap, test_copy_bitmap_to_itself);
    suite_add_tcase(test_suite, copy_bitmap);

    return test_suite;
}
