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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C"{
#endif

bool sxbp_init_buffer(sxbp_buffer_t* buffer) {
    // allocate memory with calloc to make sure all bytes are set to zero
    buffer->bytes = calloc(buffer->size, sizeof(uint8_t));
    // if bytes is not NULL, then the operation was successful
    return buffer->bytes != NULL;
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

bool sxbp_copy_buffer(const sxbp_buffer_t* from, sxbp_buffer_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_buffer(to);
    // copy across the size
    to->size = from->size;
    // allocate the 'to' buffer
    if (!sxbp_init_buffer(to)) {
        // exit early if allocation failed
        return false;
    } else {
        // allocation succeeded, so now copy the data
        memcpy(to->bytes, from->bytes, to->size);
        return true;
    }
}

bool sxbp_init_figure(sxbp_figure_t* figure) {
    // allocate the lines, using calloc to set all fields of each one to zero
    figure->lines = calloc(figure->size, sizeof(sxbp_line_t));
    // if lines is not NULL, then the operation was successful
    return figure->lines != NULL;
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

bool sxbp_copy_figure(const sxbp_figure_t* from, sxbp_figure_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_figure(to);
    // copy across the static members
    to->size = from->size;
    to->lines_remaining = from->lines_remaining;
    // allocate the 'to' figure
    if (!sxbp_init_figure(to)) {
        // exit early if allocation failed
        return false;
    } else {
        // allocation succeeded, so now copy the lines
        memcpy(to->lines, from->lines, to->size);
        return true;
    }
}

// allocates memory for one row of a bitmap, returning whether it succeeded
static bool sxbp_init_bitmap_row(bool* row, uint32_t size) {
    // allocate row with calloc
    row = calloc(size, sizeof(bool));
    // if row is not NULL, then the operation was successful
    return row != NULL;
}

bool sxbp_init_bitmap(sxbp_bitmap_t* bitmap) {
    // first allocate pointers for the rows, using calloc to set each to NULL
    bitmap->pixels = calloc(bitmap->height, sizeof(bool*));
    if (bitmap->pixels == NULL) {
        // catch allocation error and exit early
        return false;
    } else {
        // allocation of row pointers succeeded, now try and allocate each row
        for (uint32_t row = 0; row < bitmap->height; row++) {
            if (!sxbp_init_bitmap_row(bitmap->pixels[row], bitmap->width)) {
                // allocation of one row failed, so de-allocate the whole bitmap
                sxbp_free_bitmap(bitmap);
                // indicate allocation failure
                return false;
            }
        }
        return true;
    }
}

bool sxbp_free_bitmap(sxbp_bitmap_t* bitmap) {
    // if pixels is not NULL, assume there are rows to be deallocated
    if (bitmap->pixels != NULL) {
        // deallocate each row that needs deallocating first
        for (uint32_t row = 0; row < bitmap->height; row++) {
            if (bitmap->pixels[row] != NULL) {
                free(bitmap->pixels[row]);
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
        for (uint32_t row = 0; row < to->height; row++) {
            memcpy(to->pixels[row], from->pixels[row], to->width);
        }
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
