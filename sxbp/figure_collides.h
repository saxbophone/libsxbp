/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_shrink_from_end`, a public function providing a specific
 * algorithm for refining a figure by attemptin to shrink all the lines from
 * their safe 'default' lengths (as plotted by `sxbp_begin_figure`) to the
 * shortest length possible, starting from the end and working backwards.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018-2019
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

// private, sets collided to true if the figure's line collides with itself
sxbp_Result sxbp_figure_collides(
    const sxbp_Figure* figure,
    bool* collided
);
