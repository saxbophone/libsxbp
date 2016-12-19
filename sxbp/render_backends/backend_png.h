/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functionality to render a bitmap struct to a
 * PNG image (stored in a buffer).
 *
 * NOTE: PNG output support may have not been enabled in the compiled version
 * of libsxbp that you have. If support is not enabled, the library
 * boolean constant SXBP_PNG_SUPPORT will be set to false and the one public
 * function defined in this library will return SXBP_NOT_IMPLEMENTED.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_BACKEND_PNG_H
#define SAXBOPHONE_SAXBOSPIRAL_BACKEND_PNG_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a bitmap_t struct and a pointer to a blank buffer_t, write the bitmap
 * data as a PNG image to the buffer, using libpng.
 * returns a status struct containing error information
 * returns SXBP_NOT_IMPLEMENTED if PNG support was not enabled
 *
 * Asserts:
 * - That bitmap.pixels is not NULL
 * - That buffer->bytes is NULL
 */
sxbp_status_t sxbp_render_backend_png(
    sxbp_bitmap_t bitmap, sxbp_buffer_t* buffer
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
