/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure`, a
 * public function used to render an SXBP figure to a basic bitmap structure.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>

#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

bool sxbp_render_figure(const sxbp_figure_t* figure, sxbp_bitmap_t* bitmap) {
    // erase the bitmap structure first just in case
    sxbp_free_bitmap(bitmap);
    // TODO: implement the rest of the rendering code
    return false;
}

#ifdef __cplusplus
} // extern "C"
#endif
