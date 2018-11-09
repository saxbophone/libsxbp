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
    printf("%" PRIu32 "|", figure->lines_remaining);
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
    assert(
        sxbp_render_figure_to_bitmap(NULL, NULL)
        == SXBP_RESULT_FAIL_PRECONDITION
    );
    assert(
        sxbp_render_figure(NULL, NULL, NULL, NULL, NULL)
        == SXBP_RESULT_FAIL_PRECONDITION
    );
    assert(
        sxbp_render_figure_to_null(NULL, NULL, NULL, NULL)
        == SXBP_RESULT_FAIL_UNIMPLEMENTED
    );
    assert(
        sxbp_render_figure_to_pbm(NULL, NULL, NULL, NULL)
        == SXBP_RESULT_FAIL_PRECONDITION
    );
    // now test normal usage of the public API
    const char* string = "sxbp";
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
        sxbp_render_figure_to_bitmap(&figure, &bitmap);
        sxbp_refine_figure_options_t options = {
            .progress_callback = print_progress,
            .refine_method = SXBP_REFINE_METHOD_GROW_FROM_START,
        };
        sxbp_result_t outcome = sxbp_refine_figure(&figure, &options);
        assert(outcome == SXBP_RESULT_OK);
        // test null renderer can be called
        sxbp_render_figure(
            &figure,
            &buffer,
            sxbp_render_figure_to_null,
            NULL,
            NULL
        );
        // render complete figure to bitmap
        sxbp_render_figure_to_bitmap(&figure, &bitmap);
        printf("\n");
        sxbp_print_bitmap(&bitmap, stdout);
        outcome = sxbp_render_figure(
            &figure,
            &buffer,
            sxbp_render_figure_to_svg,
            NULL,
            NULL
        );
        assert(outcome == SXBP_RESULT_OK);
        FILE* output_file = fopen("sxbp-test.svg", "wb");
        assert(output_file != NULL);
        outcome = sxbp_buffer_to_file(&buffer, output_file);
        assert(outcome == SXBP_RESULT_OK);
        fclose(output_file);
        sxbp_free_figure(&figure);
        sxbp_free_bitmap(&bitmap);
        sxbp_free_buffer(&buffer);
        return 0;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
