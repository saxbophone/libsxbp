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
 * @copyright Copyright (C) Joshua Saxby 2016-2017, 2018-2019
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4] = {
    [SXBP_UP]    = {  0,  1, },
    [SXBP_RIGHT] = {  1,  0, },
    [SXBP_DOWN]  = {  0, -1, },
    [SXBP_LEFT]  = { -1,  0, },
};

void sxbp_update_bounds(sxbp_co_ord_t location, sxbp_bounds_t* bounds) {
    // preconditional assertions
    assert(bounds != NULL);
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
    // preconditional assertions
    assert(location != NULL);
    sxbp_vector_t vector = SXBP_VECTOR_DIRECTIONS[direction];
    location->x += vector.x * (sxbp_tuple_item_t)length;
    location->y += vector.y * (sxbp_tuple_item_t)length;
}

void sxbp_move_location_along_line(
    sxbp_co_ord_t* location,
    sxbp_line_t line
) {
    // preconditional assertions
    assert(location != NULL);
    sxbp_move_location(location, line.direction, line.length);
}

sxbp_bounds_t sxbp_get_bounds(const sxbp_figure_t* figure, size_t scale) {
    // preconditional assertions
    assert(figure != NULL);
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
    bool plot_vertices_only,
    bool( *plot_point_callback)(sxbp_co_ord_t location, void* callback_data),
    void* callback_data
) {
    // preconditional assertions
    assert(figure != NULL);
    assert(plot_point_callback != NULL);
    // get figure's bounds
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, scale);
    // start the line at the origin
    sxbp_co_ord_t location = sxbp_get_origin_from_bounds(bounds);
    // plot the first point of the line, if callback returned false then exit
    if (!plot_point_callback(location, callback_data)) {
        return;
    }
    // for each line, plot one or more points along it (depending on plot mode)
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        sxbp_line_t line = figure->lines[i];
        // scale the line's size
        sxbp_length_t length = line.length * scale;
        // if plotting vertices only, plot one point at the end of this line
        if (plot_vertices_only) {
            // move the location length amount of units
            sxbp_move_location(&location, line.direction, length);
            // plot a point, if callback returned false then exit
            if (!plot_point_callback(location, callback_data)) {
                return;
            }
        } else { // otherwise, plot one point along each one unit of line length
            for (sxbp_length_t l = 0; l < length; l++) {
                // move the location one unit
                sxbp_move_location(&location, line.direction, 1);
                // plot a point, if callback returned false then exit
                if (!plot_point_callback(location, callback_data)) {
                    return;
                }
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

bool sxbp_dimension_to_string(
    sxbp_figure_dimension_t dimension,
    char(* output_string)[11],
    size_t* string_length
) {
    // preconditional assertions
    assert(output_string != NULL);
    assert(string_length != NULL);
    // call snprintf() to convert to string and store the return code
    int result = snprintf(*output_string, 11, "%" PRIu32, dimension);
    // snprintf() returns negative values when it fails, so check this
    if (result < 0) {
        // indicate failure
        return false;
    } else {
        // otherwise, store the length of the resulting string to string_length
        *string_length = (size_t)result;
        // indicate success
        return true;
    }
}

sxbp_result_t sxbp_make_bitmap_for_bounds(
    const sxbp_bounds_t bounds,
    sxbp_bitmap_t* bitmap
) {
    // preconditional assertions
    assert(bitmap != NULL);
    // calculate the width and height
    sxbp_get_size_from_bounds(bounds, &bitmap->width, &bitmap->height);
    printf("w = %"PRIu32" h = %"PRIu32"\n", bitmap->width, bitmap->height);
    bitmap->pixels = NULL;
    // allocate memory for the bitmap and return the status of this operation
    return sxbp_init_bitmap(bitmap);
}

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct figure_collides_context {
    sxbp_bitmap_t* image;
    bool* collided;
} figure_collides_context;

// private, callback function for sxbp_figure_collides()
static bool sxbp_figure_collides_callback(sxbp_co_ord_t location, void* data) {
    // cast void pointer to a pointer to our context structure
    figure_collides_context* callback_data = (figure_collides_context*)data;
    // check if there's already a pixel here
    if (callback_data->image->pixels[location.x][location.y] == false) {
        // if not, plot it
        callback_data->image->pixels[location.x][location.y] = true;
        // return true to tell the walk function that we want to continue
        return true;
    } else {
        // otherwise, set collided to true to mark collision
        *callback_data->collided = true;
        // return false to tell the walk function to stop early
        return false;
    }
}

sxbp_result_t sxbp_figure_collides(
    const sxbp_figure_t* figure,
    bool* collided
) {
    // get figure bounds first
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 1);
    // build bitmap for bounds
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
    if (!sxbp_success(sxbp_make_bitmap_for_bounds(bounds, &bitmap))) {
        // a memory allocation error occurred
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // construct callback context data
        figure_collides_context data = {
            .image = &bitmap, .collided = collided,
        };
        // set collided to false initially
        *data.collided = false;
        // begin walking the figure, use our callback function to handle points
        sxbp_walk_figure(
            figure,
            1,
            false, // don't plot vertices only, we need all 1-unit sub-lines
            sxbp_figure_collides_callback, (void*)&data
        );
        // free the memory allocated for the bitmap
        sxbp_free_bitmap(&bitmap);
        return SXBP_RESULT_OK;
    }
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
