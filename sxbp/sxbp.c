/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of core global symbols of sxbp,
 * and a function for checking the return status of other functions.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stddef.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

// Version numbers are passed as preprocessor definitions by CMake
const sxbp_version_t SXBP_VERSION = {
    .major = SXBP_VERSION_MAJOR,
    .minor = SXBP_VERSION_MINOR,
    .patch = SXBP_VERSION_PATCH,
    .string = SXBP_VERSION_STRING,
};

bool sxbp_success(sxbp_result_t state) {
    // return whether state was 'OK or not'
    return state == SXBP_RESULT_OK;
}

bool sxbp_check(sxbp_result_t state, sxbp_result_t* report_to) {
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

#ifdef __cplusplus
} // extern "C"
#endif
