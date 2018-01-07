/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of `sxbp_begin_figure`, a
 * public function used to build an unrefined SXBP figure from input data.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "sxbp.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private enum type for representing rotation direction
 * This makes the maths for discerning the line directions read particularly
 * well and intuitively.
 */
typedef enum sxbp_rotation_t {
    SXBP_ANTI_CLOCKWISE = -1, // The rotational direction 'ANTI-CLOCKWISE'
    SXBP_CLOCKWISE = 1, // The rotational direction 'CLOCKWISE'
} sxbp_rotation_t;

/*
 * returns the new cartesian direction that will be faced after turning from
 * the given cartesian direction by the given rotational direction
 */
static sxbp_direction_t sxbp_change_line_direction(
    sxbp_direction_t current, sxbp_rotation_t turn
) {
    return (current + turn) % 4;
}

bool sxbp_begin_figure(const sxbp_buffer_t* data, sxbp_figure_t* figure) {
    // erase the figure first to ensure it's blank
    sxbp_free_figure(figure);
    /*
     * the number of lines is the number of bits in the buffer (byte count * 8)
     * + 1 (for the extra starting line)
     */
    figure->size = data->size * 8 + 1;
    // allocate memory for the figure
    if (!sxbp_init_figure(figure)) {
        // exit early and signal error status
        return false;
    } else {
        // allocation succeeded, now populate the lines
        // TODO: Populate lines
        return true;
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
