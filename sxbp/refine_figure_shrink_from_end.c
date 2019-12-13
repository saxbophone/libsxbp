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

/*
 * private, attempts to shorten the line of the figure at index l
 * if it succeeds, it will call itself recursively for each line after l from
 * max to l, in that order.
 */
static sxbp_Result sxbp_attempt_line_shorten(
    sxbp_Figure* figure,
    const sxbp_FigureSize l,
    const sxbp_FigureSize max
) {
    sxbp_Line* line = &figure->lines[l];
    // it only makes sense to try and shorten lines longer than 1
    if (line->length > 1) {
        // we'll need this later to check if we were actually able to shorten it
        sxbp_Length original_length = line->length;
        // as an ambitious first step, set to 1 (try best case scenario first)
        line->length = 1;
        // check if it collided
        sxbp_CollisionResult result = SXBP_COLLISION_RESULT_CONTINUES;
        // we'll store any errors encountered by this function here
        sxbp_Result status = SXBP_RESULT_UNKNOWN;
        if (
            !sxbp_check(sxbp_figure_collides(figure, &result, false), &status)
        ) {
            // handle error
            return status;
        } else {
            /*
             * if that caused a collision, keep extending it until it no longer
             * collides (or we reach the original length)
             * --we can quit in that case as we already know it doesn't collide
             */
            while (
                line->length < original_length &&
                result == SXBP_COLLISION_RESULT_COLLIDES
            ) {
                line->length++;
                // check again if it colldes and handle any errors
                if (
                    !sxbp_check(
                        sxbp_figure_collides(figure, &result, false), &status
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
                for (sxbp_FigureSize i = max; i >= l; i--) {
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

sxbp_Result sxbp_refine_figure_shrink_from_end(
    sxbp_Figure* figure,
    const sxbp_RefineFigureOptions* options
) {
    // variable to store any errors in
    sxbp_Result status = SXBP_RESULT_UNKNOWN;
    // iterate over lines backwards - we don't care about line 0
    for (sxbp_FigureSize i = figure->size - 1; i > 0; i--) {
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
