/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functionality to render a bitmap struct
 * to a PNG image (stored in a buffer).
 * 
 * @note PNG output support may have not been enabled in the compiled version
 * of libsxbp that you have. If support is not enabled, the library
 * boolean constant SXBP_PNG_SUPPORT will be set to false and the one public
 * function defined in this library will return SXBP_NOT_IMPLEMENTED.
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016
 *
 * @copyright Copyright (C) Joshua Saxby 2016
 *
 * @copyright This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * (version 3), as published by the Free Software Foundation.
 *
 * @copyright This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_BACKEND_PNG_H
#define SAXBOPHONE_SAXBOSPIRAL_BACKEND_PNG_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Flag for whether PNG output support has been enabled.
 * @details This is compiled into the library, based on a macro set at build
 * time. The value of this constant is false if PNG support is not enabled and
 * true if it is.
 */
extern const bool SXBP_PNG_SUPPORT;

/**
 * @brief Renders a bitmap image to a PNG image.
 *
 * @param bitmap Bitmap containing the image to render.
 * @param buffer [out] Buffer to write out the PBM image data to.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_NOT_IMPLEMENTED if PNG support has not been enabled.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note Asserts:
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
