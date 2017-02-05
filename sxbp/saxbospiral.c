/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) 2016, 2017, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// Version numbers are passed as preprocessor definitions by CMake
const sxbp_version_t LIB_SXBP_VERSION = {
    .major = LIBSXBP_VERSION_MAJOR,
    .minor = LIBSXBP_VERSION_MINOR,
    .patch = LIBSXBP_VERSION_PATCH,
    .string = LIBSXBP_VERSION_STRING,
};

bool sxbp_version_less_than(sxbp_version_t a, sxbp_version_t b) {
    if(a.major < b.major) {
        return true;
    }
    if((a.major == b.major) && (a.minor < b.minor)) {
        return true;
    }
    if ((a.major == b.major) && (a.minor == b.minor) && (a.patch < b.patch)) {
        return true;
    }
    return false;
}

bool sxbp_version_greater_than(sxbp_version_t a, sxbp_version_t b) {
    if(a.major > b.major) {
        return true;
    }
    if((a.major == b.major) && (a.minor > b.minor)) {
        return true;
    }
    if ((a.major == b.major) && (a.minor == b.minor) && (a.patch > b.patch)) {
        return true;
    }
    return false;
}

bool sxbp_version_equal_to(sxbp_version_t a, sxbp_version_t b) {
    return (
        (a.major == b.major) && (a.minor == b.minor) && (a.patch == b.patch)
    );
}

// vector direction constants
const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4] = {
    // UP       RIGHT       DOWN        LEFT
    { 0, 1, }, { 1, 0, }, { 0, -1, }, { -1, 0, },
};

#ifdef __cplusplus
} // extern "C"
#endif
