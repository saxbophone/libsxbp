/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure_to_svg`,
 * a public function used to render an SXBP figure to an SVG file buffer.
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
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct write_polyline_context {
    sxbp_buffer_t* buffer; // the buffer to write out SVG code fragments to
    sxbp_figure_dimension_t height; // the height of the image being written
    size_t current_point; // the index of the point currently being rendered
    sxbp_result_t error; // any error conditions that occur will be stored here
} write_polyline_context;

// private, given a figure and a buffer, writes out the SVG header to the buffer
static sxbp_result_t sxbp_write_svg_head(
    sxbp_figure_dimension_t width,
    sxbp_figure_dimension_t height,
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
    char width_string[11], height_string[11];
    // we'll store the length of each string here
    size_t width_string_length, height_string_length = 0;
    // convert width and height to a decimal string, check for errors
    if (
        !sxbp_dimension_to_string(width, &width_string, &width_string_length) ||
        !sxbp_dimension_to_string(height, &height_string, &height_string_length)
    ) {
        // return I/O error code
        return SXBP_RESULT_FAIL_IO;
    }
    // work out how long the header needs to be
    buffer->size = (
        strlen(head) // the head template string
        - 6 // remove formatting codes
        // add the lengths of the width and height strings
        + width_string_length
        + height_string_length
        + 1 // NUL-terminator
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

/*
 * private, given a figure and a buffer, writes out the SVG code for the origin
 * dot of the figure to the buffer
 */
static sxbp_result_t sxbp_write_svg_body_origin_dot(
    const sxbp_figure_t* const figure,
    sxbp_figure_dimension_t height,
    sxbp_buffer_t* const buffer
) {
    // generate the code for the origin dot
    const char* origin_dot = (
        "    <rect\n"
        "        x=\"%s\"\n"
        "        y=\"%s\"\n"
        "        width=\"1\"\n"
        "        height=\"1\"\n"
        "        fill=\"black\"\n"
        "    />\n"
    );
    // get the coördinates of the origin dot
    sxbp_co_ord_t origin = sxbp_get_origin_from_bounds(
        sxbp_get_bounds(figure, 2)
    );
    sxbp_figure_dimension_t origin_x = (sxbp_figure_dimension_t)origin.x;
    // y coördinate is flipped
    sxbp_figure_dimension_t origin_y =
        height - 1 - (sxbp_figure_dimension_t)origin.y;
    char origin_x_str[11], origin_y_str[11];
    size_t origin_x_length, origin_y_length = 0;
    // stringify the origin dot x/y values
    if (
        !sxbp_dimension_to_string(origin_x, &origin_x_str, &origin_x_length) ||
        !sxbp_dimension_to_string(origin_y, &origin_y_str, &origin_y_length)
    ) {
        // return I/O error if failure
        return SXBP_RESULT_FAIL_IO;
    }
    // now work out how much to extend buffer by
    size_t extend_amount = (
        strlen(origin_dot) // size of string template
        - 4 // subtract formatting codes
        // add lengths of origin strings
        + origin_x_length
        + origin_y_length
        + 1 // NUL-terminator
    );
    // try and resize the buffer
    if (
        !sxbp_success(sxbp_resize_buffer(buffer, buffer->size + extend_amount))
    ) {
        // catch and return memory error
        return SXBP_RESULT_FAIL_MEMORY;
    }
    // write out origin dot code, checking for snprintf() error
    if (
        snprintf(
            (char*)buffer->bytes + (buffer->size - extend_amount),
            extend_amount,
            origin_dot,
            origin_x_str,
            origin_y_str
        ) < 0
    ) {
        // return I/O failure
        return SXBP_RESULT_FAIL_IO;
    }
    // chop off the null-terminator at the end
    if (!sxbp_success(sxbp_resize_buffer(buffer, buffer->size - 1))) {
        return SXBP_RESULT_FAIL_MEMORY;
    }
    // if we got here, we succeeded, so return success
    return SXBP_RESULT_OK;
}

// private, callback function for sxbp_write_svg_body_figure_line()
static bool sxbp_render_figure_to_bitmap_callback(
    sxbp_co_ord_t location,
    void* callback_data
) {
    // cast void pointer to a pointer to our context structure
    write_polyline_context* data =
        (write_polyline_context*)callback_data;
    // skip plotting the first and second line segments
    if (data->current_point >= 2) {
        // stringify this point's coördinates
        sxbp_figure_dimension_t x = (sxbp_figure_dimension_t)location.x;
        // y coördinate is flipped
        sxbp_figure_dimension_t y =
            data->height - 1 - (sxbp_figure_dimension_t)location.y;
        char x_str[11], y_str[11];
        size_t x_str_length, y_str_length = 0;
        if (
            !sxbp_dimension_to_string(x, &x_str, &x_str_length) ||
            !sxbp_dimension_to_string(y, &y_str, &y_str_length)
        ) {
            // if this fails, set error to I/O error and stop walk()-ing early
            data->error = SXBP_RESULT_FAIL_IO;
            return false;
        }
        const char* co_ord_template = "%s.5,%s.5 ";
        size_t extend_amount = (
            strlen(co_ord_template) // length of template string
            - 4 // subtract formatting codes
            // add lengths of coördinate strings
            + x_str_length
            + y_str_length
            + 1 // NUL-terminator
        );
        // de-reference buffer pointer for ease
        sxbp_buffer_t* buffer = data->buffer;
        // try and allocate more memory
        if (
            !sxbp_success(
                sxbp_resize_buffer(buffer, buffer->size + extend_amount)
            )
        ) {
            // set error to memory error and stop walk()-in early
            data->error = SXBP_RESULT_FAIL_MEMORY;
            return false;
        }
        // output string coördinates to buffer
        if (
            snprintf(
                (char*)buffer->bytes + (buffer->size - extend_amount),
                extend_amount,
                co_ord_template,
                x_str,
                y_str
            ) < 0
        ) {
            // if this fails, set error to I/O error and stop walk()-ing early
            data->error = SXBP_RESULT_FAIL_IO;
            return false;
        }
        // chop off null-terminator at the end
        if (!sxbp_success(sxbp_resize_buffer(buffer, buffer->size - 1))) {
            // set error to memory error and stop walk()-in early
            data->error = SXBP_RESULT_FAIL_MEMORY;
            return false;
        }
    }
    // increment point index
    data->current_point++;
    return true;
}

/*
 * private, given a figure and a buffer, writes out the SVG code for the
 * figure's line to the buffer
 */
static sxbp_result_t sxbp_write_svg_body_figure_line(
    const sxbp_figure_t* const figure,
    sxbp_figure_dimension_t height,
    sxbp_buffer_t* const buffer
) {
    const char* polyline_boilerplate = (
        "    <polyline\n"
        "        fill=\"none\"\n"
        "        stroke=\"black\"\n"
        "        stroke-width=\"1\"\n"
        "        stroke-linecap=\"square\"\n"
        "        stroke-linejoin=\"miter\"\n"
        "        points=\""
    );
    size_t polyline_boilerplate_length = strlen(polyline_boilerplate);
    // try and extend buffer
    if (
        !sxbp_success(
            sxbp_resize_buffer(
                buffer,
                buffer->size + polyline_boilerplate_length
            )
        )
    ) {
        // catch and return memory error
        return SXBP_RESULT_FAIL_MEMORY;
    }
    // write out polyline boilerplate
    memcpy(
        buffer->bytes + (buffer->size - polyline_boilerplate_length),
        polyline_boilerplate,
        polyline_boilerplate_length
    );
    // construct callback context data
    write_polyline_context data = {
        .buffer = buffer,
        .height = height,
        .current_point = 0,
        .error = SXBP_RESULT_OK, // assume no errors to start with
    };
    // write out polyline points
    sxbp_walk_figure(
        figure,
        2,
        sxbp_render_figure_to_bitmap_callback,
        (void*)&data
    );
    // chop off the extra space at the end
    if (!sxbp_success(sxbp_resize_buffer(buffer, buffer->size - 1))) {
        return SXBP_RESULT_FAIL_MEMORY;
    }
    // return the error condition stored in the callback context data
    return data.error;
}

/*
 * private, given a figure and a buffer, writes out the bulk of the SVG markup
 * to the buffer
 */
static sxbp_result_t sxbp_write_svg_body(
    const sxbp_figure_t* const figure,
    sxbp_figure_dimension_t height,
    sxbp_buffer_t* const buffer
) {
    // any errors encountered will be stored here
    sxbp_result_t error;
    // write the origin dot
    if (!sxbp_check(
        sxbp_write_svg_body_origin_dot(figure, height, buffer), &error)
    ) {
        // catch and return error
        return error;
    }
    // write the line
    if (!sxbp_check(
        sxbp_write_svg_body_figure_line(figure, height, buffer), &error)
    ) {
        // catch and return error
        return error;
    }
    // if we got here, the operation was successful
    return SXBP_RESULT_OK;
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
        // catch and return memory error
        return SXBP_RESULT_FAIL_MEMORY;
    }
    // write the tail to the end of the buffer
    memcpy(buffer->bytes + (buffer->size - tail_length), tail, tail_length);
    // return success
    return SXBP_RESULT_OK;
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
    sxbp_figure_dimension_t width = 0;
    sxbp_figure_dimension_t height = 0;
    sxbp_get_size_from_bounds(bounds, &width, &height);
    // write image head, including everything up to the line's points
    if (!sxbp_check(
        sxbp_write_svg_head(width, height, buffer), &error)
    ) {
        // catch and return error
        return error;
    }
    // write the image body
    if (!sxbp_check(sxbp_write_svg_body(figure, height, buffer), &error)) {
        // catch and return error
        return error;
    }
    // write the image tail
    if (!sxbp_check(sxbp_write_svg_tail(buffer), &error)) {
        // catch and return error
        return error;
    }
    // if we got here, the operation was successful
    return SXBP_RESULT_OK;
}
// reënable all warnings
#pragma GCC diagnostic pop
