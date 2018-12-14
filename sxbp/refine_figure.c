/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_refine_figure`, a
 * public function used to shorten the lines of an SXBP figure to something less
 * space-consuming, while still maintaining a pattern that has no collisions
 * between lines.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

sxbp_result_t sxbp_refine_figure(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    // figure and figure's lines must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(figure->lines);
    // work out which refinement method to use
    sxbp_refine_method_t method = SXBP_REFINE_METHOD_DEFAULT;
    if (options != NULL && options->refine_method != SXBP_REFINE_METHOD_ANY) {
        method = options->refine_method;
    }
    // now run the appropriate refinement function for all implemented methods
    switch (method) {
        case SXBP_REFINE_METHOD_GROW_FROM_START:
            return sxbp_refine_figure_grow_from_start(figure, options);
        case SXBP_REFINE_METHOD_SHRINK_FROM_END:
            return sxbp_refine_figure_shrink_from_end(figure, options);
        default:
            // an unimplemented refinement method was requested
            return SXBP_RESULT_FAIL_UNIMPLEMENTED;
    }
}
