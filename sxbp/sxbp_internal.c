/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This header file provides internal definitions for use within sxbp
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
    location->x += vector.x * length;
    location->y += vector.y * length;
}

void sxbp_move_location_along_line(
    sxbp_co_ord_t* location,
    sxbp_line_t line
) {
    sxbp_move_location(location, line.direction, line.length);
}

sxbp_bounds_t sxbp_get_bounds(const sxbp_figure_t* figure) {
    // loop state variables
    sxbp_co_ord_t location = { 0 }; // where the end of the last line is
    sxbp_bounds_t bounds = { 0 }; // the bounds of the line walked so far
    // walk the line!
    for (uint32_t i = 0; i < figure->size; i++) {
        // update the location
        sxbp_move_location_along_line(&location, figure->lines[i]);
        // update the bounds
        sxbp_update_bounds(location, &bounds);
    }
    // now all lines have been walked, return the bounds
    return bounds;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
void sxbp_walk_figure(
    const sxbp_figure_t* figure,
    size_t scale,
    void(* plot_point_callback)(uint32_t x, uint32_t y, void* callback_data),
    void* callback_data
) {
    // preconditional assertions
    assert(plot_point_callback != NULL);
    // get figure's bounds
    sxbp_bounds_t bounds = sxbp_get_bounds(figure);
}
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

void sxbp_print_bitmap(sxbp_bitmap_t* bitmap, FILE* stream) {
    for (uint32_t x = 0; x < bitmap->width; x++) {
        for (uint32_t y = 0; y < bitmap->height; y++) {
            fprintf(stream, bitmap->pixels[x][bitmap->height - y] ? "█" : "░");
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "\n");
}

#ifdef __cplusplus
} // extern "C"
#endif
