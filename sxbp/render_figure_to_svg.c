/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure_to_svg`
 * , a public function used to render an SXBP figure to an SVG file buffer.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <string.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

// private, given a figure and a buffer, writes out the SVG header to the buffer
static sxbp_result_t sxbp_write_svg_header(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer
) {
    sxbp_result_t error;
    // the fixed data that is always at the start of the file
    const char* head = (
        "<svg\n"
        "    xmlns=\"http://www.w3.org/2000/svg\"\n"
        "    viewBox=\"0 0 %s %s\"\n"
        "    style=\"background-color: white\"\n"
        ">\n"
        "    <rect\n"
        "        x=\"0\"\n"
        "        y=\"0\"\n"
        "        width=\"100%%\"\n"
        "        height=\"100%%\"\n"
        "        fill=\"white\"\n"
        "    />\n"
    );
    /*
     * because SVG is a vector-based format, this backend differs from the
     * others as we don't need to plot a bunch of pixels, instead we need to use
     * sxbp_walk_figure() to draw an SVG <polyline> element.
     * the setup process for this is similar to that in
     * `sxbp_render_figure_to_bitmap()` however, as the image we produce still
     * needs to have the same dimensions and scale.
     */
    // get figure bounds, at scale 2
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 2);
    // calculate width and height of the image from the bounds
    uint32_t width = 0;
    uint32_t height = 0;
    sxbp_get_size_from_bounds(bounds, &width, &height);
    char width_string[11], height_string[11];
    // we'll store the length of each string here
    size_t width_string_length, height_string_length = 0;
    // convert width and height to a decimal string, check for errors
    if (
        !sxbp_check(
            sxbp_stringify_dimensions(
                width,
                height,
                &width_string,
                &height_string,
                &width_string_length,
                &height_string_length
            ),
            &error
        )
    ) {
        // return error
        return error;
    } else {
        // work out how long the header needs to be
        buffer->size = (
            strlen(head) // the head template string
            - 6 // remove formatting codes
            // add the lengths of the width and height strings
            + width_string_length
            + height_string_length
        );
        // initialise the buffer
        if (!sxbp_check(sxbp_init_buffer(buffer), &error)) {
            return error;
        }
        // write the header to the buffer
        if (
            snprintf(
                (char*)buffer->bytes,
                buffer->size,
                head,
                width_string,
                height_string
            ) < 0
        ) {
            // snprintf() failed, so it's an I/O error
            return SXBP_RESULT_FAIL_IO;
        }
        // chop off the null-terminator at the end
        if (!sxbp_check(sxbp_resize_buffer(buffer, buffer->size - 1), &error)) {
            return error;
        }
        return SXBP_RESULT_OK;
    }
}

// private, given a buffer, writes out the end of the SVG file to the buffer
static sxbp_result_t sxbp_write_svg_tail(sxbp_buffer_t* const buffer) {
    /*
     * the 'tail' of the SVG images produced by libsxbp never change, the only
     * way this function can fail is if reallocating memory was refused
     */
    // the 'tail' (close polyline quote and tag, closing svg tag)
    const char* tail = (
        "\"\n"
        "    />\n"
        "</svg>\n"
    );
    size_t tail_length = strlen(tail);
    // any errors encountered will be stored here
    sxbp_result_t error;
    // try and reallocate memory to include the tail
    if (
        !sxbp_check(
            sxbp_resize_buffer(buffer, buffer->size + tail_length),
            &error
        )
    ) {
        // catch and return error
        return error;
    } else {
        // write the tail to the end of the buffer
        memcpy(buffer->bytes + (buffer->size - tail_length), tail, tail_length);
        // return success
        return SXBP_RESULT_OK;
    }
}

/*
 * disable GCC warning about unused parameters, as this dummy function doesn't
 * do anything with its arguments
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
sxbp_result_t sxbp_render_figure_to_svg(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    // figure, figure lines and buffer must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(figure->lines);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    // any errors encountered will be stored here
    sxbp_result_t error;
    // write image header, including everything up to the line's points
    if (!sxbp_check(sxbp_write_svg_header(figure, buffer), &error)) {
        // catch and return error
        return error;
    } else {
        // TODO: use sxbp_walk_figure() to write all the line's points
        // ...
        // write the image tail
        if (!sxbp_check(sxbp_write_svg_tail(buffer), &error)) {
            // catch and return error
            return error;
        } else {
            // success!
            return SXBP_RESULT_OK;
        }
    }
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
// reënable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
