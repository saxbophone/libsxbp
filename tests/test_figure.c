/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides unit tests for the sxbp_figure_t data type.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "check_wrapper.h"

#include "../sxbp/sxbp.h"

#include "test_suites.h"


// global module-private data that is useful for serialisation tests

static const uint8_t SAMPLE_SEED = 0x6D;
static const size_t SAMPLE_FIGURE_SIZE = 9;

static const sxbp_line_t SAMPLE_FIGURE_LINES[] = {
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_RIGHT, .length = 1, },
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_LEFT, .length = 2, },
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_LEFT, .length = 1, },
    { .direction = SXBP_DOWN, .length = 4, },
    { .direction = SXBP_LEFT, .length = 1, },
    { .direction = SXBP_DOWN, .length = 1, },
};

static const sxbp_line_t REFINED_SAMPLE_FIGURE_LINES[] = {
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_RIGHT, .length = 1, },
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_LEFT, .length = 1, },
    { .direction = SXBP_UP, .length = 1, },
    { .direction = SXBP_LEFT, .length = 1, },
    { .direction = SXBP_DOWN, .length = 1, },
    { .direction = SXBP_LEFT, .length = 1, },
    { .direction = SXBP_DOWN, .length = 1, },
};

static const uint8_t SAMPLE_SXBP_FILE_DATA[] = {
    0x73, 0x78, 0x62, 0x70, // "sxbp"
    0x00, 0x00, // major version
    0x00, 0x36, // minor version
    0x00, 0x00, // patch version
    0x00, 0x00, 0x00, 0x09, // number of lines total
    0xFF, 0xFF, 0xFF, 0xFF, // unused, formerly number of lines solved
    0xFF, 0xFF, 0xFF, 0xFF, // unused, formerly seconds spent solving
    0x00, 0x00, 0x00, 0x00, // number of lines remaining to be solved
    0x00, 0x00, 0x00, 0x01, // line 0
    0x40, 0x00, 0x00, 0x01, // line 1
    0x00, 0x00, 0x00, 0x01, // line 2
    0xC0, 0x00, 0x00, 0x02, // line 3
    0x00, 0x00, 0x00, 0x01, // line 4
    0xC0, 0x00, 0x00, 0x01, // line 5
    0x80, 0x00, 0x00, 0x04, // line 6
    0xC0, 0x00, 0x00, 0x01, // line 7
    0x80, 0x00, 0x00, 0x01, // line 8
};

static const uint8_t SAMPLE_PBM_FILE_DATA[] = {
    0x50, 0x34, 0x0a, // "P4"
    0x37, 0x0a, // "7"
    0x37, 0x0a, // "7"
    0x38, 0x28, 0xee, 0x82, 0x8e, 0x00, 0x08, // one row  of pixels per per byte
};

