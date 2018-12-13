/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of core global symbols of sxbp.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stddef.h>

#include "sxbp.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// Version numbers are passed as preprocessor definitions by CMake
const sxbp_version_t SXBP_VERSION = {
    .major = SXBP_VERSION_MAJOR,
    .minor = SXBP_VERSION_MINOR,
    .patch = SXBP_VERSION_PATCH,
    .string = SXBP_VERSION_STRING,
};

const size_t SXBP_BEGIN_BUFFER_MAX_SIZE = 1073741823;

const sxbp_begin_figure_options_t SXBP_BEGIN_FIGURE_OPTIONS_DEFAULT = {
    .max_lines = 0,
};

const sxbp_refine_method_t SXBP_REFINE_METHOD_DEFAULT =
    SXBP_REFINE_METHOD_SHRINK_FROM_END;
