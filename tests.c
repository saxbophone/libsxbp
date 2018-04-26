/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "sxbp/sxbp.h"
#include "sxbp/sxbp_internal.h"


#ifdef __cplusplus
extern "C"{
#endif


/*
 * disable GCC warning about the unused parameter, as this is a callback it must
 * include all arguments specified by the `progress_callback`, even if not used
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void print_progress(const sxbp_figure_t* figure, void* context) {
    printf("%" PRIu32 "\n", figure->lines_remaining);
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
    sxbp_render_figure(figure, &bitmap);
    sxbp_print_bitmap(&bitmap, stdout);
    // free the memory, be a good person!
    sxbp_free_bitmap(&bitmap);
    fflush(stdout);
}
// reënable all warnings
#pragma GCC diagnostic pop


int main(void) {
    printf("This is SXBP v%s\n", SXBP_VERSION.string);
    // test that the public API is resistant to NULL-pointer-deref errors
    assert(sxbp_init_buffer(NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_free_buffer(NULL) == false);
    assert(sxbp_copy_buffer(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_buffer_from_file(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_buffer_to_file(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_init_figure(NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_free_figure(NULL) == false);
    assert(sxbp_copy_figure(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_init_bitmap(NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_free_bitmap(NULL) == false);
    assert(sxbp_copy_bitmap(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_begin_figure(NULL, NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_refine_figure(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_dump_figure(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_load_figure(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    assert(sxbp_render_figure(NULL, NULL) == SXBP_RESULT_FAIL_PRECONDITION);
    // now test normal usage of the public API
    const char* string = "SXBP";
    size_t length = strlen(string);
    sxbp_buffer_t buffer = { .size = length, .bytes = NULL, };
    if (!sxbp_init_buffer(&buffer)) {
        return -1;
    } else {
        memcpy(buffer.bytes, string, length);
        sxbp_figure_t figure = sxbp_blank_figure();
        sxbp_begin_figure(&buffer, NULL, &figure);
        sxbp_free_buffer(&buffer);
        // render incomplete figure to bitmap
        sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
        sxbp_render_figure(&figure, &bitmap);
        sxbp_refine_figure_options_t options = {
            .progress_callback = print_progress,
        };
        assert(sxbp_refine_figure(&figure, &options) == SXBP_RESULT_OK);
        // render complete figure to bitmap
        sxbp_render_figure(&figure, &bitmap);
        sxbp_free_figure(&figure);
        sxbp_free_bitmap(&bitmap);
        return 0;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
