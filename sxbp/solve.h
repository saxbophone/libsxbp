/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for calculating the length of all
 * lines in a spiral such that the length of each line is at least 1 and none
 * of them collide.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a pointer to a spiral struct, the index of one of it's lines and a
 * target length to set that line to and a perfection threshold (0 for no
 * perfection, or otherwise the maximmum line length at which to allow
 * aggressive optimisation) attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 * returns a status struct (used for error information)
 *
 * Asserts:
 * - That spiral->lines is not NULL
 * - That index is less than spiral->size
 */
sxbp_status_t sxbp_resize_spiral(
    sxbp_spiral_t* spiral, uint64_t index, sxbp_length_t length,
    sxbp_length_t perfection_threshold
);

/*
 * given a pointer to a spiral spiral for which the length of all its lines are
 * not yet known, a perfection threshold (0 for no perfection, or otherwise
 * the maximmum line length at which to allow aggressive optimisation), the
 * index of the highest line to plot to, a pointer to a callback function and
 * a void pointer to a user-defined data struct for use with the callback,
 * calculate the length needed for each line in the spiral up to this index
 * (to avoid line overlap) and store these in a the spiral struct that is
 * pointed to by the pointer. If the spiral has some lines already solved, the
 * algorithm will start at the next unsolved line.
 * the function pointer and the user data pointer can be NULL.
 * If the function pointer is not NULL, then it will be called every time a new
 * line of the spiral is solved. The function should be of return type void and
 * take four arguments: a pointer to a spiral_t struct, an integer specifying
 * the index of the latest solved line, an integer specifying the index of the
 * highest line that will be solved and a void pointer used for accessing the
 * user data.
 * returns a status struct (used for error information)
 *
 * Asserts:
 * - That spiral->lines is not NULL
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
