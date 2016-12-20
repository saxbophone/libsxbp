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
#include <assert.h>
// only include these extra dependencies if support for PNG output was enabled
#ifdef SAXBOSPIRAL_PNG_SUPPORT
#include <stdlib.h>
#include <string.h>
#endif

// only include libpng if support for it was enabled
#ifdef SAXBOSPIRAL_PNG_SUPPORT
#include <png.h>
#endif

#include "../saxbospiral.h"
#include "../render.h"
#include "backend_png.h"


#ifdef __cplusplus
extern "C"{
#endif

// only define the following private functions if libpng support was enabled
#ifdef SAXBOSPIRAL_PNG_SUPPORT
// private custom libPNG buffer write function
static void buffer_write_data(
    png_structp png_ptr, png_bytep data, png_size_t length
) {
    // retrieve pointer to buffer
    sxbp_buffer_t* p = (sxbp_buffer_t*)png_get_io_ptr(png_ptr);
    size_t new_size = p->size + length;
    // if buffer bytes pointer is not NULL, then re-allocate
    if(p->bytes != NULL) {
        p->bytes = realloc(p->bytes, new_size);
    } else {
        // otherwise, allocate
        p->bytes = malloc(new_size);
    }
    if(!p->bytes) {
        png_error(png_ptr, "Write Error");
    }
    // copy new bytes to end of buffer
    memcpy(p->bytes + p->size, data, length);
    p->size += length;
}

// disable GCC warning about the unused parameter, as this is a dummy function
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// dummy function for unecessary flush function
static void dummy_png_flush(png_structp png_ptr) {}
// re-enable all warnings
#pragma GCC diagnostic pop

// simple libpng cleanup function - used mainly for freeing memory
static void cleanup_png_lib(
    png_structp png_ptr, png_infop info_ptr, png_bytep row
) {
    if(info_ptr != NULL) {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    }
    if(png_ptr != NULL) {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    }
    if(row != NULL) {
        free(row);
    }
}
#endif // SAXBOSPIRAL_PNG_SUPPORT

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
) {
    // preconditional assertsions
    assert(bitmap.pixels != NULL);
    assert(buffer->bytes == NULL);
    // only do PNG operations if support is enabled
    #ifndef SAXBOSPIRAL_PNG_SUPPORT
    // return SXBP_NOT_IMPLEMENTED
    return SXBP_NOT_IMPLEMENTED;
    #else
    // result status
    sxbp_status_t result;
    // init buffer
    buffer->size = 0;
    // init libpng stuff
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    // allocate libpng memory
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    // catch malloc fail
    if(png_ptr == NULL) {
        result = SXBP_MALLOC_REFUSED;
        // cleanup
        cleanup_png_lib(png_ptr, info_ptr, row);
        return result;
    }
    // allocate libpng memory
    info_ptr = png_create_info_struct(png_ptr);
    // catch malloc fail
    if(info_ptr == NULL) {
        result = SXBP_MALLOC_REFUSED;
        // cleanup
        cleanup_png_lib(png_ptr, info_ptr, row);
        return result;
    }
    // set PNG write function - in this case, a function that writes to buffer
    png_set_write_fn(png_ptr, buffer, buffer_write_data, dummy_png_flush);
    // Write header - specify a 1-bit grayscale image with adam7 interlacing
    png_set_IHDR(
        png_ptr, info_ptr, bitmap.width, bitmap.height,
        1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
    );
    // configure bit packing - 1 bit gray channel
    png_color_8 sig_bit;
    sig_bit.gray = 1;
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);
    // Set image metadata
    png_text metadata[5]; // Author, Description, Copyright, Software, Comment
    metadata[0].key = "Author";
    metadata[0].text = "Joshua Saxby (https://github.com/saxbophone)";
    metadata[1].key = "Description";
    metadata[1].text = (
        "Experimental generation of 2D spiralling lines based on input binary "
        "data"
    );
    metadata[2].key = "Copyright";
    metadata[2].text = "Copyright Joshua Saxby";
    metadata[3].key = "Software";
    // LIBSXBP_VERSION_STRING is a macro that expands to a double-quoted string
    metadata[3].text = "libsxbp v" LIBSXBP_VERSION_STRING;
    metadata[4].key = "Comment";
    metadata[4].text = "https://github.com/saxbophone/libsxbp";
    // set compression of each metadata key
    for(size_t i = 0; i < 5; i++) {
        metadata[i].compression = PNG_TEXT_COMPRESSION_NONE;
    }
    // write metadata
    png_set_text(png_ptr, info_ptr, metadata, 5);
    png_write_info(png_ptr, info_ptr);
    // set bit shift - TODO: Check if this is acutally needed
    png_set_shift(png_ptr, &sig_bit);
    // set bit packing
    // NOTE: I'm pretty sure this bit is needed but worth checking
    png_set_packing(png_ptr);
    // Allocate memory for one row (1 byte per pixel - RGB)
    row = (png_bytep) malloc(bitmap.width * sizeof(png_byte));
    // catch malloc fail
    if(row == NULL) {
        result = SXBP_MALLOC_REFUSED;
        // cleanup
        cleanup_png_lib(png_ptr, info_ptr, row);
        return result;
    }
    // Write image data
    for(size_t y = 0 ; y < bitmap.height; y++) {
        for(size_t x = 0; x < bitmap.width; x++) {
            // set to black if there is a point here, white if not
            row[x] = (bitmap.pixels[x][y] == true) ? 0 : 1;
        }
       png_write_row(png_ptr, row);
    }
    // End write
    png_write_end(png_ptr, NULL);
    // cleanup
    cleanup_png_lib(png_ptr, info_ptr, row);
    // status ok
    result = SXBP_OPERATION_OK;
    return result;
    #endif // SAXBOSPIRAL_PNG_SUPPORT
}

#ifdef __cplusplus
} // extern "C"
#endif
