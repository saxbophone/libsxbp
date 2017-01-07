/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functions for calculating the length of
 * all lines in a spiral such that the length of each line is at least 1 and
 * none of them collide.
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016, 2017
 *
 * @copyright Copyright (C) Joshua Saxby 2016
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Attempt to set a given line of a spiral to a given length.
 * @details If this cannot be done because the operation would cause a line
 * collision, then backtrack and re-size the previous lines recursively until a
 * non-colliding conclusion can be found.
 *
 * @param spiral The spiral for which the line should be resized.
 * @param index The index of the line to resize.
 * @param length The length to attempt to resize the line to.
 * @param perfection_threshold The maximum line length of colliding lines at
 * which aggressive optimisations are allowed (or 0 to disable these
 * optimisations completely).
 * @return SXBP_OPERATION_OK on success.
 * @return Any other failure code on failure.
 *
 * @note Asserts:
 * - That spiral->lines is not NULL
 * - That index is less than spiral->size
 */
sxbp_status_t sxbp_resize_spiral(
    sxbp_spiral_t* spiral, uint64_t index, sxbp_length_t length,
    sxbp_length_t perfection_threshold
);

/**
 * @brief Solve the given incomplete spiral such that each line has a length of
 * at least 1 unit.
 * @details This function will solve a given spiral such that each line of the
 * spiral has a length of at least 1 unit, and that none of these lines collide
 * with one another. The algorithm operates recursively on the spiral to achieve
 * this. A spiral at any point of completion may be passed to this function and
 * the algorithm will solve just the parts which have yet to be solved if
 * partial work has already been done to solve the spiral. A progress callback
 * may optionally also be given to the function. If this is non-NULL, then this
 * callback function will be called every time the algorithm finds the length of
 * another line. Once this function has finished, it may be possible to render
 * the spiral.
 *
 * @param spiral The spiral to solve. Function operates on the spiral in-place
 * (mutating operation).
 * @param perfection_threshold The maximum line length of colliding lines at
 * which aggressive optimisations are allowed (or 0 to disable these
 * optimisations completely).
 * @param max_line The index of the highest line to plot to.
 * @param progress_callback An optional function pointer to use as a callback
 * which gets called every time a new line length is successfully found. The
 * signature of the callback should be like so:
 * @code
 * void callback_name(
 *     sxbp_spiral_t* spiral, uint64_t latest_line, uint64_t target_line,
 *     void* progress_callback_user_data
 * )
 * @endcode
 * If a callback is not required, this argument should be NULL.
 * The callback takes four arguments, where:
 *   - spiral is the spiral being solved
 *   - latest_line is the index of the latest line to be solved
 *   - target_line is the index of the highest line that will be solved
 *   - progress_callback_user_data is a void pointer to some data of
 *   user-defined type.
 * @param progress_callback_user_data An optional void pointer to a user-defined
 * type, useful for passing context or state to the callback function if it
 * needs to do something requiring file handles or some other context handle.
 * This argument should be NULL if not required.
 * @warning No assumptions are made on the data type of the variable to which
 * progress_callback_user_data points to (if any). Hence, responsibility for
 * type-safety is placed on the user.
 * @return SXBP_OPERATION_OK on success.
 * @return Any other failure code on failure.
 *
 * @warning This function may take a **VERY VERY LONG TIME** to run. This means
 * on a magnitude of anything from minutes to months.
 *
 * @note Asserts:
 * - That spiral->lines is not NULL
 *
 * @todo Improve the efficiency of this function so that it runs much faster.
 */
sxbp_status_t sxbp_plot_spiral(
    sxbp_spiral_t* spiral, sxbp_length_t perfection_threshold, uint64_t max_line,
    void(* progress_callback)(
        sxbp_spiral_t* spiral, uint64_t latest_line, uint64_t target_line,
        void* progress_callback_user_data
    ),
    void* progress_callback_user_data
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
