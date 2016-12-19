/*
 * This source file forms part of libsxbp, a library which generates
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
 *
 * Asserts:
 * - That image->pixels is NULL
 * - That spiral.lines is not NULL
 */
sxbp_status_t sxbp_render_spiral_raw(
    sxbp_spiral_t spiral, sxbp_bitmap_t* image
);

/*
 * given a spiral struct, a pointer to a blank buffer and a pointer to a
 * function with the appropriate signature, render the spiral as a bitmap and
 * then call the pointed-to function to render the image to the buffer (in
 * whatever format the function pointer is written for)
 * returns a status struct with error information (if any)
 *
 * Asserts:
 * - That spiral.lines is not NULL
 * - That buffer->bytes is NULL
 * - That the function pointer is not NULL
 */
sxbp_status_t sxbp_render_spiral_image(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer,
    sxbp_status_t(* image_writer_callback)(
        sxbp_bitmap_t image, sxbp_buffer_t* buffer
    )
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
