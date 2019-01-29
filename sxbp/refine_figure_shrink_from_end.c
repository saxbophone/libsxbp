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
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

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

// private, sets collided to true if the figure's line collides with itself
static sxbp_result_t sxbp_figure_collides(
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

/*
 * private, attempts to shorten the line of the figure at index l
 * if it succeeds, it will call itself recursively for each line after l from
 * max to l, in that order.
 */
static sxbp_result_t sxbp_attempt_line_shorten(
    sxbp_figure_t* figure,
    const sxbp_figure_size_t l,
    const sxbp_figure_size_t max
) {
    sxbp_line_t* line = &figure->lines[l];
    // it only makes sense to try and shorten lines longer than 1
    if (line->length > 1) {
        // we'll need this later to check if we were actually able to shorten it
        sxbp_length_t original_length = line->length;
        // as an ambitious first step, set to 1 (try best case scenario first)
        line->length = 1;
        // check if it collided
        bool collided = false;
        // we'll store any errors encountered by this function here
        sxbp_result_t status = SXBP_RESULT_UNKNOWN;
        if (!sxbp_check(sxbp_figure_collides(figure, &collided), &status)) {
            // handle error
            return status;
        } else {
            /*
             * if that caused a collision, keep extending it until it no longer
             * collides (or we reach the original length)
             * --we can quit in that case as we already know it doesn't collide
             */
            while (line->length < original_length && collided) {
                line->length++;
                // check again if it colldes and handle any errors
                if (
                    !sxbp_check(
                        sxbp_figure_collides(figure, &collided), &status
                    )
                ) {
                    // handle error
                    return status;
                }
            }
            /*
             * at this point, the shape now no longer collides. now, we check to
             * see if we were able to shorten the line at all. If so, we then
             * try and shorten the lines after this one, in reverse order.
             */
            if (line->length < original_length) {
                // try and shorten other lines some more
                for (sxbp_figure_size_t i = max; i >= l; i--) {
                    // handle any errors returned by the call
                    if (
                        !sxbp_check(
                            sxbp_attempt_line_shorten(figure, i, max), &status
                        )
                    ) {
                        return status;
                    }
                }
            }
        }
    }
    return SXBP_RESULT_OK;
}

sxbp_result_t sxbp_refine_figure_shrink_from_end(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    // variable to store any errors in
    sxbp_result_t status = SXBP_RESULT_UNKNOWN;
    // iterate over lines backwards - we don't care about line 0
    for (sxbp_figure_size_t i = figure->size - 1; i > 0; i--) {
        // try and shorten it, or return error if not
        if (
            !sxbp_check(
                sxbp_attempt_line_shorten(figure, i, figure->size - 1),
                &status
            )
        ) {
            return status;
        } else {
            /*
             * set which how many lines we have left to solve
             * NOTE: this value is -1 because line 0 never needs solving
             */
            figure->lines_remaining = i - 1;
            // call the progress callback if it's been given
            if (options != NULL && options->progress_callback != NULL) {
                options->progress_callback(figure, options->callback_context);
            }
        }
    }
    // signal to caller that the call succeeded
    return SXBP_RESULT_OK;
}
