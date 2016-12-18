/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for plotting and caching the points
 * which make up the lines of a spiral.
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License (version 3),
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_PLOT_H
#define SAXBOPHONE_SAXBOSPIRAL_PLOT_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * returns the sum of all line lengths within the given indexes
 *
 * Asserts:
 * - That start and end indexes are less than or equal to the spiral size
 * - That spiral.lines is not NULL
 */
size_t sxbp_sum_lines(sxbp_spiral_t spiral, size_t start, size_t end);

/*
 * given a spiral_t struct, a pointer to a co_ord_array_t, a pair of co-ords
 * specifying the start point and indexes of the lowest and highest line
 * segments to use, write to the co_ord_array_t struct all the co-ordinates of
 * the line segments of the struct according to the current directions and
 * lengths of the lines in the spiral.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit.
 * returns a status struct with error information (if any)
 *
 * Asserts:
 * - That the output struct's items pointer is NULL
 * - That start and end indexes are less than or equal to the spiral size
 * - That spiral.lines is not NULL
 */
sxbp_status_t sxbp_spiral_points(
    sxbp_spiral_t spiral, sxbp_co_ord_array_t* output,
    sxbp_co_ord_t start_point, size_t start, size_t end
);

/*
 * given a pointer to a spiral struct and limit, which is the index of the last
 * line to use, calculate and store the co-ordinates of all line segments that
 * would make up the spiral if the current lengths and directions were used.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit. The co-ords are stored in the spiral's
 * co_ord_cache member and are re-used if they are still valid
 * returns a status struct with error information (if any)
 *
 * Asserts:
 * - That spiral->lines is not NULL
 * - That limit is less than or equal to spiral->size
 */
sxbp_status_t sxbp_cache_spiral_points(sxbp_spiral_t* spiral, size_t limit);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
