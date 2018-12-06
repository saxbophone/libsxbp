/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of functions for allocating,
 * freeing and copying the public data types of sxbp and those for checking
 * the error codes returned by certain functions in sxbp.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C"{
#endif

bool sxbp_success(sxbp_result_t state) {
    // return whether state was 'OK or not'
    return state == SXBP_RESULT_OK;
}

bool sxbp_check(sxbp_result_t state, sxbp_result_t* const report_to) {
    // return true immediately if the state is 'OK'
    if (sxbp_success(state)) {
        return true;
    } else {
        // otherwise, store it in the location at `report_to` if not NULL
        if (report_to != NULL) {
            *report_to = state;
        }
        // return false to indicate some kind of error occurred
        return false;
    }
}

sxbp_buffer_t sxbp_blank_buffer(void) {
    return (sxbp_buffer_t){ .size = 0, .bytes = NULL, };
}

sxbp_result_t sxbp_init_buffer(sxbp_buffer_t* const buffer) {
    // check buffer isn't NULL
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    // allocate memory with calloc to make sure all bytes are set to zero
    buffer->bytes = calloc(buffer->size, sizeof(uint8_t));
    // if bytes is not NULL, then the operation was successful
    return buffer->bytes != NULL ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_MEMORY;
}

sxbp_result_t sxbp_resize_buffer(sxbp_buffer_t* const buffer, size_t size) {
    // check buffer and buffer->bytes aren't NULL
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    SXBP_RETURN_FAIL_IF_NULL(buffer->bytes);
    // use realloc() to try and re-allocate the memory to the new requested size
    void* requested_memory = realloc(buffer->bytes, size * sizeof(uint8_t));
    // if the reallocation request was successful
    if (requested_memory != NULL) {
        // re-assign buffer->bytes to point to the newly allocated memory region
        buffer->bytes = requested_memory;
        // update the buffer's size
        buffer->size = size;
        // return success code
        return SXBP_RESULT_OK;
    } else {
        /*
         * otherwise, we don't need to touch the bytes --original memory is
         * intact, we just weren't able to change its size. return failure code
         */
        return SXBP_RESULT_FAIL_MEMORY;
    }
}

bool sxbp_free_buffer(sxbp_buffer_t* const buffer) {
    // if buffer and bytes are not NULL, assume there's memory to be deallocated
    if (buffer != NULL and buffer->bytes != NULL) {
        free(buffer->bytes);
        // set bytes to NULL (be a good person)
        buffer->bytes = NULL;
        return true;
    } else {
        // nothing to deallocate
        return false;
    }
}

sxbp_result_t sxbp_copy_buffer(
    const sxbp_buffer_t* const from,
    sxbp_buffer_t* const to
) {
    // check both pointers to ensure they're not NULL
    SXBP_RETURN_FAIL_IF_NULL(from);
    SXBP_RETURN_FAIL_IF_NULL(to);
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_buffer(to);
    // copy across the size
    to->size = from->size;
    // allocate the 'to' buffer
    if (not sxbp_success(sxbp_init_buffer(to))) {
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
    size_t file_size = (size_t)ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0, SEEK_SET);
    return file_size;
}

sxbp_result_t sxbp_buffer_from_file(
    FILE* file_handle,
    sxbp_buffer_t* const buffer
) {
    // check both pointers to ensure they're not NULL
    SXBP_RETURN_FAIL_IF_NULL(file_handle);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    // erase buffer
    sxbp_free_buffer(buffer);
    // get the file's size
    buffer->size = sxbp_get_file_size(file_handle);
    // allocate the buffer to this size and handle error if this failed
    if (not sxbp_success(sxbp_init_buffer(buffer))) {
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
            return SXBP_RESULT_FAIL_IO;
        } else {
            // we read the buffer successfully, so return success
            return SXBP_RESULT_OK;
        }
    }
}

sxbp_result_t sxbp_buffer_to_file(
    const sxbp_buffer_t* const buffer,
    FILE* file_handle
) {
    // check both pointers to ensure they're not NULL
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    SXBP_RETURN_FAIL_IF_NULL(file_handle);
    // try and write the file contents
    size_t bytes_written = fwrite(
        buffer->bytes,
        sizeof(uint8_t),
        buffer->size,
        file_handle
    );
    // return success/failure if the correct number of bytes were written
    return bytes_written == buffer->size ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_IO;
}

