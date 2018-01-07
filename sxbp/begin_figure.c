/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_begin_figure`, a
 * public function used to build an unrefined SXBP figure from input data.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <inttypes.h> // NOTE: DEBUG
#include <stdio.h> // NOTE: DEBUG
#include <stdlib.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private enum type for representing rotation direction
 * This makes the maths for discerning the line directions read particularly
 * well and intuitively.
 */
typedef enum sxbp_rotation_t {
    SXBP_ANTI_CLOCKWISE = -1, // The rotational direction 'ANTI-CLOCKWISE'
    SXBP_CLOCKWISE = 1, // The rotational direction 'CLOCKWISE'
} sxbp_rotation_t;

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
static const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4] = {
    {  0,  1, }, /* SXBP_UP    (0) */
    {  1,  0, }, /* SXBP_RIGHT (1) */
    {  0, -1, }, /* SXBP_DOWN  (2) */
    { -1,  0, }, /* SXBP_LEFT  (3) */
};

// private, builds a line from a direction and length
static sxbp_line_t sxbp_make_line(
    sxbp_direction_t direction,
    sxbp_length_t length
) {
    return (sxbp_line_t){ .direction = direction, .length = length, };
}

// private, converts a binary bit into a rotational direction
static sxbp_rotation_t sxbp_rotation_from_bit(bool bit) {
    return bit == 0 ? SXBP_CLOCKWISE : SXBP_ANTI_CLOCKWISE;
}

/*
 * private, returns the new cartesian direction that will be faced after
 * turning from the given cartesian direction by the given rotational direction
 */
static sxbp_direction_t sxbp_change_line_direction(
    sxbp_direction_t current,
    sxbp_rotation_t turn
) {
    return (current + turn) % 4;
}

/*
 * private, works out how long the next line should be to ensure no lines after
 * it collide
 */
static sxbp_length_t sxbp_next_length(
    sxbp_co_ord_t location,
    sxbp_direction_t direction,
    sxbp_bounds_t bounds
) {
    switch (direction) {
        case SXBP_UP:
            return abs(bounds.y_max - location.y) + 1;
        case SXBP_RIGHT:
            return abs(bounds.x_max - location.x) + 1;
        case SXBP_DOWN:
            return abs(bounds.y_min - location.y) + 1;
        case SXBP_LEFT:
            return abs(bounds.x_min - location.x) + 1;
        default:
            // NOTE: should never happen
            return 1;
    }
}

/*
 * private, updates the current figure bounds given the location of the end of
 * the most recently-plotted line
 */
static void sxbp_update_bounds(sxbp_co_ord_t location, sxbp_bounds_t* bounds) {
    if (location.x > bounds->x_max) {
        bounds->x_max = location.x;
    } else if (location.x < bounds->x_min) {
        bounds->x_min = location.x;
    }
    if (location.y > bounds->y_max) {
        bounds->y_max = location.y;
    } else if (location.y < bounds->y_min) {
        bounds->y_min = location.y;
    }
}

// private, 'move' the given location along the given line
static void sxbp_update_location(
    sxbp_co_ord_t* location,
    sxbp_line_t line
) {
    sxbp_vector_t direction_vector = SXBP_VECTOR_DIRECTIONS[line.direction];
    location->x += direction_vector.x * line.length;
    location->y += direction_vector.y * line.length;
}

/*
 * private, plots an sxbp line from the data in the buffer in the given figure,
 * setting line directions and lengths such that there are no collisions and
 * each line is at least 1 unit long
 */
static void sxbp_plot_lines(const sxbp_buffer_t* data, sxbp_figure_t* figure) {
    // loop state variables
    sxbp_co_ord_t location = { 0 }; // where the end of the last line is
    sxbp_bounds_t bounds = { 0 }; // the bounds of the line traced so far
    // the first line is always an up line - this is for orientation purposes
    sxbp_direction_t facing = SXBP_UP;
    // add first line to the figure
    figure->lines[0] = sxbp_make_line(facing, 1);
    // update the location
    sxbp_update_location(&location, figure->lines[0]);
    // update the bounds
    sxbp_update_bounds(location, &bounds);
    /*
     * now, iterate over all the bits in the data and convert to directions that
     * make the spiral pattern, also deducing the length to set these to to
     * avoid any collisions
     */
    for (uint32_t s = 0; s < data->size; s++) {
        // byte-level loop
        for (uint8_t b = 0; b < 8; b++) {
            // bit level loop - extract the bit
            uint8_t e = 7 - b; // which power of two to use with bit mask
            bool bit = (data->bytes[s] & (1 << e)) >> e; // the current bit
            uint32_t index = (s * 8) + (uint32_t)b + 1; // line index
            // set rotation direction based on the current bit
            sxbp_rotation_t rotation = sxbp_rotation_from_bit(bit);
            // calculate the new direction
            facing = sxbp_change_line_direction(facing, rotation);
            // calculate what length this line should be
            sxbp_length_t length = sxbp_next_length(location, facing, bounds);
            // NOTE: DEBUG
            printf("%" PRIu8 ", %" PRIu32 "\n", facing, length);
            // make line
            sxbp_line_t line = sxbp_make_line(facing, length);
            // add line to figure
            figure->lines[index] = line;
            // update location and bounds
            sxbp_update_location(&location, line);
            sxbp_update_bounds(location, &bounds);
        }
    }
    // NOTE: DEBUG
    printf(
        "%" PRId32 ", %" PRId32 ", %" PRId32 ", %" PRId32 "\n",
        bounds.x_min, bounds.y_min, bounds.x_max, bounds.y_max
    );
}

bool sxbp_begin_figure(const sxbp_buffer_t* data, sxbp_figure_t* figure) {
    // erase the figure first to ensure it's blank
    sxbp_free_figure(figure);
    /*
     * the number of lines is the number of bits in the buffer (byte count * 8)
     * + 1 (for the extra starting line)
     */
    figure->size = data->size * 8 + 1;
    // allocate memory for the figure
    if (!sxbp_init_figure(figure)) {
        // exit early and signal error status
        return false;
    } else {
        // allocation succeeded, now populate the lines
        sxbp_plot_lines(data, figure);
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
