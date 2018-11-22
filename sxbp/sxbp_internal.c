/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @internal
 * @file
 *
 * @brief This source file provides internal definitions for use within sxbp
 * only
 *
 * @author Joshua Saxby <joshua.a.saxby@gmail.com>
 * @date 2018
 *
 * @copyright Copyright (C) Joshua Saxby 2016-2017, 2018
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4] = {
    [SXBP_UP]    = {  0,  1, },
    [SXBP_RIGHT] = {  1,  0, },
    [SXBP_DOWN]  = {  0, -1, },
    [SXBP_LEFT]  = { -1,  0, },
};

void sxbp_update_bounds(sxbp_co_ord_t location, sxbp_bounds_t* bounds) {
    if (location.x > bounds->x_max) {
        bounds->x_max = location.x;
    } else if (location.x < bounds->x_min) {
        bounds->x_min = location.x;
    }
    if (location.y > bounds->y_max) {
        bounds->y_max = location.y;
    } else if (location.y < bounds->y_min) {
        bounds->y_min = location.y;
    }
}

void sxbp_move_location(
    sxbp_co_ord_t* location,
    sxbp_direction_t direction,
    sxbp_length_t length
) {
    sxbp_vector_t vector = SXBP_VECTOR_DIRECTIONS[direction];
    location->x += vector.x * (sxbp_tuple_item_t)length;
    location->y += vector.y * (sxbp_tuple_item_t)length;
}

void sxbp_move_location_along_line(
    sxbp_co_ord_t* location,
    sxbp_line_t line
) {
    sxbp_move_location(location, line.direction, line.length);
}

sxbp_bounds_t sxbp_get_bounds(const sxbp_figure_t* figure, size_t scale) {
    // loop state variables
    sxbp_co_ord_t location = { 0 }; // where the end of the last line is
    sxbp_bounds_t bounds = { 0 }; // the bounds of the line walked so far
    // walk the line!
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        // update the location, scaling in proportion to scale
        sxbp_move_location(
            &location,
            figure->lines[i].direction,
            figure->lines[i].length * scale
        );
        // update the bounds
        sxbp_update_bounds(location, &bounds);
    }
    // now all lines have been walked, return the bounds
    return bounds;
}

sxbp_co_ord_t sxbp_get_origin_from_bounds(const sxbp_bounds_t bounds) {
    /*
     * the origin (also known as the transformation vector) for all coördinates
     * is the negative of the minimum bounds of the line
     * starting the line at this non-negative point ensures that all coördinates
     * of the line are positive
     */
    return (sxbp_co_ord_t){
        .x = -bounds.x_min,
        .y = -bounds.y_min,
    };
}

void sxbp_walk_figure(
    const sxbp_figure_t* figure,
    size_t scale,
    bool( *plot_point_callback)(sxbp_co_ord_t location, void* callback_data),
    void* callback_data
) {
    // preconditional assertions
    assert(plot_point_callback != NULL);
    // get figure's bounds
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, scale);
    // start the line at the origin
    sxbp_co_ord_t location = sxbp_get_origin_from_bounds(bounds);
    // plot the first point of the line, if callback returned false then exit
    if (!plot_point_callback(location, callback_data)) {
        return;
    }
    // for each line, plot separate points along their length
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        sxbp_line_t line = figure->lines[i];
        // scale the line's size
        for (sxbp_figure_size_t l = 0; l < line.length * scale; l++) {
            // move the location
            sxbp_move_location(&location, line.direction, 1);
            // plot a point, if callback returned false then exit
            if (!plot_point_callback(location, callback_data)) {
                return;
            }
        }
    }
}

void sxbp_get_size_from_bounds(
    const sxbp_bounds_t bounds,
    sxbp_figure_dimension_t* restrict width,
    sxbp_figure_dimension_t* restrict height
) {
    // pointer arguments must not be NULL!
    assert(width != NULL);
    assert(height != NULL);
    /*
     * the width and height are the difference of the max and min dimensions
     * + 1.
     * this makes sense because for example from 1 to 10 there are 10 values
     * and the difference of these is 9 so the number of values is 9+1 = 10
     */
    *width = (sxbp_figure_dimension_t)((bounds.x_max - bounds.x_min) + 1);
    *height = (sxbp_figure_dimension_t)((bounds.y_max - bounds.y_min) + 1);
}

sxbp_result_t sxbp_stringify_dimensions(
    sxbp_figure_dimension_t width,
    sxbp_figure_dimension_t height,
    char(* width_string)[11],
    char(* height_string)[11],
    size_t* width_string_length,
    size_t* height_string_length
) {
    // we'll store the return values of two snprintf() calls in these variables
    int width_string_result, height_string_result = 0;
    // convert width and height to a decimal string, check for errors
    width_string_result = snprintf(
        *width_string, 11, "%" PRIu32, width
    );
    height_string_result = snprintf(
        *height_string, 11, "%" PRIu32, height
    );
    if (width_string_result < 0 || height_string_result < 0) {
        // snprintf() returns negative values when it fails, so return an error
        return SXBP_RESULT_FAIL_IO;
    } else {
        // store lengths from snprintf() return values
        *width_string_length = (size_t)width_string_result;
        *height_string_length = (size_t)height_string_result;
        // return success
        return SXBP_RESULT_OK;
    }
}

sxbp_result_t sxbp_make_bitmap_for_bounds(
    const sxbp_bounds_t bounds,
    sxbp_bitmap_t* bitmap
) {
    // calculate the width and height
    sxbp_get_size_from_bounds(bounds, &bitmap->width, &bitmap->height);
    bitmap->pixels = NULL;
    // allocate memory for the bitmap and return the status of this operation
    return sxbp_init_bitmap(bitmap);
}

void sxbp_print_bitmap(sxbp_bitmap_t* bitmap, FILE* stream) {
    for (sxbp_figure_size_t y = 0; y < bitmap->height; y++) {
        for (sxbp_figure_size_t x = 0; x < bitmap->width; x++) {
            fprintf(stream, bitmap->pixels[x][y] ? "█" : "░");
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");
}

#ifdef __cplusplus
} // extern "C"
#endif