sxbp_figure_t sxbp_blank_figure(void) {
    return (sxbp_figure_t){ .size = 0, .lines = NULL, .lines_remaining = 0, };
}

sxbp_result_t sxbp_init_figure(sxbp_figure_t* const figure) {
    // check figure isn't NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    // allocate the lines, using calloc to set all fields of each one to zero
    figure->lines = calloc(figure->size, sizeof(sxbp_line_t));
    // if lines is not NULL, then the operation was successful
    return figure->lines != NULL ? SXBP_RESULT_OK : SXBP_RESULT_FAIL_MEMORY;
}

bool sxbp_free_figure(sxbp_figure_t* const figure) {
    // if figure and lines are not NULL, assume there's memory to be deallocated
    if (figure != NULL and figure->lines != NULL) {
        free(figure->lines);
        // set lines to NULL (be a good person)
        figure->lines = NULL;
        return true;
    } else {
        // nothing to deallocate
        return false;
    }
}

sxbp_result_t sxbp_copy_figure(
    const sxbp_figure_t* const from,
    sxbp_figure_t* const to
) {
    // check both pointers to ensure they're not NULL
    SXBP_RETURN_FAIL_IF_NULL(from);
    SXBP_RETURN_FAIL_IF_NULL(to);
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_figure(to);
    // copy across the static members
    to->size = from->size;
    to->lines_remaining = from->lines_remaining;
    // allocate the 'to' figure
    if (not sxbp_init_figure(to)) {
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
static bool sxbp_init_bitmap_col(bool** col, sxbp_figure_dimension_t size) {
    // allocate col with calloc
    *col = calloc(size, sizeof(bool));
    // if col is not NULL, then the operation was successful
    return *col != NULL;
}

sxbp_result_t sxbp_init_bitmap(sxbp_bitmap_t* const bitmap) {
    // check bitmap isn't NULL
    SXBP_RETURN_FAIL_IF_NULL(bitmap);
    // first allocate pointers for the columns
    bitmap->pixels = calloc(bitmap->width, sizeof(bool*));
    if (bitmap->pixels == NULL) {
        // catch allocation error and exit early
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // allocation of col pointers succeeded, now try and allocate each col
        for (sxbp_figure_dimension_t col = 0; col < bitmap->width; col++) {
            if (
                not sxbp_success(
                    sxbp_init_bitmap_col(&bitmap->pixels[col], bitmap->height)
                )
            ) {
                // allocation of one col failed, so de-allocate the whole bitmap
                sxbp_free_bitmap(bitmap);
                // indicate allocation failure
                return SXBP_RESULT_FAIL_MEMORY;
            }
        }
        return SXBP_RESULT_OK;
    }
}

bool sxbp_free_bitmap(sxbp_bitmap_t* const bitmap) {
    // if bitmap and pixels aren't NULL, assume there are cols to be deallocated
    if (bitmap != NULL and bitmap->pixels != NULL) {
        // deallocate each col that needs deallocating first
        for (sxbp_figure_dimension_t col = 0; col < bitmap->width; col++) {
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

sxbp_result_t sxbp_copy_bitmap(
    const sxbp_bitmap_t* const from,
    sxbp_bitmap_t* const to
) {
    // check both pointers to ensure they're not NULL
    SXBP_RETURN_FAIL_IF_NULL(from);
    SXBP_RETURN_FAIL_IF_NULL(to);
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_bitmap(to);
    // copy across width and height
    to->width = from->width;
    to->height = from->height;
    // allocate the 'to' bitmap
    if (not sxbp_success(sxbp_init_bitmap(to))) {
        // exit early if allocation failed - this can only be a memory error
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // allocation succeeded, so now copy the pixels
        for (sxbp_figure_dimension_t col = 0; col < to->width; col++) {
            memcpy(to->pixels[col], from->pixels[col], to->height);
        }
        return SXBP_RESULT_OK;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
