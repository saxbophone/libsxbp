/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for rendering a spiral to a bitmap.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_RENDER_H
#define SAXBOPHONE_SAXBOSPIRAL_RENDER_H

// sanity check for support of 64-bit integers
#if __SIZEOF_SIZE_T__ < 8
#warning "Please compile this code for a target with 64-bit words or greater."
#endif

#include <stdbool.h>
#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef struct sxbp_bitmap_t {
    uint64_t width;
    uint64_t height;
    bool ** pixels;
} sxbp_bitmap_t;

/*
 * given a spiral struct and a pointer to a blank bitmap_t struct, writes data
 * representing a monochromatic image of the rendered spiral to the bitmap
 * returns a status struct with error information (if any)
 */
sxbp_status_t sxbp_render_spiral(sxbp_spiral_t spiral, sxbp_bitmap_t* image);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
