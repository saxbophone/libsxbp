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
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    /*
     * allocate two char arrays for the width and height strings - these may be
     * up to 19 characters each (max uint64_t is 19 digits long), so allocate 2
     * char arrays of 20 chars each (1 extra char for null-terminator)
     */
    char width_string[20], height_string[20];
    // these are used to keep track of how many digits each is
    int width_string_length, height_string_length = 0;
    // convert width and height to a decimal string, store lengths
    width_string_length = sprintf(width_string, "%lu", bitmap.width);
    height_string_length = sprintf(height_string, "%lu", bitmap.height);
    /*
     * now that we know the length of the image dimension strings, we can now
     * calculate how much memory we'll have to allocate for the image buffer
     */
    // calculate number of bytes per row - this is ceiling(width / 8)
    size_t bytes_per_row = (size_t)ceil((double)bitmap.width / 8.0);
    // calculate number of bytes for the entire image pixels (rows and columns)
    size_t image_bytes = bytes_per_row * bitmap.height;
    // finally put it all together to get total image buffer size
    size_t image_buffer_size = (
        3 // "P4" magic number + whitespace
        + width_string_length + 1 // width of image in decimal + whitespace
        + height_string_length + 1 // height of image in decimal + whitespace
        + image_bytes // lastly, the bytes which make up the image pixels
    );
    // try and allocate the data for the buffer
    buffer->bytes = calloc(image_buffer_size, sizeof(uint8_t));
    // check fo memory allocation failure
    if(buffer->bytes == NULL) {
        return SXBP_MALLOC_REFUSED;
    } else {
        // set buffer size
        buffer->size = image_buffer_size;
        // otherwise carry on
        size_t index = 0; // this index is used to index the buffer
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
        // now for the image data, packed into rows to the nearest byte
        for(size_t y = 0; y < bitmap.height; y++) { // row loop
            for(size_t x = 0; x < bitmap.width; x++) {
                // byte index is index + floor(x / 8)
                size_t byte_index = index + (x / 8);
                // bit index is x mod 8
                uint8_t bit_index = x % 8;
                // write bits most-significant-bit first
                buffer->bytes[byte_index] |= (
                    bitmap.pixels[x][y] << (7 - bit_index)
                );
            }
            // increment index so next row is written in the correct place
            index += bytes_per_row;
        }
    }
    return SXBP_IMPOSSIBLE_CONDITION;
}

#ifdef __cplusplus
} // extern "C"
#endif
