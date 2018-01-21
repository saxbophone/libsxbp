/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_refine_figure`, a
 * public function used to shorten the lines of an SXBP figure to something less
 * space-consuming, while still maintaining a pattern that has no collisions
 * between lines.
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
extern "C" {
#endif

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct figure_collides_context {
    sxbp_bitmap_t* image;
    bool collided;
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
        callback_data->collided = true;
        // return false to tell the walk function to stop early
        return false;
    }
}

// private, returns true if the figure collides with itself or false if not
static bool sxbp_figure_collides(const sxbp_figure_t* figure) {
    // get spiral bounds first
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 1);
    // build bitmap for bounds
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
    if (!sxbp_make_bitmap_for_bounds(bounds, &bitmap)) {
        // TODO: implment better error-handling than this
        abort();
    } else {
        // construct callback context data
        figure_collides_context data = {
            .image = &bitmap, .collided = false,
        };
        // begin walking the figure, use our callback function to handle points
        sxbp_walk_figure(figure, 1, sxbp_figure_collides_callback, (void*)&data);
        // NOTE: debug printing
        if (!data.collided) {
            sxbp_print_bitmap(&bitmap, stdout);
        }
        // free the memory allocated for the bitmap
        sxbp_free_bitmap(&bitmap);
        // return whether or not we found no collisions
        return data.collided;
    }
}

/*
 * private, attempts to shorten the line of the figure at index l
 * if it succeeds, it will call itself recursively for each line after l from
 * max to l, in that order.
 */
static void sxbp_attempt_line_shorten(
    sxbp_figure_t* figure,
    const uint32_t l,
    const uint32_t max
) {
    sxbp_line_t* line = &figure->lines[l];
    // it only makes sense to try and shorten lines longer than 1
    if (line->length > 1) {
        // we'll need this later to check if we were actually able to shorten it
        sxbp_length_t original_length = line->length;
        // as an ambitious first step, set to 1 (try best case scenario first)
        line->length = 1;
        /*
         * if that caused a collision, keep extending it until it no longer
         * collides (or we reach the original length) --we can quit in that case
         * as we already know it doesn't collide
         */
        while (line->length < original_length && sxbp_figure_collides(figure)) {
            line->length++;
        }
        /*
         * at this point, the shape now no longer collides. now, we check to see
         * if we were able to shorten the line at all. If so, we then try and
         * shorten the lines after this one, in reverse order.
         */
        if (line->length < original_length) {
            // try and shorten other lines some more
            for (uint32_t i = max; i >= l; i--) {
                sxbp_attempt_line_shorten(figure, i, max);
            }
        }
    }
}

bool sxbp_refine_figure(sxbp_figure_t* figure) {
    // we can't refine a figure that has no lines allocated, so check this first
    if (figure->lines == NULL) {
        // bail early, we can't do anything with this
        return false;
    } else {
        // iterate over lines backwards - we don't care about line 0
        for (uint32_t i = figure->size - 1; i > 0; i--) {
            // try and shorten it
            sxbp_attempt_line_shorten(figure, i, figure->size - 1);
        }
        // signal to caller that the call was valid
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
