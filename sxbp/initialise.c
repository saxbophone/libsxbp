/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) 2016, 2017, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "initialise.h"


#ifdef __cplusplus
extern "C"{
#endif

sxbp_direction_t sxbp_change_direction(
    sxbp_direction_t current, sxbp_rotation_t turn
) {
    return (current + turn) % 4U;
}

sxbp_spiral_t sxbp_blank_spiral(void) {
    return (sxbp_spiral_t){0, NULL, {{NULL, 0}, 0}, false, 0, 0, 0, 0, 0, 0, };
}

sxbp_status_t sxbp_init_spiral(sxbp_buffer_t buffer, sxbp_spiral_t* spiral) {
    // preconditional assertions
    assert(spiral->lines == NULL);
    assert(spiral->co_ord_cache.co_ords.items == NULL);
    // result status object
    sxbp_status_t result;
    // number of lines is number of bits of the data, + 1 for the first UP line
    size_t line_count = (buffer.size * 8) + 1;
    // TODO: Check here for overflow condition
    // populate spiral struct
    spiral->size = line_count;
    spiral->collides = -1;
    // allocate enough memory for a line_t struct for each bit
    spiral->lines = calloc(sizeof(sxbp_line_t), line_count);
    // check for memory allocation failure
    if(spiral->lines == NULL) {
        result = SXBP_MALLOC_REFUSED;
        return result;
    }
    // First line is always an UP line - this is for orientation purposes
    sxbp_direction_t current = SXBP_UP;
    spiral->lines[0].direction = current;
    spiral->lines[0].length = 0;
    /*
     * now, iterate over all the bits in the data and convert to directions that
     * make the spiral pattern, storing these directions in the result lines
     */
    for(size_t s = 0; s < buffer.size; s++) {
        // byte-level loop
        for(uint8_t b = 0; b < 8; b++) {
            // bit level loop
            uint8_t e = 7 - b; // which power of two to use with bit mask
            uint8_t bit = (buffer.bytes[s] & (1 << e)) >> e; // the current bit
            size_t index = (s * 8) + (size_t)b + 1; // line index
            sxbp_rotation_t rotation; // the rotation we're going to make
            // set rotation direction based on the current bit
            rotation = (bit == 0) ? SXBP_CLOCKWISE : SXBP_ANTI_CLOCKWISE;
            // calculate the change of direction
            current = sxbp_change_direction(current, rotation);
            // store direction in result struct
            spiral->lines[index].direction = current;
            // set length to 0 initially
            spiral->lines[index].length = 0;
        }
    }
    // all ok
    result = SXBP_OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
