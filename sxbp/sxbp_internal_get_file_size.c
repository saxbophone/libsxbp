/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @internal
 * @file
 *
 * @brief This source file provides the implementation for a function which
 * tries to portably retrieve the size of a file, using OS-specific methods for
 * POSIX and Windows, falling back to the standard library (which ironically is
 * not portable due to the standard not requiring the host to meaningfully
 * support SEEK_END in the ftell() function) in all other cases.
 *
 * @author Joshua Saxby <joshua.a.saxby@gmail.com>
 * @date 2018
 *
 * @copyright Copyright (C) Joshua Saxby 2016-2017, 2018
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <stdlib.h>

#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// TODO: Check these macro constants and how to check two macro constants

// POSIX version
#ifdef POSIX
// we're gonna need fstat() for this!
#include <sys/stat.h>


size_t sxbp_get_file_size(FILE* file_handle) {
    // preconditional assertions
    assert(file_handle != NULL);
    // results from the fstat call are stored here
    struct stat file_info = { 0 };
    if (fstat(file_handle, &file_info) == 0) {
        // file size is file_info.st_size
        return (size_t)file_info.st_size;
    } else {
        /*
         * TODO: change function signature to make file size an out parameter,
         * so that success/failure may be returned directly.
         */
        return 0;
    }
}
#endif

// Windows version
#ifdef WIN32
// We're gonna need GetFileSizeEx() for this!
#include <Windows.h>


size_t sxbp_get_file_size(FILE* file_handle) {
    // preconditional assertions
    assert(file_handle != NULL);
    // the file's size will be stored here
    LARGE_INTEGER file_size;
    // call the Windows API to check the file size
    GetFileSizeEx(file_handle, &file_size);
    // LARGE_INTEGER is actually a struct...
    // TODO: work out which part to return
    // Oh... Windows API... :P
    return 0;
}
#endif

// Generic version
// NOTE: I don't think this is the correct way to check two macro constants
#ifndef POSIX
#ifndef WIN32
size_t sxbp_get_file_size(FILE* file_handle) {
    // preconditional assertions
    assert(file_handle != NULL);
    /*
     * seek to end
     * NOTE: This isn't portable due to lack of meaningful support of `SEEK_END`
     */
    fseek(file_handle, 0, SEEK_END);
    // get size
    size_t file_size = (size_t)ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0, SEEK_SET);
    // return the calculated file size
    return file_size;
}
#endif
#endif
