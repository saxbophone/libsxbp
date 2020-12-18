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
#include <stdio.h>
#include <stdlib.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

// these concatenation macros are needed to properly concatenate other macros
#define CAT(prefix, major, minor, patch, symbol) CAT_(prefix, major, minor, patch, symbol)
#define CAT_(prefix, major, minor, patch, symbol) prefix ## _ ## major ## minor ## patch ## _ ## symbol
// a cheap "private" symbol generator, which uses version number as a prefix
#define SXBP_PRIVATE(symbol) CAT(SXBP_PRIVATE, SXBP_VERSION_MAJOR, SXBP_VERSION_MINOR, SXBP_VERSION_PATCH, symbol)

// the following symbols need to be external, but "private"
#define SXBP_VECTOR_DIRECTIONS SXBP_PRIVATE(VECTOR_DIRECTIONS)
#define sxbp_update_bounds SXBP_PRIVATE(update_bounds)
#define sxbp_move_location SXBP_PRIVATE(move_location)
#define sxbp_move_location_along_line SXBP_PRIVATE(move_location_along_line)
#define sxbp_get_bounds SXBP_PRIVATE(get_bounds)
#define sxbp_get_origin_from_bounds SXBP_PRIVATE(get_origin_from_bounds)
#define sxbp_walk_figure SXBP_PRIVATE(walk_figure)
#define sxbp_get_size_from_bounds SXBP_PRIVATE(get_size_from_bounds)
#define sxbp_dimension_to_string SXBP_PRIVATE(dimension_to_string)
#define sxbp_make_bitmap_for_bounds SXBP_PRIVATE(make_bitmap_for_bounds)
#define sxbp_print_bitmap SXBP_PRIVATE(print_bitmap)
#define sxbp_refine_figure_shrink_from_end SXBP_PRIVATE(refine_figure_shrink_from_end)

// private type for storing one of the items of a tuple
typedef int32_t sxbp_TupleItem;

// private generic tuple type for storing a vector-based quantity
typedef struct sxbp_Tuple {
    sxbp_TupleItem x; // the x (horizontal) value of the tuple
    sxbp_TupleItem y; // the y (vertical) value of the tuple
} sxbp_Tuple;

// private vector type used for representing directions
typedef sxbp_Tuple sxbp_Vector;
// private coördinate type used for representing cartesian coördinates
typedef sxbp_Tuple sxbp_CoOrd;

// private structure for storing figure's bounds (when line is plotted out)
typedef struct sxbp_Bounds {
    sxbp_TupleItem x_min; // the smallest value x has been so far
    sxbp_TupleItem x_max; // the largest value x has been so far
    sxbp_TupleItem y_min; // the smallest value y has been so far
    sxbp_TupleItem y_max; // the largest value y has been so far
} sxbp_Bounds;

/*
 * vector direction constants (private)
 * these can be indexed by the cartesian direction constants
 */
extern const sxbp_Vector SXBP_VECTOR_DIRECTIONS[4];

/*
 * private, updates the current figure bounds given the location of the end of
 * the most recently-plotted line
 */
void sxbp_update_bounds(sxbp_CoOrd location, sxbp_Bounds* bounds);

// private, 'move' the given location in the given direction by the given amount
void sxbp_move_location(
    sxbp_CoOrd* location,
    sxbp_Direction direction,
    sxbp_Length length
);

// private, 'move' the given location along the given line
void sxbp_move_location_along_line(sxbp_CoOrd* location, sxbp_Line line);

// private, calculates the figure's complete bounds in one step
sxbp_Bounds sxbp_get_bounds(const sxbp_Figure* figure, size_t scale);

/*
 * private, calculates the correct starting coördinates of a line such that
 * every coördinate is a positive number from the line's bounds
 */
sxbp_CoOrd sxbp_get_origin_from_bounds(const sxbp_Bounds bounds);

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
    const sxbp_Figure* figure,
    size_t scale,
    bool plot_vertices_only,
    bool( *plot_point_callback)(sxbp_CoOrd location, void* callback_data),
    void* callback_data
);

/*
 * private, given a bounds struct, calculate the width and height of the bounds
 * and write these values out to the width and height pointed to by `width` and
 * `height`
 */
void sxbp_get_size_from_bounds(
    const sxbp_Bounds bounds,
    sxbp_FigureDimension* restrict width,
    sxbp_FigureDimension* restrict height
);

/*
 * private, given a dimension value, try and generate a string representing it
 * in decimal notation, outputting this string to `output_string` and storing
 * its length in `string_length` if successful.
 * returns true/false for whether the operation succeeded or not
 */
bool sxbp_dimension_to_string(
    sxbp_FigureDimension dimension,
    char(* output_string)[11],
    size_t* string_length
);

// private, builds a bitmap large enough to fit coördinates in the given bounds
sxbp_Result sxbp_make_bitmap_for_bounds(
    const sxbp_Bounds bounds,
    sxbp_Bitmap* bitmap
);

// private, prints out a bitmap to the given stream, for debugging
void sxbp_print_bitmap(sxbp_Bitmap* bitmap, FILE* stream);

// private, refines a figure using the 'shrink from end' method
sxbp_Result sxbp_refine_figure_shrink_from_end(
    sxbp_Figure* figure,
    const sxbp_RefineFigureOptions* options
);

// private, macro to assist in 'return early if NULL pointer' error checks
#define SXBP_RETURN_FAIL_IF_NULL(pointer) if (pointer == NULL) return SXBP_RESULT_FAIL_PRECONDITION

/*
 * private, macro which prints an error message, then aborts
 * to be used for indicating when supposedly unreachable code is reached
 */
#define SXBP_ABORT_UNREACHABLE_CODE() fprintf(stderr, "Unreachable code reached in function %s()\n", __func__); abort()

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
