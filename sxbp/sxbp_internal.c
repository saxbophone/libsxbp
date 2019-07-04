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
#include <stdint.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

const sxbp_Vector SXBP_VECTOR_DIRECTIONS[4] = {
    [SXBP_UP]    = {  0,  1, },
    [SXBP_RIGHT] = {  1,  0, },
    [SXBP_DOWN]  = {  0, -1, },
    [SXBP_LEFT]  = { -1,  0, },
};

void sxbp_update_bounds(sxbp_CoOrd location, sxbp_Bounds* bounds) {
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
    sxbp_CoOrd* location,
    sxbp_Direction direction,
    sxbp_Length length
) {
    // preconditional assertions
    assert(location != NULL);
    sxbp_Vector vector = SXBP_VECTOR_DIRECTIONS[direction];
    location->x += vector.x * (sxbp_TupleItem)length;
    location->y += vector.y * (sxbp_TupleItem)length;
}

void sxbp_move_location_along_line(
    sxbp_CoOrd* location,
    sxbp_Line line
) {
    // preconditional assertions
    assert(location != NULL);
    sxbp_move_location(location, line.direction, line.length);
}

sxbp_Bounds sxbp_get_bounds(const sxbp_Figure* figure, size_t scale) {
    // preconditional assertions
    assert(figure != NULL);
    // loop state variables
    sxbp_CoOrd location = { 0 }; // where the end of the last line is
    sxbp_Bounds bounds = { 0 }; // the bounds of the line walked so far
    // walk the line!
    for (sxbp_FigureSize i = 0; i < figure->size; i++) {
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

sxbp_CoOrd sxbp_get_origin_from_bounds(const sxbp_Bounds bounds) {
    /*
     * the origin (also known as the transformation vector) for all coördinates
     * is the negative of the minimum bounds of the line
     * starting the line at this non-negative point ensures that all coördinates
     * of the line are positive
     */
    return (sxbp_CoOrd){
        .x = -bounds.x_min,
        .y = -bounds.y_min,
    };
}

void sxbp_walk_figure(
    const sxbp_Figure* figure,
    size_t scale,
    bool plot_vertices_only,
    bool( *plot_point_callback)(sxbp_CoOrd location, void* callback_data),
    void* callback_data
) {
    // preconditional assertions
    assert(figure != NULL);
    assert(plot_point_callback != NULL);
    // get figure's bounds
    sxbp_Bounds bounds = sxbp_get_bounds(figure, scale);
    // start the line at the origin
    sxbp_CoOrd location = sxbp_get_origin_from_bounds(bounds);
    // plot the first point of the line, if callback returned false then exit
    if (!plot_point_callback(location, callback_data)) {
        return;
    }
    // for each line, plot one or more points along it (depending on plot mode)
    for (sxbp_FigureSize i = 0; i < figure->size; i++) {
        sxbp_Line line = figure->lines[i];
        // scale the line's size
        sxbp_Length length = line.length * scale;
        // if plotting vertices only, plot one point at the end of this line
        if (plot_vertices_only) {
            // move the location length amount of units
            sxbp_move_location(&location, line.direction, length);
            // plot a point, if callback returned false then exit
            if (!plot_point_callback(location, callback_data)) {
                return;
            }
        } else { // otherwise, plot one point along each one unit of line length
            for (sxbp_Length l = 0; l < length; l++) {
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
    const sxbp_Bounds bounds,
    sxbp_FigureDimension* restrict width,
    sxbp_FigureDimension* restrict height
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
    *width = (sxbp_FigureDimension)((bounds.x_max - bounds.x_min) + 1);
    *height = (sxbp_FigureDimension)((bounds.y_max - bounds.y_min) + 1);
}

bool sxbp_dimension_to_string(
    sxbp_FigureDimension dimension,
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

sxbp_Result sxbp_make_bitmap_for_bounds(
    const sxbp_Bounds bounds,
    sxbp_Bitmap* bitmap
) {
    // preconditional assertions
    assert(bitmap != NULL);
    // calculate the width and height
    sxbp_get_size_from_bounds(bounds, &bitmap->width, &bitmap->height);
    bitmap->pixels = NULL;
    // allocate memory for the bitmap and return the status of this operation
    return sxbp_init_bitmap(bitmap);
}

void sxbp_print_bitmap(sxbp_Bitmap* bitmap, FILE* stream) {
    for (sxbp_FigureSize y = 0; y < bitmap->height; y++) {
        for (sxbp_FigureSize x = 0; x < bitmap->width; x++) {
            fprintf(stream, bitmap->pixels[x][y] ? "█" : "░");
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");
}