static const uint8_t SAMPLE_SVG_FILE_DATA[] = {
    0x3c, 0x73, 0x76, 0x67, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c,
    0x6e, 0x73, 0x3d, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77,
    0x77, 0x77, 0x2e, 0x77, 0x33, 0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x32, 0x30,
    0x30, 0x30, 0x2f, 0x73, 0x76, 0x67, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x76, 0x69, 0x65, 0x77, 0x42, 0x6f, 0x78, 0x3d, 0x22, 0x30, 0x20, 0x30,
    0x20, 0x37, 0x20, 0x37, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74,
    0x79, 0x6c, 0x65, 0x3d, 0x22, 0x62, 0x61, 0x63, 0x6b, 0x67, 0x72, 0x6f,
    0x75, 0x6e, 0x64, 0x2d, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3a, 0x20, 0x77,
    0x68, 0x69, 0x74, 0x65, 0x22, 0x0a, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x3c, 0x72, 0x65, 0x63, 0x74, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x78, 0x3d, 0x22, 0x30, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x79, 0x3d, 0x22, 0x30, 0x22, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3d,
    0x22, 0x31, 0x30, 0x30, 0x25, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x68, 0x65, 0x69, 0x67, 0x68, 0x74, 0x3d, 0x22, 0x31,
    0x30, 0x30, 0x25, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x66, 0x69, 0x6c, 0x6c, 0x3d, 0x22, 0x77, 0x68, 0x69, 0x74, 0x65,
    0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x3e, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x3c, 0x72, 0x65, 0x63, 0x74, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x78, 0x3d, 0x22, 0x34, 0x22, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x79, 0x3d, 0x22, 0x36, 0x22, 0x0a, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x77, 0x69, 0x64, 0x74, 0x68,
    0x3d, 0x22, 0x31, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x68, 0x65, 0x69, 0x67, 0x68, 0x74, 0x3d, 0x22, 0x31, 0x22, 0x0a,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x69, 0x6c, 0x6c,
    0x3d, 0x22, 0x62, 0x6c, 0x61, 0x63, 0x6b, 0x22, 0x0a, 0x20, 0x20, 0x20,
    0x20, 0x2f, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x70, 0x6f, 0x6c,
    0x79, 0x6c, 0x69, 0x6e, 0x65, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x66, 0x69, 0x6c, 0x6c, 0x3d, 0x22, 0x6e, 0x6f, 0x6e, 0x65,
    0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74,
    0x72, 0x6f, 0x6b, 0x65, 0x3d, 0x22, 0x62, 0x6c, 0x61, 0x63, 0x6b, 0x22,
    0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74, 0x72,
    0x6f, 0x6b, 0x65, 0x2d, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3d, 0x22, 0x31,
    0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74,
    0x72, 0x6f, 0x6b, 0x65, 0x2d, 0x6c, 0x69, 0x6e, 0x65, 0x63, 0x61, 0x70,
    0x3d, 0x22, 0x73, 0x71, 0x75, 0x61, 0x72, 0x65, 0x22, 0x0a, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74, 0x72, 0x6f, 0x6b, 0x65,
    0x2d, 0x6c, 0x69, 0x6e, 0x65, 0x6a, 0x6f, 0x69, 0x6e, 0x3d, 0x22, 0x6d,
    0x69, 0x74, 0x65, 0x72, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x70, 0x6f, 0x69, 0x6e, 0x74, 0x73, 0x3d, 0x22, 0x34, 0x2e,
    0x35, 0x2c, 0x34, 0x2e, 0x35, 0x20, 0x35, 0x2e, 0x35, 0x2c, 0x34, 0x2e,
    0x35, 0x20, 0x36, 0x2e, 0x35, 0x2c, 0x34, 0x2e, 0x35, 0x20, 0x36, 0x2e,
    0x35, 0x2c, 0x33, 0x2e, 0x35, 0x20, 0x36, 0x2e, 0x35, 0x2c, 0x32, 0x2e,
    0x35, 0x20, 0x35, 0x2e, 0x35, 0x2c, 0x32, 0x2e, 0x35, 0x20, 0x34, 0x2e,
    0x35, 0x2c, 0x32, 0x2e, 0x35, 0x20, 0x34, 0x2e, 0x35, 0x2c, 0x31, 0x2e,
    0x35, 0x20, 0x34, 0x2e, 0x35, 0x2c, 0x30, 0x2e, 0x35, 0x20, 0x33, 0x2e,
    0x35, 0x2c, 0x30, 0x2e, 0x35, 0x20, 0x32, 0x2e, 0x35, 0x2c, 0x30, 0x2e,
    0x35, 0x20, 0x32, 0x2e, 0x35, 0x2c, 0x31, 0x2e, 0x35, 0x20, 0x32, 0x2e,
    0x35, 0x2c, 0x32, 0x2e, 0x35, 0x20, 0x31, 0x2e, 0x35, 0x2c, 0x32, 0x2e,
    0x35, 0x20, 0x30, 0x2e, 0x35, 0x2c, 0x32, 0x2e, 0x35, 0x20, 0x30, 0x2e,
    0x35, 0x2c, 0x33, 0x2e, 0x35, 0x20, 0x30, 0x2e, 0x35, 0x2c, 0x34, 0x2e,
    0x35, 0x22, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x3e, 0x0a, 0x3c, 0x2f,
    0x73, 0x76, 0x67, 0x3e, 0x0a,
};

START_TEST(test_blank_figure) {
    sxbp_figure_t figure = sxbp_blank_figure();

    // figure returned should have all fields set to zero/blank values
    ck_assert(figure.size == 0);
    ck_assert_ptr_null(figure.lines);
    ck_assert(figure.lines_remaining == 0);
} END_TEST

