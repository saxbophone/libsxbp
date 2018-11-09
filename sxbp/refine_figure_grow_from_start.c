/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_grow_from_start`, a public function providing a specific
 * algorithm for refining a figure by attempting to grow the lines before those
 * that collide until the collision stops, doing this recursively as required.
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
extern "C" {
#endif

// disable GCC warning about the unused parameter, as this is currently a stub
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/*
 * Algorithm:
 *
 * - FOR all lines, counting forwards:
 *     - SetLineLength(CurrentLine, 1)
 *
 * - SetLineLength(line, length)
 *     - Try and SET 'line' length to 'length'
 *     - IF this causes a collision:
 *         - NOTE the line that it collided with (CollidedLine)
 *         - SetLineLength(
 *               PreviousLine, EvadeDistance(CurrentLine, CollidedLine)
 *           )
 *
 * - EvadeDistance(PreviousLine, CurrentLine, CollidedLine)
 *     - COMPARE PreviousLine, CurrentLine and CollidedLine lengths, directions
 *       and positions to work out what length PreviousLine should be in order
 *       for CurrentLine to not collide with CollidedLine
 *     - RETURN new distance for PreviousLine
 */
sxbp_result_t sxbp_refine_figure_grow_from_start(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    // FIXME: This function is currently unimplemented!
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
// reënable all warnings
#pragma GCC diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif
