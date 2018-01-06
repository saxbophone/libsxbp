/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
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

// Version numbers are passed as preprocessor definitions by CMake
const sxbp_version_t SXBP_VERSION = {
    .major = SXBP_VERSION_MAJOR,
    .minor = SXBP_VERSION_MINOR,
    .patch = SXBP_VERSION_PATCH,
    .string = SXBP_VERSION_STRING,
};

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

bool sxbp_copy_buffer(sxbp_buffer_t* from, sxbp_buffer_t* to) {
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

/*
 * disable GCC warning about the unused parameter, as figure doesn't have any
 * real members yet
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool sxbp_init_figure(sxbp_figure_t* figure) {
    // there's no members that need to be allocated yet so this can never fail
    return true;
}

bool sxbp_free_figure(sxbp_figure_t* figure) {
    // there are no members that need to be freed yet, so always return false
    return false;
}

bool sxbp_copy_figure(sxbp_figure_t* from, sxbp_figure_t* to) {
    // before we do anything else, make sure 'to' has been freed
    sxbp_free_figure(to);
    // TODO: copy across static members
    // allocate the 'to' figure
    if (!sxbp_init_figure(to)) {
        // exit early if allocation failed
        return false;
    } else {
        // allocation succeeded, so now copy the data
        // TODO: copy across dynamic members
        return true;
    }
}
// re-enable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
