/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_shrink_from_end`, a public function providing a specific
 * algorithm for refining a figure by attemptin to shrink all the lines from
 * their safe 'default' lengths (as plotted by `sxbp_begin_figure`) to the
 * shortest length possible, starting from the end and working backwards.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018-2019
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "figure_collides.h"
#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct figure_collides_context {
    sxbp_Bitmap* image;
    sxbp_CollisionResult* status;
    sxbp_CoOrd last_location;
} figure_collides_context;

// private, callback function for sxbp_figure_collides()
static bool sxbp_figure_collides_callback(sxbp_CoOrd location, void* data) {
    // cast void pointer to a pointer to our context structure
    figure_collides_context* callback_data = (figure_collides_context*)data;
    // set last_location to our current one
    callback_data->last_location = location;
    // check if there's already a pixel here
    if (callback_data->image->pixels[location.x][location.y] == false) {
        // if not, plot it
        callback_data->image->pixels[location.x][location.y] = true;
        // return true to tell the walk function that we want to continue
        return true;
    } else {
        // otherwise, set status to COLLIDES to mark collision
        *callback_data->status = SXBP_COLLISION_RESULT_COLLIDES;
        // return false to tell the walk function to stop early
        return false;
    }
}

// private, sets status to true if the figure's line collides with itself
sxbp_Result sxbp_figure_collides(
    const sxbp_Figure* figure,
    sxbp_CollisionResult* status
) {
    // get figure bounds
    sxbp_Bounds bounds = sxbp_get_bounds(figure, 1);
    // build bitmap for bounds
    sxbp_Bitmap bitmap = sxbp_blank_bitmap();
    if (!sxbp_success(sxbp_make_bitmap_for_bounds(bounds, &bitmap))) {
        // a memory allocation error occurred
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // construct callback context data
        figure_collides_context data = {
            .image = &bitmap, .status = status, .last_location = {0},
        };
        // assume it doesn't collide at all or terminate until proven otherwise
        *data.status = SXBP_COLLISION_RESULT_CONTINUES;
        // begin walking the figure, use our callback function to handle points
        sxbp_walk_figure(
            figure,
            1,
            false, // don't plot vertices only, we need all 1-unit sub-lines
            sxbp_figure_collides_callback, (void*)&data
        );
        // if it didn't collide, do an additional check to see if it terminates
        if (*data.status != SXBP_COLLISION_RESULT_COLLIDES) {
            // get the direction of the last line, so we know which axis to check
            sxbp_Direction last_direction = figure->lines[figure->size - 1].direction;
            bool neighbours[2] = {false};
            if (last_direction % 2U == 0U) { // even == UP, DOWN
                // if last direction is vertical, we check horizontal neighbours
                sxbp_TupleItem left = data.last_location.x + SXBP_VECTOR_DIRECTIONS[SXBP_LEFT].x;
                sxbp_TupleItem right = data.last_location.x + SXBP_VECTOR_DIRECTIONS[SXBP_RIGHT].x;
                if (left >= 0 && left < (sxbp_TupleItem)bitmap.width) {
                    neighbours[0] = bitmap.pixels[left][data.last_location.y];
                }
                if (right >= 0 && right < (sxbp_TupleItem)bitmap.width) {
                    neighbours[1] = bitmap.pixels[right][data.last_location.y];
                }
            } else { // odd = LEFT, RIGHT
                // if last direction is horizontal, we check vertical neighbours
                sxbp_TupleItem up = data.last_location.y + SXBP_VECTOR_DIRECTIONS[SXBP_UP].y;
                sxbp_TupleItem down = data.last_location.y + SXBP_VECTOR_DIRECTIONS[SXBP_DOWN].y;
                if (up >= 0 && up < (sxbp_TupleItem)bitmap.height) {
                    neighbours[0] = bitmap.pixels[data.last_location.x][up];
                }
                if (down >= 0 && down < (sxbp_TupleItem)bitmap.height) {
                    neighbours[1] = bitmap.pixels[data.last_location.x][down];
                }
            }
            if (neighbours[0] && neighbours[1]) {
                *data.status = SXBP_COLLISION_RESULT_TERMINATES;
            }
        }
        // free the memory allocated for the bitmap
        sxbp_free_bitmap(&bitmap);
        return SXBP_RESULT_OK;
    }
}
