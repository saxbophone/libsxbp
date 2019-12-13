/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_figure_collides`, a private function providing collision-detection and
 * potential future collision-detection for a given sxbp figure.
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

/**
 * @brief Type for representing the outcome of collision-detection.
 * @details `sxbp_figure_collides()` no longer just checks if the figure,
 * collides, it now also checks if the figure can continue for another line,
 * i.e. if it can be guaranteed to collide if another line is plotted or not.
 * @since v0.54.0
 */
typedef enum sxbp_CollisionResult {
    SXBP_COLLISION_RESULT_COLLIDES, /**< The figure collides */
    SXBP_COLLISION_RESULT_TERMINATES, /**< The figure will collide next line */
    SXBP_COLLISION_RESULT_CONTINUES, /**< The figure does not collide next line */
} sxbp_CollisionResult;

/*
 * private, sets `status` to one of its possible values depending on whether the
 * given figure collides currently, or does not currently but is guaranteed to
 * collide if another line is plotted, or does not currently and cannot be
 * guaranteed to collide if another line is plotted.
 * if detect_terminals is true, will enable the detection of terminal solutions,
 * in which case status may be set to TERMINATES. If not, then terminals are
 * classed as CONTINUES
 */
sxbp_Result sxbp_figure_collides(
    const sxbp_Figure* figure,
    sxbp_CollisionResult* status,
    bool detect_terminals
);
