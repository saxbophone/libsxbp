/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure_to_svg`
 * , a public function used to render an SXBP figure to an SVG file buffer.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * disable GCC warning about unused parameters, as this dummy function doesn't
 * do anything with its arguments
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
sxbp_result_t sxbp_render_figure_to_svg(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    // figure, figure lines and buffer must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(figure->lines);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    /*
     * because SVG is a vector-based format, this backend differs from the
     * others as we don't need to plot a bunch of pixels, instead we need to use
     * sxbp_walk_figure() to draw an SVG <polyline> element.
     * the setup process for this is similar to that in
     * `sxbp_render_figure_to_bitmap()` however, as the image we produce still
     * needs to have the same dimensions and scale (including the 1px border).
     */
    // get figure bounds, at scale 2
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 2);
    /*
     * calculate width and height of the image from the bounds
     * (see ... for details)
     */
    uint32_t width = (uint32_t)((bounds.x_max - bounds.x_min) + 1);
    uint32_t height = (uint32_t)((bounds.y_max - bounds.y_min) + 1);
    // TODO: write image header, including everything up to the line's points
    // ...
    // TODO: use sxbp_walk_figure() to write all the line's points
    // ...
    // TODO: write the image tail (close polyline quote, tag, closing svg tag)
    // ...
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
// reënable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