START_TEST(test_init_figure) {
    sxbp_figure_t figure = {
        .size = 100,
        .lines = NULL,
        .lines_remaining = 0,
    };

    sxbp_result_t result = sxbp_init_figure(&figure);

    // check memory was allocated
    ck_assert(result == SXBP_RESULT_OK);
    ck_assert_ptr_nonnull(figure.lines);
    for (size_t i = 0; i < figure.size; i++) {
        ck_assert(figure.lines[i].length == 0);
        ck_assert(figure.lines[i].direction == 0);
    }

    // cleanup
    sxbp_free_figure(&figure);
} END_TEST

START_TEST(test_init_figure_null) {
    sxbp_result_t result = sxbp_init_figure(NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_init_figure_blank) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_init_figure(&figure);

    // check that the return code was a 'not implemented' error
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
} END_TEST

START_TEST(test_free_figure_unallocated) {
    sxbp_figure_t figure = sxbp_blank_figure();

    /*
     * it should be possible to safely call the freeing function on an
     * unallocated figure
     */
    bool needed_free = sxbp_free_figure(&figure);

    // no memory should be pointed to
    ck_assert_ptr_null(figure.lines);
    // the function should return false to tell us it didn't need to free
    ck_assert(!needed_free);
} END_TEST

START_TEST(test_free_figure_allocated) {
    sxbp_figure_t figure = {
        .size = 100,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }

    // calling the freeing function on this allocated figure should free memory
    bool needed_free = sxbp_free_figure(&figure);

    // no memory should be pointed to
    ck_assert_ptr_null(figure.lines);
    // the function should return true to tell us it needed to free
    ck_assert(needed_free);
} END_TEST

START_TEST(test_copy_figure) {
    sxbp_figure_t from = {
        .size = 100,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&from) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with 'random' lines
    for (size_t i = 0; i < from.size; i++) {
        from.lines[i].direction = rand() & 0x03; // range 0..3
        from.lines[i].length = rand() & 0x3fffffff; // range 0..2^30
    }
    // this is the destination figure to copy to
    sxbp_figure_t to = sxbp_blank_figure();

    sxbp_result_t result = sxbp_copy_figure(&from, &to);

    // check operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // memory should have been allocated
    ck_assert_ptr_nonnull(to.lines);
    // check that contents are actually identical
    ck_assert(to.size == from.size);
    ck_assert(to.lines_remaining == from.lines_remaining);
    for (size_t i = 0; i < to.size; i++) {
        ck_assert(to.lines[i].direction == from.lines[i].direction);
        ck_assert(to.lines[i].length == from.lines[i].length);
    }

    // cleanup
    sxbp_free_figure(&from);
    sxbp_free_figure(&to);
} END_TEST

START_TEST(test_copy_figure_from_null) {
    sxbp_figure_t to = sxbp_blank_figure();

    sxbp_result_t result = sxbp_copy_figure(NULL, &to);

    // precondition check error should be returned when from is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_figure_to_null) {
    sxbp_figure_t from = sxbp_blank_figure();

    sxbp_result_t result = sxbp_copy_figure(&from, NULL);

    // precondition check error should be returned when to is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_copy_figure_blank) {
    sxbp_figure_t from = sxbp_blank_figure();
    sxbp_figure_t to = sxbp_blank_figure();

    sxbp_result_t result = sxbp_copy_figure(&from, &to);

    /*
     * it should be possible to successfully 'copy' a blank figure, with the
     * result being that no memory is allocated for to, and that all fields of
     * to are set to zero/NULL
     */
    ck_assert(result == SXBP_RESULT_OK);
    // check that 'to' is indeed still blank
    ck_assert(to.size == 0);
    ck_assert_ptr_null(to.lines);
    ck_assert(to.lines_remaining == 0);
} END_TEST

START_TEST(test_copy_figure_lines_null) {
    sxbp_figure_t from = {
        .size = 32,
        .lines = NULL,
        .lines_remaining = 0,
    };
    sxbp_figure_t to = sxbp_blank_figure();

    sxbp_result_t result = sxbp_copy_figure(&from, &to);

    /*
     * if the source has non-zero size but lines are NULL, a precondition
     * failure error should be returned
     */
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
    // just to be safe, also check that to has not been allocated
    ck_assert_ptr_null(to.lines);
} END_TEST

