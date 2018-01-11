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
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "sxbp/sxbp.h"


#ifdef __cplusplus
extern "C"{
#endif

int main(void) {
    printf("This is SXBP v%s\n", SXBP_VERSION.string);
    const char* string = "SXBP";
    size_t length = strlen(string);
    sxbp_buffer_t buffer = { .size = length, .bytes = NULL, };
    if (!sxbp_init_buffer(&buffer)) {
        return -1;
    } else {
        memcpy(buffer.bytes, string, length);
        sxbp_figure_t figure = { 0 };
        sxbp_begin_figure(&buffer, &figure);
        sxbp_refine_figure(&figure);
        sxbp_free_buffer(&buffer);
        return 0;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
