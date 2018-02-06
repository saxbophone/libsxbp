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
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

// private datatype for passing context data into sxbp_walk_figure() callback
typedef struct render_figure_context {
    sxbp_bitmap_t* image; // the bitmap to draw to
    /*
     * the following unusual fields facilitate a low-tech way of skipping the
     * second pixel --this is done to assist in orientation of the shape
     */
    bool first_pixel_complete; // whether the first pixel has been plotted yet
    bool second_pixel_complete; // whether the second pixel has been plotted yet
} render_figure_context;

// private, callback function for sxbp_render_figure()
static bool sxbp_render_figure_callback(
    sxbp_co_ord_t location,
    void* callback_data
) {
    // cast void pointer to a pointer to our context structure
    render_figure_context* data = (render_figure_context*)callback_data;
    // skip the plotting of the second pixel
    if (data->first_pixel_complete && !data->second_pixel_complete) {
        // mark second pixel as complete
        data->second_pixel_complete = true;
    } else {
        // mark first pixel as complete if it isn't already
        data->first_pixel_complete = true;
        // plot the pixel
        data->image->pixels[location.x][location.y] = true;
    }
    // return true --we always want to continue
    return true;
}

sxbp_result_t sxbp_render_figure(
    const sxbp_figure_t* figure,
    sxbp_bitmap_t* bitmap
) {
    // erase the bitmap structure first just in case
    sxbp_free_bitmap(bitmap);
    // get figure bounds, at scale 2
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 2);
    // build bitmap for bounds
    if (!sxbp_make_bitmap_for_bounds(bounds, bitmap)) {
        // couldn't allocate memory, return error early
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // construct callback context data
        render_figure_context data = {
            .image = bitmap,
            .first_pixel_complete = false,
            .second_pixel_complete = false,
        };
        // walk the figure at scale 2, handle pixel plotting with callback
        sxbp_walk_figure(figure, 2, sxbp_render_figure_callback, (void*)&data);
    }
    return SXBP_RESULT_OK;
}

#ifdef __cplusplus
} // extern "C"
#endif