START_TEST(test_begin_figure) {
    sxbp_buffer_t buffer = { .size = 1, .bytes = NULL, };
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_buffer(&buffer) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // populate our one byte -this 'seed' should generate our sample lines
    buffer.bytes[0] = SAMPLE_SEED;
    // create a blank figure to write the created figure into
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(&buffer, NULL, &figure);

    // check the result was success
    ck_assert(result == SXBP_RESULT_OK);
    // check that the figure contains the sample lines and quantity thereof
    ck_assert(figure.size == SAMPLE_FIGURE_SIZE);
    for (size_t i = 0; i < SAMPLE_FIGURE_SIZE; i++) {
        ck_assert(figure.lines[i].direction == SAMPLE_FIGURE_LINES[i].direction);
        ck_assert(figure.lines[i].length == SAMPLE_FIGURE_LINES[i].length);
    }

    // cleanup
    sxbp_free_buffer(&buffer);
    sxbp_free_figure(&figure);
} END_TEST

// TODO: add test for sxbp_begin_figure() with options.max_lines overridden

START_TEST(test_begin_figure_data_too_big) {
    // create a buffer that is larger than SXBP_BEGIN_BUFFER_MAX_SIZE
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    buffer.size = SXBP_BEGIN_BUFFER_MAX_SIZE + 1;
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&buffer) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(&buffer, NULL, &figure);

    // precondition check error should be returned when data is too big
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);

    // cleanup
    sxbp_free_buffer(&buffer);
} END_TEST

START_TEST(test_begin_figure_data_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(NULL, NULL, &figure);

    // precondition check error should be returned when data is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_begin_figure_figure_null) {
    // create a buffer that
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    buffer.size = 100;
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&buffer) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }

    sxbp_result_t result = sxbp_begin_figure(&buffer, NULL, NULL);

    // precondition check error should be returned when figure is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);

    // cleanup
    sxbp_free_buffer(&buffer);
} END_TEST

