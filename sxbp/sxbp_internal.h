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
 * @copyright Copyright (C) Joshua Saxby 2016-2017, 2018
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// include guard
#ifndef SAXBOPHONE_SXBP_SXBP_INTERNAL_H
#define SAXBOPHONE_SXBP_SXBP_INTERNAL_H

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
sxbp_bounds_t sxbp_get_bounds(const sxbp_figure_t* figure);

// private, prints out a bitmap to the given stream, for debugging
void sxbp_print_bitmap(sxbp_bitmap_t* bitmap, FILE* stream);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
