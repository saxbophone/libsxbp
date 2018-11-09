/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_grow_from_start`, a public function providing a specific
 * algorithm for refining a figure by attempting to grow the lines before those
 * that collide until the collision stops, doing this recursively as required.
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

/*
 * Algorithm:
 *
 * - FOR all lines, counting forwards:
 *     - SetLineLength(CurrentLine, 1)
 *
 * - SetLineLength(line, length)
 *     - Try and SET 'line' length to 'length'
 *     - IF this causes a collision:
 *         - NOTE the line that it collided with (CollidedLine)
 *         - SetLineLength(
 *               PreviousLine, EvadeDistance(CurrentLine, CollidedLine)
 *           )
 *
 * - EvadeDistance(PreviousLine, CurrentLine, CollidedLine)
 *     - COMPARE PreviousLine, CurrentLine and CollidedLine lengths, directions
 *       and positions to work out what length PreviousLine should be in order
 *       for CurrentLine to not collide with CollidedLine
 *     - RETURN new distance for PreviousLine
 */

// XXX: Stub functions don't need warnings about unused parameters!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/*
 * private, given an sxbp figure, the most recently plotted line index and a
 * pointer to a pointer to a line, checks if the full line of the given figure
 * as plotted up to the line index collides with itself. If it does, collider
 * will be set to point to the line which it collides with.
 */
static sxbp_result_t sxbp_figure_collides_with(
    const sxbp_figure_t* figure,
    sxbp_figure_size_t line_index,
    sxbp_line_t** collider
) {
    // signal to caller that the call succeeded
    return SXBP_RESULT_OK;
}
// reënable all warnings
#pragma GCC diagnostic pop

/*
 * private, attempts to change the length of the line at the given line_index to
 * the requested line length.
 */
static sxbp_result_t sxbp_set_line_length(
    sxbp_figure_t* figure,
    sxbp_figure_size_t line_index,
    sxbp_length_t line_length
) {
    // try and set the line's length to that requested
    figure->lines[line_index].length = line_length;
    // check if the figure now collides, and if so, with which other line?
    sxbp_line_t* collider = NULL;
    // TODO: Handle error!
    sxbp_figure_collides_with(figure, line_index, &collider);
    // IF COLLIDES:
    //   TODO: work out what length to extend the previous line to
    //   ...
    //   TODO: call self recursively, to resize the previous line to new length
    //   ...
    //   TODO: set the original line at line index's length back to 1
    //   NOTE: or do we set it to the originally requested length?
    //   ...
    // signal to caller that the call succeeded
    return SXBP_RESULT_OK;
}

sxbp_result_t sxbp_refine_figure_grow_from_start(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    // variable to store any errors in
    // sxbp_result_t status = SXBP_RESULT_UNKNOWN;
    // try and set the length of each line in the figure (ascending) to 1
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        // try and set the line length to 1 using a helper function
        sxbp_set_line_length(figure, i, 1); // TODO: Handle error!
        // set which how many lines we have left to solve
        figure->lines_remaining = figure->size - 1 - i;
        // TODO: refactor the following code into sxbp_internal:
        // call the progress callback if it's been given
        if (options != NULL && options->progress_callback != NULL) {
            options->progress_callback(figure, options->callback_context);
        }
    }
    // signal to caller that the call succeeded
    return SXBP_RESULT_OK;
}

#ifdef __cplusplus
} // extern "C"
#endif
