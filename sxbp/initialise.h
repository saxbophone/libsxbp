/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides basic functions to initialise a spiral.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * when facing the direction specified by current, return the direction that
 * will be faced when turning in the rotational direction specified by turn.
 */
sxbp_direction_t sxbp_change_direction(
    sxbp_direction_t current, sxbp_rotation_t turn
);

/*
 * returns a spiral struct with all fields initialised to 0
 */
sxbp_spiral_t sxbp_blank_spiral();

/*
 * given a buffer_t full of data, and a pointer to a blank spiral_t
 * struct, populates the spiral struct from the data in the buffer
 * this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
 * instructions which are then used to build the pattern.
 * returns a status_t struct with error information (if needed)
 *
 * Asserts:
 * - That the spiral struct pointed to has its pointer attributes set to NULL
 */
sxbp_status_t sxbp_init_spiral(sxbp_buffer_t buffer, sxbp_spiral_t* spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
