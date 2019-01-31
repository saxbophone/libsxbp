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

START_TEST(test_begin_figure) {
    sxbp_buffer_t data = { .size = 1, .bytes = NULL, };
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the init function
     */
    if (sxbp_init_buffer(&data) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    // populate our one byte
    data.bytes[0] = 0x6DU;
    // this should generate a figure of 9 lines
    const size_t expected_size = 9;
    // this is the pattern of line directions and lengths we expect
    sxbp_line_t expected_lines[] = {
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
    // create a blank figure to write the created figure into
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(&data, NULL, &figure);

    // check the result was success
    ck_assert(result == SXBP_RESULT_OK);
    // check that the figure contains the expected lines and quantity thereof
    ck_assert(figure.size == expected_size);
    for (size_t i = 0; i < expected_size; i++) {
        ck_assert(figure.lines[i].direction == expected_lines[i].direction);
        ck_assert(figure.lines[i].length == expected_lines[i].length);
    }

    // cleanup
    sxbp_free_buffer(&data);
    sxbp_free_figure(&figure);
} END_TEST

// TODO: add test for sxbp_begin_figure() with options.max_lines overridden

START_TEST(test_begin_figure_data_too_big) {
    // create a buffer that is larger than SXBP_BEGIN_BUFFER_MAX_SIZE
    sxbp_buffer_t data = sxbp_blank_buffer();
    data.size = SXBP_BEGIN_BUFFER_MAX_SIZE + 1;
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&data) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(&data, NULL, &figure);

    // precondition check error should be returned when data is too big
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);

    // cleanup
    sxbp_free_buffer(&data);
} END_TEST

START_TEST(test_begin_figure_data_null) {
    sxbp_figure_t figure = sxbp_blank_figure();

    sxbp_result_t result = sxbp_begin_figure(NULL, NULL, &figure);

    // precondition check error should be returned when data is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);
} END_TEST

START_TEST(test_begin_figure_figure_null) {
    // create a buffer that
    sxbp_buffer_t data = sxbp_blank_buffer();
    data.size = 100;
    /*
     * allocate the buffer -if this fails then we'll abort here because this
     * test case is not testing the buffer's init function
     */
    if (sxbp_init_buffer(&data) != SXBP_RESULT_OK) {
        ck_abort_msg("Unable to allocate buffer");
    }

    sxbp_result_t result = sxbp_begin_figure(&data, NULL, NULL);

    // precondition check error should be returned when figure is NULL
    ck_assert(result == SXBP_RESULT_FAIL_PRECONDITION);

    // cleanup
    sxbp_free_buffer(&data);
} END_TEST

START_TEST(test_refine_figure) {
    /*
     * NOTE: This function may be rather hard to unit-test as it suffers from
     * the halting problem
     */
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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

START_TEST(test_render_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
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
    suite_add_tcase(test_suite, init_figure);

    TCase* free_figure = tcase_create("sxbp_free_figure()");
    tcase_add_test(free_figure, test_free_figure_unallocated);
    tcase_add_test(free_figure, test_free_figure_allocated);
    suite_add_tcase(test_suite, free_figure);

    TCase* copy_figure = tcase_create("sxbp_copy_figure()");
    tcase_add_test(copy_figure, test_copy_figure);
    tcase_add_test(copy_figure, test_copy_figure_from_null);
    tcase_add_test(copy_figure, test_copy_figure_to_null);
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
