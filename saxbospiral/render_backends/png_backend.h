/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functionality to render a bitmap struct to PNG
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H
#define SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a bitmap_t struct and a pointer to a blank buffer_t, write the bitmap
 * data as a PNG image to the buffer, using libpng.
 * returns a status struct containing error information, if any
 */
sxbp_status_t sxbp_write_png_image(sxbp_bitmap_t bitmap, sxbp_buffer_t* buffer);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
