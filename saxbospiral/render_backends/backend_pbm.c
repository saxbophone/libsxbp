/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functionality to render a bitmap struct to a
 * PBM image (binary version, stored in a buffer).
 *
 * Reference materials used for the PBM format are located at
 * <http://netpbm.sourceforge.net/doc/pbm.html>
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
#include <assert.h>

#include "../saxbospiral.h"
#include "../render.h"
#include "backend_pbm.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a bitmap_t struct and a pointer to a blank buffer_t, write the bitmap
 * data as a PBM image to the buffer
 * returns a status struct containing error information, if any
 *
 * Asserts:
 * - That bitmap.pixels is not NULL
 * - That buffer->bytes is NULL
 */
sxbp_status_t sxbp_render_backend_pbm(
    sxbp_bitmap_t bitmap, sxbp_buffer_t* buffer
) {
    // preconditional assertsions
    assert(bitmap.pixels != NULL);
    assert(buffer->bytes == NULL);
    // uh-oh, not implemented!
    return SXBP_IMPOSSIBLE_CONDITION;
}

#ifdef __cplusplus
} // extern "C"
#endif
