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
    bool* collided;
} figure_collides_context;

// private, callback function for sxbp_figure_collides()
static bool sxbp_figure_collides_callback(sxbp_CoOrd location, void* data) {
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

// private, sets collided to true if the figure's line collides with itself
sxbp_Result sxbp_figure_collides(
    const sxbp_Figure* figure,
    bool* collided
) {
    // get figure bounds first
    sxbp_Bounds bounds = sxbp_get_bounds(figure, 1);
    // build bitmap for bounds
    sxbp_Bitmap bitmap = sxbp_blank_bitmap();
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
