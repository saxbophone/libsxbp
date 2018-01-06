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
#include <stdio.h>

#include "sxbp/sxbp.h"


#ifdef __cplusplus
extern "C"{
#endif

int main(void) {
    printf("This is SXBP v%s\n", SXBP_VERSION.string);
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
