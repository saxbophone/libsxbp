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
#include <stdint.h>
#include <stdio.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// TODO: Check these macro constants for correctness on each OS

// POSIX version
#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
// we're gonna need fstat() for this!
#include <sys/stat.h>


sxbp_result_t sxbp_get_file_size(FILE* file_handle, size_t* file_size) {
    // preconditional assertions
    assert(file_handle != NULL);
    assert(file_size != NULL);
    // try and get a file descriptor for the file handle
    int file_descriptor = fileno(file_handle);
    if (file_descriptor == -1) {
        // getting a file descriptor failed
        return SXBP_RESULT_FAIL_IO;
    }
    // results from the fstat call are stored here
    struct stat file_info = { 0 };
    if (fstat(file_descriptor, &file_info) == 0) {
        // file size is file_info.st_size
        *file_size = (size_t)file_info.st_size;
        // return success
        return SXBP_RESULT_OK;
    } else {
        // return error code
        return SXBP_RESULT_FAIL_IO;
    }
}

// Windows version
#elif defined(_WIN32)
// We're gonna need GetFileSizeEx() for this!
#include <Windows.h>


sxbp_result_t sxbp_get_file_size(FILE* file_handle, size_t* file_size) {
    // preconditional assertions
    assert(file_handle != NULL);
    assert(file_size != NULL);
    // try and convert the file handle to a Windows file descriptor
    int file_descriptor = _fileno(file_handle);
    if (file_descriptor < 0) {
        // Windows API returns a negative code if it couldn't do the operation
        return SXBP_RESULT_FAIL_IO;
    }
    // try and convert the Windows file descriptor to a Windows HANDLE
    intptr_t windows_file_handle = _get_osfhandle(file_descriptor);
    if (windows_file_handle == INVALID_HANDLE_VALUE) {
        // Windows couldn't do it, return an error
        return SXBP_RESULT_FAIL_IO;
    }
    // the file's size from the Windows API call will be stored in this struct
    LARGE_INTEGER file_size_info;
    // call the Windows API to check the file size
    // TODO: error handling!
    GetFileSizeEx((HANDLE)windows_file_handle, &file_size_info);
    /*
     * LARGE_INTEGER is actually a struct of two DWORDS (32-bits) with an extra
     * member which is 64 bits on 64-bit systems.
     * As it happens, we don't care about files that are larger than 1GiB so if
     * the higher DWORD is set, we can return an error code instead as we don't
     * need it (can't use it for input to the algorithm)
     */
    if (file_size_info.HighPart != 0) {
        return SXBP_RESULT_FAIL_UNIMPLEMENTED;
    } else {
        *file_size = (size_t)file_size_info.LowPart;
        return SXBP_RESULT_OK;
    }
}

// Generic version
// NOTE: I don't think this is the correct way to check two macro constants
#else
sxbp_result_t sxbp_get_file_size(FILE* file_handle, size_t* file_size) {
    // preconditional assertions
    assert(file_handle != NULL);
    assert(file_size != NULL);
    /*
     * seek to end
     * NOTE: This isn't portable due to lack of meaningful support of `SEEK_END`
     */
    if (fseek(file_handle, 0, SEEK_END) != 0) {
        // handle fseek() error
        return SXBP_RESULT_FAIL_IO;
    }
    // get size and check it, before storing it or throwing an error
    int check_file_size = ftell(file_handle);
    if (check_file_size < 0) {
        // handle ftell() error
        return SXBP_RESULT_FAIL_IO;
    }
    *file_size = (size_t)check_file_size;
    // seek to start again
    fseek(file_handle, 0, SEEK_SET);
    // return the calculated file size
    return SXBP_RESULT_OK;
}
#endif
