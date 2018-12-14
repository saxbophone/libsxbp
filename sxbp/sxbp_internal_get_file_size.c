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

size_t sxbp_get_file_size(FILE* file_handle) {
    // preconditional assertions
    assert(file_handle != NULL);
    size_t file_size = 0;
    /*
     * conditional compilation, hurrah!
     * TODO: Check these macro constants, I literally made these ones up
     */
    #ifdef POSIX // POSIX implementation
    // TODO: POSIX implementation goes here
    assert(0 != 0);
    #endif // ifdef POSIX

    #ifndef POSIX
    #ifdef WIN32 // Windows implementation
    // TODO: Windows implementation goes here
    #endif // ifdef WIN32

    #ifndef WIN32 // Generic implementation
    /*
     * seek to end
     * NOTE: This isn't portable due to lack of meaningful support of `SEEK_END`
     */
    fseek(file_handle, 0, SEEK_END);
    // get size
    file_size = (size_t)ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0, SEEK_SET);
    #endif // ifndef WIN32
    #endif // ifndef POSIX

    // return the calculated file size
    return file_size;
}
