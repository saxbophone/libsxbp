/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_render_figure_to_null`
 * , a dummy renderer callback function that does nothing.
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

/*
 * disable GCC warning about unused parameters, as this dummy function doesn't
 * do anything with its arguments
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
sxbp_result_t sxbp_render_figure_to_null(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
) {
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
// reënable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
