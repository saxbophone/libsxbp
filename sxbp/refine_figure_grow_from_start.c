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
 * private datatype for building 'line maps', which are 2D arrays of pointers to
 * sxbp_line_t items
 */
typedef struct line_map {
    // the width of the line_map (same data type as bitmap width)
    uint32_t width;
    // the height of the line_map (same data type as bitmap height)
    uint32_t height;
    // dynamically allocated 2D array of pointers to lines
    sxbp_line_t*** cells;
} line_map;

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct figure_collides_with_context {
    line_map* map;
    sxbp_line_t** collider;
    sxbp_figure_size_t max_line;
} figure_collides_with_context;

/*
 * private, frees dynamically allocated memory of a line_map instance
 * returns true if it needed to free it, or false if it didn't
 */
static bool sxbp_free_line_map(line_map* map) {
    // if map->cells is not null, then it needs to be deallocated
    if (map->cells != NULL) {
        for (uint32_t col = 0; col < map->width; col++) {
            // but first check each column of the col inside cells too
            if (map->cells[col] != NULL) {
                free(map->cells[col]);
            }
        }
        // finally, deallocate the row
        free(map->cells);
        return true;
    } else {
        // nothing to deallocate!
        return false;
    }
}

/*
 * private, initialises and allocates a line_map sized for the specified bounds
 * WARN: Don't even think about calling this function with an already allocated
 * line_map instance!
 */
static sxbp_result_t sxbp_init_line_map_from_bounds(
    line_map* map,
    sxbp_bounds_t bounds
) {
    /*
     * the width and height are the difference of the max and min dimensions
     * + 1.
     * this makes sense because for example from 1 to 10 there are 10 values
     * and the difference of these is 9 so the number of values is 9+1 = 10
     */
    map->width = (bounds.x_max - bounds.x_min) + 1;
    map->height = (bounds.y_max - bounds.y_min) + 1;
    // allocate dynamic memory for the row
    map->cells = calloc(map->width, sizeof(sxbp_line_t**));
    // catch allocation error and exit early
    if (map->cells == NULL) {
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // now allocate memory for the columns of the row
        for (uint32_t col = 0; col < map->width; col++) {
            map->cells[col] = calloc(map->height, sizeof(sxbp_line_t*));
            // catch allocation error, free and exit early
            if (map->cells[col] == NULL) {
                sxbp_free_line_map(map);
                return SXBP_RESULT_FAIL_MEMORY;
            }
            // pedantic, set all cells within the column explicitly to NULL
            for (uint32_t row = 0; row < map->height; row++) {
                map->cells[col][row] = NULL;
            }
        }
    }
    return SXBP_RESULT_OK;
}

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

// private, callback function for sxbp_figure_collides_with()
static bool sxbp_figure_collides_with_callback(
    sxbp_line_t* line,
    sxbp_co_ord_t location,
    void* data
) {
    // cast void pointer to a pointer to our context structure
    figure_collides_with_context* callback_data =
        (figure_collides_with_context*)data;
    // is this the last line? if so we need to tell walk() to finish early
    bool last_line = (line->id == callback_data->max_line);
    /*
     * if a pixel would be plotted at a location that isn't NULL, then stop and
     * set collider to point to the location that would have been plotted to
     * --this is the line that would be collided with
     */
    if (callback_data->map->cells[location.x][location.y] != NULL) {
        // the thing we collided with is the pointer at this location, store it!
        *callback_data->collider = callback_data->map->cells[location.x][location.y];
        // halt walking early by returning false
        return false;
    } else {
        /*
         * otherwise, we haven't collided yet so mark the cell with a pointer to
         * the current line
         */
        callback_data->map->cells[location.x][location.y] = line;
        // only continue if it's not the last line
        return !last_line;
    }
}

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
    // variable to store any errors in
    sxbp_result_t status = SXBP_RESULT_UNKNOWN;
    // build a 'line map' (2D array of pointers to lines) from figure's bounds
    line_map map;
    if (
        !sxbp_check(
            sxbp_init_line_map_from_bounds(&map, sxbp_get_bounds(figure, 1)),
            &status
        )
    ) {
        // if an error occurred building the line map, return it
        return status;
    } else {
        /*
         * use our callback function with walk() to plot the line pointers into
         * the line_map
         */
        figure_collides_with_context data = {
            .map = &map,
            .collider = collider,
            .max_line = line_index,
        };
        sxbp_walk_figure(
            figure, 1, sxbp_figure_collides_with_callback, (void*)&data
        );
        // free the line map
        sxbp_free_line_map(&map);
        // signal to caller that the call succeeded
        return SXBP_RESULT_OK;
    }
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
    // variable to store any errors in
    sxbp_result_t status = SXBP_RESULT_UNKNOWN;
    // try and set the line's length to that requested
    figure->lines[line_index].length = line_length;
    // check if the figure now collides, and if so, with which other line?
    sxbp_line_t* collider = NULL;
    if (
        !sxbp_check(
            sxbp_figure_collides_with(figure, line_index, &collider), &status
        )
    ) {
        // if an error occurred checking the collision, return it
        return status;
    } else {
        // if collider is not NULL, then there's been a collision
        if (collider != NULL) {
            printf("!\n");
            // collision!
            //   TODO: work out what length to extend the previous line to
            //   ...
            //   TODO: call self recursively, to resize the previous line to new
            // length
            //   ...
            //   TODO: set the original line at line index's length back to 1
            //   NOTE: or do we set it to the originally requested length?
            //   ...
        }
        // signal to caller that the call succeeded
        return SXBP_RESULT_OK;
    }
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