START_TEST(test_refine_figure) {
    sxbp_figure_t figure = {
        .size = SAMPLE_FIGURE_SIZE,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with our pre-built lines
    for (size_t i = 0; i < figure.size; i++) {
        figure.lines[i] = SAMPLE_FIGURE_LINES[i];
    }

    sxbp_result_t result = sxbp_refine_figure(&figure, NULL);

    // check the operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    /*
     * check that the refined figure has the same number of lines and that these
     * lines are identical to those of the refined version
     */
    ck_assert(figure.size == SAMPLE_FIGURE_SIZE);
    for (size_t i = 0; i < SAMPLE_FIGURE_SIZE; i++) {
        ck_assert(
            figure.lines[i].direction ==
            REFINED_SAMPLE_FIGURE_LINES[i].direction
        );
        ck_assert(
            figure.lines[i].length == REFINED_SAMPLE_FIGURE_LINES[i].length
        );
    }

    // cleanup
    sxbp_free_figure(&figure);
} END_TEST

START_TEST(test_refine_figure_figure_null) {
    sxbp_result_t result = sxbp_refine_figure(NULL, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_refine_figure_no_lines) {
    // create a figure with no lines
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_refine_figure(&figure, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_refine_figure_unimplemented_method) {
    // make a figure with some lines, but we don't care what they are
    sxbp_figure_t figure = {
        .size = 100,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // specify an unimplemented refinement method
    sxbp_refine_figure_options_t options = {
        .refine_method = SXBP_REFINE_METHOD_RESERVED_END,
        .progress_callback = NULL,
        .callback_context = NULL,
    };

    sxbp_result_t result = sxbp_refine_figure(&figure, &options);

    // check that the return code was a not-implemented error
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);

    // cleanup
    sxbp_free_figure(&figure);
} END_TEST

START_TEST(test_dump_figure) {
    sxbp_figure_t figure = {
        .size = SAMPLE_FIGURE_SIZE,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with our pre-built lines
    for (size_t i = 0; i < figure.size; i++) {
        figure.lines[i] = SAMPLE_FIGURE_LINES[i];
    }
    // we'll try and dump the figure into here
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_dump_figure(&figure, &buffer);

    // check that the operation completed successfully
    ck_assert(result == SXBP_RESULT_OK);
    // check size is as expected
    ck_assert(buffer.size == sizeof(SAMPLE_SXBP_FILE_DATA));
    // check that the contents of the buffer are as expected
    ck_assert_mem_eq(buffer.bytes, SAMPLE_SXBP_FILE_DATA, buffer.size);

    // cleanup
    sxbp_free_figure(&figure);
    sxbp_free_buffer(&buffer);
} END_TEST

START_TEST(test_dump_figure_figure_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_dump_figure(NULL, &buffer);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_dump_figure_buffer_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_dump_figure(&figure, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_load_figure) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    buffer.size = sizeof(SAMPLE_SXBP_FILE_DATA);
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&buffer) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // copy the data into the buffer
    for (size_t i = 0; i < buffer.size; i++) {
        buffer.bytes[i] = SAMPLE_SXBP_FILE_DATA[i];
    }
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_load_figure(&buffer, &figure);

    // check the operation completed successfully
    ck_assert(result == SXBP_RESULT_OK);
    // check that the figure contains the sample lines and quantity thereof
    ck_assert(figure.size == SAMPLE_FIGURE_SIZE);
    for (size_t i = 0; i < SAMPLE_FIGURE_SIZE; i++) {
        ck_assert(figure.lines[i].direction == SAMPLE_FIGURE_LINES[i].direction);
        ck_assert(figure.lines[i].length == SAMPLE_FIGURE_LINES[i].length);
    }

    // cleanup
    sxbp_free_buffer(&buffer);
    sxbp_free_figure(&figure);
} END_TEST

START_TEST(test_load_figure_buffer_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_load_figure(NULL, &figure);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_load_figure_figure_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_load_figure(&buffer, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

/*
 * TODO: add further tests for:
 * - invalid data due to size
 * - invalid data due to structure
 * - invalid data due to being compatible with an older version of sxbp only
 */

// dummy struct type to test passing of custom options in sxbp_render_figure()
struct test_render_figure_custom_options { const char* foo; };

// global variables for test_render_figure test case
static sxbp_figure_t test_render_figure_figure;
static sxbp_buffer_t test_render_figure_buffer;
static sxbp_render_options_t test_render_figure_render_options;
static struct test_render_figure_custom_options test_render_figure_custom_options = {
    .foo = "bar",
};

/*
 * a dummy renderer backend which doesn't render anything at all, but which
 * asserts that its arguments match the global variables declared above
 */
static sxbp_result_t unit_test_renderer_backend(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    // given arguments should match the global variables above
    ck_assert_ptr_eq(figure, &test_render_figure_figure);
    ck_assert_ptr_eq(buffer, &test_render_figure_buffer);
    ck_assert_ptr_eq(render_options, &test_render_figure_render_options);
    ck_assert_ptr_eq(render_callback_options, &test_render_figure_custom_options);
    // always return success error code
    return SXBP_RESULT_OK;
}

START_TEST(test_render_figure) {
    test_render_figure_figure.size = SAMPLE_FIGURE_SIZE;
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&test_render_figure_figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with our pre-built lines
    for (size_t i = 0; i < test_render_figure_figure.size; i++) {
        test_render_figure_figure.lines[i] = SAMPLE_FIGURE_LINES[i];
    }
    // set buffer and render options to blank/defaults
    test_render_figure_buffer = sxbp_blank_buffer();
    test_render_figure_render_options.scale = 1;

    sxbp_result_t result = sxbp_render_figure(
        &test_render_figure_figure,
        &test_render_figure_buffer,
        unit_test_renderer_backend,
        &test_render_figure_render_options,
        (void*)&test_render_figure_custom_options
    );

    // check the operation was successful
    ck_assert(result == SXBP_RESULT_OK);

    // cleanup
    sxbp_free_figure(&test_render_figure_figure);
    sxbp_free_buffer(&test_render_figure_buffer);
} END_TEST

START_TEST(test_render_figure_figure_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_render_figure(
        NULL,
        &buffer,
        sxbp_render_figure_to_null, // just pass it the dummy backend
        NULL,
        NULL
    );

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_buffer_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_render_figure(
        &figure,
        NULL,
        sxbp_render_figure_to_null, // just pass it the dummy backend
        NULL,
        NULL
    );

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_render_callback_null) {
    sxbp_figure_t figure = sxbp_blank_figure();
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_render_figure(
        &figure,
        &buffer,
        NULL, // make sure to not pass any render backend
        NULL,
        NULL
    );

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_to_null) {
    // this function doesn't care if no objects are passed to it
    sxbp_result_t result = sxbp_render_figure_to_null(NULL, NULL, NULL, NULL);

    // this function should always return the not-implemented error code
    ck_assert(result == SXBP_RESULT_FAIL_UNIMPLEMENTED);
} END_TEST

START_TEST(test_render_figure_to_pbm) {
    sxbp_figure_t figure = {
        .size = SAMPLE_FIGURE_SIZE,
        .lines = NULL,
        .lines_remaining = 0,
    };
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with our pre-built lines
    for (size_t i = 0; i < figure.size; i++) {
        figure.lines[i] = REFINED_SAMPLE_FIGURE_LINES[i];
    }
    // the buffer we'll render the PBM file into
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    // read into another buffer the expected PBM file data
    sxbp_buffer_t expected = sxbp_blank_buffer();
    expected.size = sizeof(SAMPLE_PBM_FILE_DATA);
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&expected) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // copy the data into the buffer
    for (size_t i = 0; i < expected.size; i++) {
        expected.bytes[i] = SAMPLE_PBM_FILE_DATA[i];
    }

    // render the figure to PBM
    sxbp_result_t result = sxbp_render_figure_to_pbm(
        &figure,
        &buffer,
        NULL,
        NULL
    );

    // check that the operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // compare the buffer with the expected buffer
    ck_assert(buffer.size == expected.size);
    ck_assert_mem_eq(buffer.bytes, expected.bytes, buffer.size);

    // cleanup
    sxbp_free_figure(&figure);
    sxbp_free_buffer(&buffer);
    sxbp_free_buffer(&expected);
} END_TEST

START_TEST(test_render_figure_to_pbm_figure_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_render_figure_to_pbm(NULL, &buffer, NULL, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_to_pbm_buffer_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_render_figure_to_pbm(&figure, NULL, NULL, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_to_svg) {
    sxbp_figure_t figure = {
        .size = SAMPLE_FIGURE_SIZE,
        .lines = NULL,
        .lines_remaining = 0,
    };
    sxbp_init_figure(&figure);
    /*
     * allocate the figure -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_figure(&figure) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate figure");
    }
    // populate the figure with our pre-built lines
    for (size_t i = 0; i < figure.size; i++) {
        figure.lines[i] = REFINED_SAMPLE_FIGURE_LINES[i];
    }
    // the buffer we'll render the SVG file into
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    // read into another buffer the expected SVG file data
    sxbp_buffer_t expected = sxbp_blank_buffer();
    expected.size = sizeof(SAMPLE_SVG_FILE_DATA);
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&expected) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // copy the data into the buffer
    for (size_t i = 0; i < expected.size; i++) {
        expected.bytes[i] = SAMPLE_SVG_FILE_DATA[i];
    }

    // render the figure to SVG
    sxbp_result_t result = sxbp_render_figure_to_svg(
        &figure,
        &buffer,
        NULL,
        NULL
    );

    // check that the operation was successful
    ck_assert(result == SXBP_RESULT_OK);
    // compare the buffer with the expected buffer
    ck_assert(buffer.size == expected.size);
    ck_assert_mem_eq(buffer.bytes, expected.bytes, buffer.size);

    // cleanup
    sxbp_free_figure(&figure);
    sxbp_free_buffer(&buffer);
    sxbp_free_buffer(&expected);
} END_TEST

START_TEST(test_render_figure_to_svg_figure_null) {
    sxbp_buffer_t buffer = sxbp_blank_buffer();

    sxbp_result_t result = sxbp_render_figure_to_svg(NULL, &buffer, NULL, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_render_figure_to_svg_buffer_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_render_figure_to_svg(&figure, NULL, NULL, NULL);

    // check that the return code was a precondition check error
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

Suite* make_figure_suite(void) {
    // Test cases for figure data type
    Suite* test_suite = suite_create("Figure");

    TCase* blank_figure = tcase_create("sxbp_blank_figure()");
    tcase_add_test(blank_figure, test_blank_figure);
    suite_add_tcase(test_suite, blank_figure);

    TCase* init_figure = tcase_create("sxbp_init_figure()");
    tcase_add_test(init_figure, test_init_figure);
    tcase_add_test(init_figure, test_init_figure_null);
    tcase_add_test(init_figure, test_init_figure_blank);
    suite_add_tcase(test_suite, init_figure);

    TCase* free_figure = tcase_create("sxbp_free_figure()");
    tcase_add_test(free_figure, test_free_figure_unallocated);
    tcase_add_test(free_figure, test_free_figure_allocated);
    suite_add_tcase(test_suite, free_figure);

    TCase* copy_figure = tcase_create("sxbp_copy_figure()");
    tcase_add_test(copy_figure, test_copy_figure);
    tcase_add_test(copy_figure, test_copy_figure_from_null);
    tcase_add_test(copy_figure, test_copy_figure_to_null);
    tcase_add_test(copy_figure, test_copy_figure_blank);
    tcase_add_test(copy_figure, test_copy_figure_lines_null);
    suite_add_tcase(test_suite, copy_figure);

    TCase* begin_figure = tcase_create("sxbp_begin_figure()");
    tcase_add_test(begin_figure, test_begin_figure);
    tcase_add_test(begin_figure, test_begin_figure_data_too_big);
    tcase_add_test(begin_figure, test_begin_figure_data_null);
    tcase_add_test(begin_figure, test_begin_figure_figure_null);
    suite_add_tcase(test_suite, begin_figure);

    TCase* refine_figure = tcase_create("sxbp_refine_figure()");
    tcase_add_test(refine_figure, test_refine_figure);
    tcase_add_test(refine_figure, test_refine_figure_figure_null);
    tcase_add_test(refine_figure, test_refine_figure_no_lines);
    tcase_add_test(refine_figure, test_refine_figure_unimplemented_method);
    suite_add_tcase(test_suite, refine_figure);

    TCase* dump_figure = tcase_create("sxbp_dump_figure()");
    tcase_add_test(dump_figure, test_dump_figure);
    tcase_add_test(dump_figure, test_dump_figure_figure_null);
    tcase_add_test(dump_figure, test_dump_figure_buffer_null);
    suite_add_tcase(test_suite, dump_figure);

    TCase* load_figure = tcase_create("sxbp_load_figure()");
    tcase_add_test(load_figure, test_load_figure);
    tcase_add_test(load_figure, test_load_figure_buffer_null);
    tcase_add_test(load_figure, test_load_figure_figure_null);
    suite_add_tcase(test_suite, load_figure);

    TCase* render_figure = tcase_create("sxbp_render_figure()");
    tcase_add_test(render_figure, test_render_figure);
    tcase_add_test(render_figure, test_render_figure_figure_null);
    tcase_add_test(render_figure, test_render_figure_buffer_null);
    tcase_add_test(render_figure, test_render_figure_render_callback_null);
    suite_add_tcase(test_suite, render_figure);

    TCase* render_figure_to_null = tcase_create("sxbp_render_figure_to_null()");
    tcase_add_test(render_figure_to_null, test_render_figure_to_null);
    suite_add_tcase(test_suite, render_figure_to_null);

    TCase* render_figure_to_pbm = tcase_create("sxbp_render_figure_to_pbm()");
    tcase_add_test(render_figure_to_pbm, test_render_figure_to_pbm);
    tcase_add_test(render_figure_to_pbm, test_render_figure_to_pbm_figure_null);
    tcase_add_test(render_figure_to_pbm, test_render_figure_to_pbm_buffer_null);
    suite_add_tcase(test_suite, render_figure_to_pbm);

    TCase* render_figure_to_svg = tcase_create("sxbp_render_figure_to_svg()");
    tcase_add_test(render_figure_to_svg, test_render_figure_to_svg);
    tcase_add_test(render_figure_to_svg, test_render_figure_to_svg_figure_null);
    tcase_add_test(render_figure_to_svg, test_render_figure_to_svg_buffer_null);
    suite_add_tcase(test_suite, render_figure_to_svg);

    return test_suite;
}
