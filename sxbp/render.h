/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functions for rendering a spiral to a
 * bitmap.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_RENDER_H
#define SAXBOPHONE_SAXBOSPIRAL_RENDER_H

#include <stdbool.h>
#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Used to represent a basic 1-bit, pure black/white bitmap image.
 * @details The image has integer height and width, and a 2-dimensional array of
 * 1-bit pixels which are either black or white.
 */
typedef struct sxbp_bitmap_t {
    /** @brief The width of the bitmap in pixels */
    uint64_t width;
    /** @brief The height of the bitmap in pixels */
    uint64_t height;
    /**
     * @brief A 2-dimensional array of pixels.
     * @details false is black and true is white.
     */
    bool ** pixels;
} sxbp_bitmap_t;

/**
 * @brief Renders the line of a spiral to a bitmap.
 * @details The lines of the spiral are plotted on a white background and the
 * pixel data representing the resulting shape is written to the given image.
 *
 * @param spiral The spiral which should be rendered.
 * @param image The bitmap to write the pixel data out to.
 * @return SXBP_OPERATION_OK on success.
 * @return SXBP_MALLOC_REFUSED on memory allocation failure.
 *
 * @note Asserts:
 * - That image->pixels is NULL
 * - That spiral.lines is not NULL
 */
sxbp_status_t sxbp_render_spiral_raw(
    sxbp_spiral_t spiral, sxbp_bitmap_t* image
);

/**
 * @brief Renders the line of a spiral to an image format.
 * @details The lines of the spiral are plotted on a white background and the
 * pixel data representing the resulting shape is written as data in a given
 * image file format, using the callback function to achieve this conversion.
 * The callback function should inspect the pixels of the bitmap passed to it
 * and write the file data representing the bitmap in it's respective file
 * format out to the buffer.
 * 
 * @details The function signature of the callback is the same as that of the
 * image rendering functions provided by the library: sxbp_render_backend_pbm
 * and sxbp_render_backend_png. This allows one of these to be provided as the
 * callback function, a convenient use of function chaining can achieve this
 * like so:
 * @code
 * sxbp_render_spiral_image(spiral, buffer, sxbp_render_backend_pbm);
 * @endcode
 *
 * @param spiral The spiral which should be rendered.
 * @param buffer [out] The data buffer to which the bytes of the image file
 * should be written.
 * @param image_writer_callback A function pointer with the following signature:
 * @code
 * sxbp_status_t callback_name(sxbp_bitmap_t image, sxbp_buffer_t* buffer)
 * @endcode
 * @return SXBP_OPERATION_OK on success.
 * @return Any other valid value of the enumeration sxbp_status_t on error.
 *
 * @note Asserts:
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
