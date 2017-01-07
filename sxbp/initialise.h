/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides basic functions to initialise a spiral.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Gets the direction which is clockwise or anti-clockwise to the current
 * direction.
 * @details Given a current direction and a rotational direction, return the new
 * direction which would be faced after turning 90 degrees in the given
 * rotational direction.
 *
 * @param current The current direction to turn from.
 * @param turn The rotational direction to turn in.
 * @return The new direction which will be faced after turning.
 */
sxbp_direction_t sxbp_change_direction(
    sxbp_direction_t current, sxbp_rotation_t turn
);

/**
 * @brief Builds a blank spiral struct.
 * @details This is a convenience function, as the struct has many fields. This
 * function will create a spiral struct with all of these fields initialised to
 * 0 or their default 'blank' state. No memory is allocated.
 *
 * @return A blank spiral struct with all fields initialised to 0 or blank values.
 */
sxbp_spiral_t sxbp_blank_spiral(void);

/**
 * @brief Builds a partially-complete spiral from binary input data stored in a
 * buffer.
 * @details This converts the 0s and 1s in the buffer data into UP, LEFT, DOWN,
 * RIGHT instructions which are then used to build the pattern. Only the line
 * directions are calculated at this point, all line lengths are 0.
 *
 * @param buffer A buffer containing at least one byte of data, used to
 * determine the directions of the lines in the spiral it will create.
 * @param spiral [out] Spiral object which the line directions will be written
 * to.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note Asserts:
 * - That all the pointer members of parameter spiral are set to NULL
 */
sxbp_status_t sxbp_init_spiral(sxbp_buffer_t buffer, sxbp_spiral_t* spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
