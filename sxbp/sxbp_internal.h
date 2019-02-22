/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @internal
 * @file
 *
 * @brief This header file provides internal definitions for use within sxbp
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
// include guard
#ifndef SAXBOPHONE_SXBP_SXBP_INTERNAL_H
#define SAXBOPHONE_SXBP_SXBP_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

// private type for storing one of the items of a tuple
typedef int32_t sxbp_tuple_item_t;

// private generic tuple type for storing a vector-based quantity
typedef struct sxbp_tuple_t {
    sxbp_tuple_item_t x; // the x (horizontal) value of the tuple
    sxbp_tuple_item_t y; // the y (vertical) value of the tuple
} sxbp_tuple_t;

// private vector type used for representing directions
typedef sxbp_tuple_t sxbp_vector_t;
// private coördinate type used for representing cartesian coördinates
typedef sxbp_tuple_t sxbp_co_ord_t;

// private structure for storing figure's bounds (when line is plotted out)
typedef struct sxbp_bounds_t {
    sxbp_tuple_item_t x_min; // the smallest value x has been so far
    sxbp_tuple_item_t x_max; // the largest value x has been so far
    sxbp_tuple_item_t y_min; // the smallest value y has been so far
    sxbp_tuple_item_t y_max; // the largest value y has been so far
} sxbp_bounds_t;

/*
 * vector direction constants (private)
 * these can be indexed by the cartesian direction constants
 */
extern const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4];

/*
 * private, updates the current figure bounds given the location of the end of
 * the most recently-plotted line
 */
void sxbp_update_bounds(sxbp_co_ord_t location, sxbp_bounds_t* bounds);

// private, 'move' the given location in the given direction by the given amount
void sxbp_move_location(
    sxbp_co_ord_t* location,
    sxbp_direction_t direction,
    sxbp_length_t length
);

// private, 'move' the given location along the given line
void sxbp_move_location_along_line(sxbp_co_ord_t* location, sxbp_line_t line);

// private, calculates the figure's complete bounds in one step
sxbp_bounds_t sxbp_get_bounds(const sxbp_figure_t* figure, size_t scale);

/*
 * private, calculates the correct starting coördinates of a line such that
 * every coördinate is a positive number from the line's bounds
 */
sxbp_co_ord_t sxbp_get_origin_from_bounds(const sxbp_bounds_t bounds);

/*
 * private, walks the line of the figure, calling the callback with the
 * coördinates of each point of space occupied by the line of the figure
 * the scale of the shape produced can be increased with the scale parameter
 * parameter plot_vertices_only, if true will cause the callback to only be
 * called every time a vetice of the figure's line is encountered, i.e. the
 * interface of each line segment with another
 * the callback should return false if it does not want the function to continue
 * walking the line, otherwise it should return true.
 */
void sxbp_walk_figure(
    const sxbp_figure_t* figure,
    size_t scale,
    bool plot_vertices_only,
    bool( *plot_point_callback)(sxbp_co_ord_t location, void* callback_data),
    void* callback_data
);

/*
 * private, given a bounds struct, calculate the width and height of the bounds
 * and write these values out to the width and height pointed to by `width` and
 * `height`
 */
void sxbp_get_size_from_bounds(
    const sxbp_bounds_t bounds,
    sxbp_figure_dimension_t* restrict width,
    sxbp_figure_dimension_t* restrict height
);

/*
 * private, given a dimension value, try and generate a string representing it
 * in decimal notation, outputting this string to `output_string` and storing
 * its length in `string_length` if successful.
 * returns true/false for whether the operation succeeded or not
 */
bool sxbp_dimension_to_string(
    sxbp_figure_dimension_t dimension,
    char(* output_string)[11],
    size_t* string_length
);

// private, builds a bitmap large enough to fit coördinates in the given bounds
sxbp_result_t sxbp_make_bitmap_for_bounds(
    const sxbp_bounds_t bounds,
    sxbp_bitmap_t* bitmap
);

// private, prints out a bitmap to the given stream, for debugging
void sxbp_print_bitmap(sxbp_bitmap_t* bitmap, FILE* stream);

// private, refines a figure using the 'shrink from end' method
sxbp_result_t sxbp_refine_figure_shrink_from_end(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
);

// private, macro to assist in 'return early if NULL pointer' error checks
#define SXBP_RETURN_FAIL_IF_NULL(pointer) if (pointer == NULL) return SXBP_RESULT_FAIL_PRECONDITION

/*
 * private, macro which aborts via fale assertion with a message in expression,
 * to be used for indicating when supposedly unreachable code is reached
 */
#define SXBP_ABORT_UNREACHABLE_CODE() assert("Unreachable code reached")

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
