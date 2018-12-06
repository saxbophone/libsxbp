/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure_to_pbm`,
 * a public function used to render an SXBP figure to a PBM file buffer.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <inttypes.h>
#include <iso646.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

// private, works out how many bytes are needed to store the given bitmap as PBM
static size_t sxbp_get_pbm_image_size(
    size_t width,
    size_t height,
    size_t width_string_length,
    size_t height_string_length,
    size_t* bytes_per_row
) {
    // calculate number of bytes per row - this is ceiling(width / 8)
    *bytes_per_row = (size_t)ceil((double)width / 8.0);
    // calculate number of bytes for the entire image pixels (rows and columns)
    size_t image_bytes = *bytes_per_row * height;
    // finally put it all together to get total image buffer size
    return (
        3 // "P4" magic number + whitespace
        + width_string_length + 1 // width of image in decimal + whitespace
        + height_string_length + 1 // height of image in decimal + whitespace
        + image_bytes // lastly, the bytes which make up the image pixels
    );
}

// private, tries to allocate data for the whole image and writes the header
static sxbp_result_t sxbp_write_pbm_header(
    const sxbp_bitmap_t* const bitmap,
    sxbp_buffer_t* const buffer,
    size_t* bytes_per_row_ptr,
    size_t* index_ptr
) {
    sxbp_result_t error;
    /*
     * allocate two char arrays for the width and height strings - these may be
     * up to 10 characters each (max uint32_t is 10 digits long), so allocate 2
     * char arrays of 11 chars each (1 extra char for null-terminator)
     */
    char width_string[11], height_string[11];
    // we'll store the length of each string here
    size_t width_string_length, height_string_length = 0;
    // convert width and height to a decimal string, check for errors
    if (
        not sxbp_dimension_to_string(
            bitmap->width, &width_string, &width_string_length
        ) or
        not sxbp_dimension_to_string(
            bitmap->height, &height_string, &height_string_length
        )
    ) {
        // return I/O error code
        return SXBP_RESULT_FAIL_IO;
    } else {
        /*
         * now that we know the length of the image dimension strings, we can now
         * calculate how much memory we'll have to allocate for the image buffer
         */
        buffer->size = sxbp_get_pbm_image_size(
            bitmap->width,
            bitmap->height,
            width_string_length,
            height_string_length,
            bytes_per_row_ptr
        );
        // try and allocate the buffer
        if (not sxbp_check(sxbp_init_buffer(buffer), &error)) {
            // catch and return error if there was one
            return error;
        } else {
            // now with the buffer allocated, write the header
            size_t index = *index_ptr; // this index is used to index the buffer
            // construct magic number + whitespace
            memcpy(buffer->bytes + index, "P4\n", 3);
            index += 3;
            // image width
            memcpy(buffer->bytes + index, width_string, width_string_length);
            index += width_string_length;
            // whitespace
            memcpy(buffer->bytes + index, "\n", 1);
            index += 1;
            // image height
            memcpy(buffer->bytes + index, height_string, height_string_length);
            index += height_string_length;
            // whitespace
            memcpy(buffer->bytes + index, "\n", 1);
            index += 1;
            // update the index pointer
            *index_ptr = index;
        }
        return SXBP_RESULT_OK;
    }
}

// private, writes the image data out to the buffer
static void sxbp_write_pbm_data(
    const sxbp_bitmap_t* const bitmap,
    sxbp_buffer_t* const buffer,
    size_t bytes_per_row,
    size_t index
) {
    // now for the image data, packed into rows to the nearest byte
    for(size_t y = 0; y < bitmap->height; y++) { // row loop
        for(size_t x = 0; x < bitmap->width; x++) {
            // byte index is index + floor(x / 8)
            size_t byte_index = index + (x / 8);
            // bit index is x mod 8
            uint8_t bit_index = x % 8;
            // write bits most-significant-bit first
            buffer->bytes[byte_index] |= (
                // black pixel = bool true = 1, just like in PBM format
                bitmap->pixels[x][y] << (7 - bit_index)
            );
        }
        // increment index so next row is written in the correct place
        index += bytes_per_row;
    }
}

// private, serialises a bitmap to PBM image data
static sxbp_result_t sxbp_bitmap_to_pbm(
    const sxbp_bitmap_t* const bitmap,
    sxbp_buffer_t* const buffer
) {
    sxbp_result_t error;
    // index into the image data
    size_t index = 0;
    // we'll use this later to track how many bytes we pack each row into
    size_t bytes_per_row;
    /*
     * write the header first --this allocates the buffer to the size required
     * to store the whole image data too
     */
    if (
        not sxbp_check(
            sxbp_write_pbm_header(
                bitmap,
                buffer,
                &bytes_per_row,
                &index
            ),
            &error
        )
    ) {
        // catch and return error if there was one
        return error;
    } else {
        // memory allocation successful, now write out the image data
        sxbp_write_pbm_data(bitmap, buffer, bytes_per_row, index);
    }
    return SXBP_RESULT_OK;
}

/*
 * disable GCC warning about unused parameters, as this render backend doesn't
 * use all the arguments
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
sxbp_result_t sxbp_render_figure_to_pbm(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    // figure, figure lines and buffer must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(figure->lines);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    sxbp_result_t error;
    // rasterise the figure to bitmap first of all
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
    if (not sxbp_check(sxbp_render_figure_to_bitmap(figure, &bitmap), &error)) {
        // catch and return raised error
        return error;
    } else {
        // rasterisation was successful, now convert the raw bitmap to PBM
        sxbp_result_t outcome = sxbp_bitmap_to_pbm(&bitmap, buffer);
        // free the bitmap and return the result code
        sxbp_free_bitmap(&bitmap);
        return outcome;
    }
}
// reënable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
