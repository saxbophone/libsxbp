/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_evolve`, a public function which attempts to refine a
 * figure using an evolutionary algorithm to evolve a solution which is most
 * compact.
 *
 * NOTE: This algorithm is experimental!
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018-2019
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

sxbp_result_t sxbp_refine_figure_evolve(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    // XXX: allow dummy implementation to compile by calling the callback
    if (options != NULL && options->progress_callback != NULL) {
        options->progress_callback(figure, options->callback_context);
    }
    // do nothing, unsuccessfully
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
