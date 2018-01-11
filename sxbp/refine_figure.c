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
#include <stdio.h>
#include <stdlib.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

// private, returns true if the figure collides with itself or false if not
static bool sxbp_figure_collides(sxbp_figure_t* figure) {
    // get spiral bounds first
    sxbp_bounds_t bounds = sxbp_get_bounds(figure);
    // now build an empty bitmap with the dimensions of the bounds + 1
    sxbp_bitmap_t bitmap = {
        // HACK: I have no idea why these need to be + 1, needs investigating
        .width = (bounds.x_max - bounds.x_min) + 1,
        .height = (bounds.y_max - bounds.y_min) + 1,
        .pixels = NULL,
    };
    // allocate memory for the bitmap and check this succeeded
    if (!sxbp_init_bitmap(&bitmap)) {
        // XXX: of course this isn't a good idea
        abort();
    } else {
        /*
         * the transformation vector for all coördinates is the negative of the min
         * bounds
         * the start location is the transformation vector
         * (avoids extra calculations)
         */
        sxbp_co_ord_t location = {
            .x = -bounds.x_min,
            .y = -bounds.y_min,
        };
        // plot a pixel at the start location first of all
        bitmap.pixels[location.x][location.y] = true;
        /*
         * walk the spiral's line and plot the coördinates of each
         * quit and return false early if any collide (if pixel exists already)
         */
        for (uint32_t i = 0; i < figure->size; i++) {
            sxbp_line_t line = figure->lines[i];
            sxbp_vector_t direction = SXBP_VECTOR_DIRECTIONS[line.direction];
            // plot as many pixels as the length of the line
            for (sxbp_length_t l = 0; l < line.length; l++) {
                location.x += direction.x;
                location.y += direction.y;
                // if there's no pixel here, plot it
                if (bitmap.pixels[location.x][location.y] == false) {
                    bitmap.pixels[location.x][location.y] = true;
                } else {
                    // otherwise, cleanup and return true to mark collision
                    sxbp_free_bitmap(&bitmap);
                    return true;
                }
            }
        }
        // free the memory allocated for the bitmap
        sxbp_free_bitmap(&bitmap);
        // return false, we found no collisions
        return false;
    }
}

/*
 * private, attempts to shorten the line of the figure at index l
 * if it succeeds, it will call itself recursively for each line after l from
 * max to l, in that order.
 */
static void sxbp_attempt_line_shorten(
    sxbp_figure_t* figure,
    uint32_t l,
    uint32_t max
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
            // NOTE: DEBUG
            printf("\t%03u: %03i\n", l, figure->lines[l].length);
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
            // NOTE: DEBUG
            printf("%03u: %03i\n", i, figure->lines[i].length);
        }
        // signal to caller that the call was valid
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
