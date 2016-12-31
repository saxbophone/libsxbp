/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functions for plotting and caching the
 * points which make up the lines of a spiral.
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016
 *
 * @copyright Copyright (C) Joshua Saxby 2016
 *
 * @copyright This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * (version 3), as published by the Free Software Foundation.
 *
 * @copyright This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_PLOT_H
#define SAXBOPHONE_SAXBOSPIRAL_PLOT_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Calculates the sum of all line lengths in this spiral within the given
 * start and end indexes.
 * @details params start and end specify the range of line indexes to use for the
 * lines which are to be summed. These are inclusive of the lower bound but
 * exclusive of the upper bound.
 *
 * @param spiral The spiral of which line lengths should be summed.
 * @param start The index of the first line to include in those of which lengths
 * should be summed.
 * @param end The index of the line after the last line to include in those of
 * which lengths should be summed.
 * @return The sum of the lengths of all the lines in the given range.
 *
 * @note Asserts:
 * - That start and end indexes are less than or equal to the spiral size
 * - That spiral.lines is not NULL
 *
 * @todo Might also need to add an assertion to check that end is never less
 * than start.
 */
size_t sxbp_sum_lines(sxbp_spiral_t spiral, size_t start, size_t end);

/**
 * @brief Calculates a series of co-ords for which trace a given part of a
 * spiral line.
 * @details The whole spiral may be calculated, or just a given segment of it.
 * This is controlled by specifying which line to start at and which to end at.
 * 
 * @param spiral The spiral for which line co-ords should be calculated.
 * @param output [out] The co-ords array which calculated co-ords should be
 * written to.
 * @param start_point The x/y co-ordinates at which the line being plotted
 * originates at.
 * @param start The index of the first line which makes up the segment which is
 * being calculated.
 * @param end The index of the line after the last line which makes up the
 * segment which is being calculated.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note For this function to do anything useful, the spiral should at least
 * have some line lengths calculated, but this is not essential.
 *
 * @note Asserts:
 * - That output->items is NULL
 * - That start and end indexes are less than or equal to the spiral size
 * - That spiral.lines is not NULL
 *
 * @todo Might also need to add an assertion to check that end is never less
 * than start.
 */
sxbp_status_t sxbp_spiral_points(
    sxbp_spiral_t spiral, sxbp_co_ord_array_t* output,
    sxbp_co_ord_t start_point, size_t start, size_t end
);

/**
 * @brief Caches any uncached co-ords of the line of a spiral up to a given line
 * index.
 * @details If there are any outstanding co-ordinates which haven't been stored
 * in the spiral's internal cache, then these will be calculated and stored in
 * the cache. Subsequent identical calls will not incur the overhead of
 * re-calculating these co-ords, provided the lengths of any of the spiral's
 * lines are not changed in the process.
 *
 * @param spiral The spiral for which co-ords should be cached.
 * @param limit The highest index of line for which co-ords should be cached to.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note Asserts:
 * - That spiral->lines is not NULL
 * - That limit is less than or equal to spiral->size
 */
sxbp_status_t sxbp_cache_spiral_points(sxbp_spiral_t* spiral, size_t limit);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
