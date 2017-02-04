/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functionality to render a bitmap struct
 * to a PBM image (binary version, stored in a buffer).
 *
 * @remark Reference materials used for the PBM format are located at
 * <http://netpbm.sourceforge.net/doc/pbm.html>
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016, 2017
 *
 * @copyright Copyright (C) Joshua Saxby 2016, 2017
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_BACKEND_PBM_H
#define SAXBOPHONE_SAXBOSPIRAL_BACKEND_PBM_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Renders a bitmap image to a PBM image.
 *
 * @param bitmap Bitmap containing the image to render.
 * @param[out] buffer Buffer to write out the PBM image data to.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note Asserts:
 * - That bitmap.pixels is not NULL
 * - That buffer->bytes is NULL
 */
sxbp_status_t sxbp_render_backend_pbm(
    sxbp_bitmap_t bitmap, sxbp_buffer_t* buffer
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
