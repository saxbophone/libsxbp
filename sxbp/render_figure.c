/*
 * This source file forms part of sxbp,
 a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_render_figure`, a public function used to render an SXBP figure to an
 * image of its shape, using a user-specified rendering callback.
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

sxbp_result_t sxbp_render_figure(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    sxbp_figure_renderer_t render_callback,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    // figure, buffer and render_callback must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    SXBP_RETURN_FAIL_IF_NULL(render_callback);
    // use the callback to render the figure
    return render_callback(
        figure,
        buffer,
        render_options,
        render_callback_options
    );
}

#ifdef __cplusplus
} // extern "C"
#endif