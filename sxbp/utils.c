/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of functions for allocating,
 * freeing and copying the public data types of sxbp.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C"{
#endif

sxbp_buffer_t sxbp_blank_buffer(void) {
    return (sxbp_buffer_t){ .size = 0, .bytes = NULL, };
}

sxbp_result_t sxbp_init_buffer(sxbp_buffer_t* buffer) {
    // allocate memory with calloc to make sure all bytes are set to zero
    buffer->bytes = calloc(buffer->size, sizeof(uint8_t));
    // if bytes is not NULL, then the operation was successful
    return buffer->bytes != NULL ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_MEMORY;
}

bool sxbp_free_buffer(sxbp_buffer_t* buffer) {
    // if bytes is not NULL, assume there's memory to be deallocated
    if (buffer->bytes != NULL) {
        free(buffer->bytes);
        // set bytes to NULL (be a good person)
        buffer->bytes = NULL;
        return true;
    } else {
        // nothing to deallocate
        return false;
    }
}

sxbp_result_t sxbp_copy_buffer(const sxbp_buffer_t* from, sxbp_buffer_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_buffer(to);
    // copy across the size
    to->size = from->size;
    // allocate the 'to' buffer
    if (!sxbp_success(sxbp_init_buffer(to))) {
        // exit early if allocation failed - this can only be a memory error
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // allocation succeeded, so now copy the data
        memcpy(to->bytes, from->bytes, to->size);
        return SXBP_RESULT_OK;
    }
}

/*
 * private, works out and returns the size of the file referred to by the given
 * file handle
 */
static size_t sxbp_get_file_size(FILE* file_handle) {
    // seek to end
    // NOTE: This isn't portable due to lack of meaningful support of `SEEK_END`
    fseek(file_handle, 0, SEEK_END);
    // get size
    size_t file_size = ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0, SEEK_SET);
    return file_size;
}

sxbp_result_t sxbp_buffer_from_file(FILE* file_handle, sxbp_buffer_t* buffer) {
    // erase buffer
    sxbp_free_buffer(buffer);
    // get the file's size
    buffer->size = sxbp_get_file_size(file_handle);
    // allocate the buffer to this size and handle error if this failed
    if (!sxbp_success(sxbp_init_buffer(buffer))) {
        // allocation failed - this can only be a memory error
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // allocation succeeded, so read the file contents into the buffer
        size_t bytes_read = fread(
            buffer->bytes,
            sizeof(uint8_t),
            buffer->size,
            file_handle
        );
        /*
         * check that the correct number of bytes were read and exit with error
         * if it doesn't match the reported file size
         */
        if (bytes_read != buffer->size) {
            // we didn't read the same number of bytes as the file's size
            sxbp_free_buffer(buffer);
            // return a file error
            return SXBP_RESULT_FAIL_FILE;
        } else {
            // we read the buffer successfully, so return success
            return SXBP_RESULT_OK;
        }
    }
}

sxbp_result_t sxbp_buffer_to_file(
    const sxbp_buffer_t* buffer,
    FILE* file_handle
) {
    // try and write the file contents
    size_t bytes_written = fwrite(
        buffer->bytes,
        sizeof(uint8_t),
        buffer->size,
        file_handle
    );
    // return success/failure if the correct number of bytes were written
    return bytes_written == buffer->size ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_FILE;
}

sxbp_figure_t sxbp_blank_figure(void) {
    return (sxbp_figure_t){ .size = 0, .lines = NULL, .lines_remaining = 0, };
}

sxbp_result_t sxbp_init_figure(sxbp_figure_t* figure) {
    // allocate the lines, using calloc to set all fields of each one to zero
    figure->lines = calloc(figure->size, sizeof(sxbp_line_t));
    // if lines is not NULL, then the operation was successful
    return figure->lines != NULL ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_MEMORY;
}

bool sxbp_free_figure(sxbp_figure_t* figure) {
    // if lines is not NULL, assume there's memory to be deallocated
    if (figure->lines != NULL) {
        free(figure->lines);
        // set lines to NULL (be a good person)
        figure->lines = NULL;
        return true;
    } else {
        // nothing to deallocate
        return false;
    }
}

sxbp_result_t sxbp_copy_figure(const sxbp_figure_t* from, sxbp_figure_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_figure(to);
    // copy across the static members
    to->size = from->size;
    to->lines_remaining = from->lines_remaining;
    // allocate the 'to' figure
    if (!sxbp_init_figure(to)) {
        // exit early if allocation failed - this can only be a memory error
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // allocation succeeded, so now copy the lines
        memcpy(to->lines, from->lines, to->size);
        return SXBP_RESULT_OK;
    }
}

sxbp_bitmap_t sxbp_blank_bitmap(void) {
    return (sxbp_bitmap_t){ .width = 0, .height = 0, .pixels = NULL, };
}

// allocates memory for one column of a bitmap, returning whether it succeeded
static bool sxbp_init_bitmap_col(bool** col, uint32_t size) {
    // allocate col with calloc
    *col = calloc(size, sizeof(bool));
    // if col is not NULL, then the operation was successful
    return *col != NULL;
}

bool sxbp_init_bitmap(sxbp_bitmap_t* bitmap) {
    // first allocate pointers for the columns
    bitmap->pixels = calloc(bitmap->width, sizeof(bool*));
    if (bitmap->pixels == NULL) {
        // catch allocation error and exit early
        return false;
    } else {
        // allocation of col pointers succeeded, now try and allocate each col
        for (uint32_t col = 0; col < bitmap->width; col++) {
            if (!sxbp_init_bitmap_col(&bitmap->pixels[col], bitmap->height)) {
                // allocation of one col failed, so de-allocate the whole bitmap
                sxbp_free_bitmap(bitmap);
                // indicate allocation failure
                return false;
            }
        }
        return true;
    }
}

bool sxbp_free_bitmap(sxbp_bitmap_t* bitmap) {
    // if pixels is not NULL, assume there are cols to be deallocated
    if (bitmap->pixels != NULL) {
        // deallocate each col that needs deallocating first
        for (uint32_t col = 0; col < bitmap->width; col++) {
            if (bitmap->pixels[col] != NULL) {
                free(bitmap->pixels[col]);
            }
        }
        // finally, deallocate the rows pointer
        free(bitmap->pixels);
        return true;
    } else {
        return false;
    }
}

bool sxbp_copy_bitmap(const sxbp_bitmap_t* from, sxbp_bitmap_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_bitmap(to);
    // copy across width and height
    to->width = from->width;
    to->height = from->height;
    // allocate the 'to' bitmap
    if (!sxbp_init_bitmap(to)) {
        // exit early if allocation failed
        return false;
    } else {
        // allocation succeeded, so now copy the pixels
        for (uint32_t col = 0; col < to->width; col++) {
            memcpy(to->pixels[col], from->pixels[col], to->height);
        }
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
