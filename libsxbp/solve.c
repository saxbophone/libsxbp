/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for calculating the length of all
 * lines in a spiral such that the length of each line is at least 1 and none
 * of them collide.
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License (version 3),
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "saxbospiral.h"
#include "plot.h"
#include "solve.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * private function, given a pointer to a spiral struct and the index of the
 * highest line to use, check if the latest line would collide with any of the
 * others, given their current directions and jump sizes (using co-ords stored
 * in cache).
 * NOTE: This assumes that all lines except the most recent are valid and
 * don't collide.
 * Returns boolean on whether or not the spiral collides or not. Also, sets the
 * collider field in the spiral struct to the index of the colliding line
 * (if any)
 *
 * Asserts:
 * - That spiral->lines is not NULL
 * - That spiral->co_ord_cache.co_ords.items is not NULL
 * - That index is less than spiral->size
 */
static bool spiral_collides(sxbp_spiral_t* spiral, size_t index) {
    // preconditional assertions
    assert(spiral->lines != NULL);
    assert(spiral->co_ord_cache.co_ords.items != NULL);
    assert(index < spiral->size);
    /*
     * if there are less than 4 lines in the spiral, then there's no way it
     * can collide, so return false early
     */
    if(spiral->size < 4) {
        return false;
    } else {
        // initialise a counter to keep track of what line we're on
        uint64_t line_count = 0;
        uint64_t ttl = spiral->lines[line_count].length + 1; // ttl of line
        size_t last_co_ord = spiral->co_ord_cache.co_ords.size;
        sxbp_line_t last_line = spiral->lines[index];
        uint64_t start_of_last_line = (last_co_ord - last_line.length) - 1;
        // check the co-ords of the last line segment against all the others
        for(uint64_t i = 0; i < start_of_last_line; i++) {
            for(size_t j = start_of_last_line; j < last_co_ord; j++) {
                if(
                    (
                        spiral->co_ord_cache.co_ords.items[i].x ==
                        spiral->co_ord_cache.co_ords.items[j].x
                    )
                    &&
                    (
                        spiral->co_ord_cache.co_ords.items[i].y ==
                        spiral->co_ord_cache.co_ords.items[j].y
                    )
                ) {
                    spiral->collider = line_count;
                    return true;
                }
            }
            // update ttl (and counter if needed)
            ttl--;
            if(ttl == 0) {
                line_count++;
                ttl = spiral->lines[line_count].length;
            }
            /*
             * terminate the loop if the next line would be the line 2 lines
             * before the last one (these two lines can never collide with the
             * last and can be safely ignored, for a small performance increase)
             */
            if(line_count == (spiral->size - 2 - 1)) { // -1 for zero-index
                break;
            }
        }
        return false;
    }
}

/*
 * given a spiral struct that is known to collide, the index of the 'last'
 * segment in the spiral (i.e. the one that was found to be colliding) and a
 * perfection threshold (0 for no perfection, or otherwise the maximmum line
 * length at which to allow aggressive optimisation), return a suggested length
 * to set the segment before this line to.
 *
 * NOTE: This function is not guaranteed to make suggestions that will not
 * collide. Every suggestion that is followed should then have the spiral
 * re-evaluated for collisions before doing any more work.
 *
 * NOTE: This function does not *need* to be called with spirals that collide,
 * but it is pointless to call this function with a spiral that does not collide
 *
 * NOTE: In the context of this function, 'rigid' or 'r' refers to the line that
 * the newly plotted line has collided with and 'previous' or 'p' refers to the
 * line before the newly plotted line.
 *
 * Asserts:
 * - That spiral.lines is not NULL
 * - That spiral.co_ord_cache.co_ords.items is not NULL
 * - That index is less than spiral.size
 */
static sxbp_length_t suggest_resize(
    sxbp_spiral_t spiral, size_t index, sxbp_length_t perfection_threshold
) {
    // preconditional assertions
    assert(spiral.lines != NULL);
    assert(spiral.co_ord_cache.co_ords.items != NULL);
    assert(index < spiral.size);
    // check if collides or not, return same size if no collision
    if(spiral.collides) {
        /*
         * if the perfection threshold is 0, then we can just use our
         * suggestion, as perfection is disabled.
         * otherwise, if the colliding line's length is greater than our
         * perfection threshold, we cannot make any intelligent suggestions on
         * the length to extend the previous line to (without the high
         * likelihood of creating a line that wastes space), so we just return
         * the previous line's length +1
         */
        if(
            (perfection_threshold > 0) &&
            (spiral.lines[index].length > perfection_threshold)
        ) {
            return spiral.lines[index - 1].length + 1;
        }
        // store the 'previous' and 'rigid' lines.
        sxbp_line_t p = spiral.lines[index - 1];
        sxbp_line_t r = spiral.lines[spiral.collider];
        // if pr and r are not parallel, we can return early
        if((p.direction % 2) != (r.direction % 2)) {
            return spiral.lines[index - 1].length + 1;
        }
        // create variables to store the start and end co-ords of these lines
        sxbp_co_ord_t pa, ra, rb;
        /*
         * We need to grab the start and end co-ords of the line previous to the
         * colliding line, and the rigid line that it collided with.
         */
        size_t p_index = sxbp_sum_lines(spiral, 0, index - 1);
        size_t r_index = sxbp_sum_lines(spiral, 0, spiral.collider);
        pa = spiral.co_ord_cache.co_ords.items[p_index];
        ra = spiral.co_ord_cache.co_ords.items[r_index];
        rb = spiral.co_ord_cache.co_ords.items[r_index + r.length];
        /*
         * Apply the rules mentioned in collision_resolution_rules.txt to
         * calculate the correct length to set the previous line and return it.
         */
        if((p.direction == SXBP_UP) && (r.direction == SXBP_UP)) {
            return (ra.y - pa.y) + r.length + 1;
        } else if((p.direction == SXBP_UP) && (r.direction == SXBP_DOWN)) {
            return (rb.y - pa.y) + r.length + 1;
        } else if((p.direction == SXBP_RIGHT) && (r.direction == SXBP_RIGHT)) {
            return (ra.x - pa.x) + r.length + 1;
        } else if((p.direction == SXBP_RIGHT) && (r.direction == SXBP_LEFT)) {
            return (rb.x - pa.x) + r.length + 1;
        } else if((p.direction == SXBP_DOWN) && (r.direction == SXBP_UP)) {
            return (pa.y - rb.y) + r.length + 1;
        } else if((p.direction == SXBP_DOWN) && (r.direction == SXBP_DOWN)) {
            return (pa.y - ra.y) + r.length + 1;
        } else if((p.direction == SXBP_LEFT) && (r.direction == SXBP_RIGHT)) {
            return (pa.x - rb.x) + r.length + 1;
        } else if((p.direction == SXBP_LEFT) && (r.direction == SXBP_LEFT)) {
            return (pa.x - ra.x) + r.length + 1;
        } else {
            // this is the catch-all case, where no way to optimise was found
            return spiral.lines[index - 1].length + 1;
        }
    } else {
        /*
         * If we got here, then no collisions could be found, which means we
         * don't have to extend the previous segment.
         */
        return spiral.lines[index - 1].length;
    }
}

/*
 * given a pointer to a spiral struct, the index of one of it's lines and a
 * target length to set that line to and a perfection threshold (0 for no
 * perfection, or otherwise the maximmum line length at which to allow
 * aggressive optimisation) attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 * returns a status struct (used for error information)
 *
 * Asserts:
 * - That spiral->lines is not NULL
 * - That index is less than spiral->size
 */
sxbp_status_t sxbp_resize_spiral(
    sxbp_spiral_t* spiral, uint64_t index, sxbp_length_t length,
    sxbp_length_t perfection_threshold
) {
    // preconditional assertions
    assert(spiral->lines != NULL);
    assert(index < spiral->size);
    /*
     * setup state variables, these are used in place of recursion for managing
     * state of which line is being resized, and what size it should be.
     */
    // set result status
    sxbp_status_t result;
    size_t current_index = index;
    sxbp_length_t current_length = length;
    while(true) {
        // set the target line to the target length
        spiral->lines[current_index].length = current_length;
        /*
         * also, set cache validity to this index so we invalidate any invalid
         * entries in the co-ord cache
         */
        spiral->co_ord_cache.validity = (
            current_index < spiral->co_ord_cache.validity
        ) ? current_index : spiral->co_ord_cache.validity;
        // update the spiral's co-ord cache, and catch any errors
        result = sxbp_cache_spiral_points(spiral, current_index + 1);
        // return if errors
        if(result != SXBP_OPERATION_OK) {
            return result;
        }
        spiral->collides = spiral_collides(spiral, current_index);
        if(spiral->collides) {
            /*
             * if we've caused a collision, we need to call the suggest_resize()
             * function to get the suggested length to resize the previous
             * segment to
             */
            current_length = suggest_resize(
                *spiral, current_index, perfection_threshold
            );
            current_index--;
        } else if(current_index != index) {
            /*
             * if we didn't cause a collision but we're not on the top-most
             * line, then we've just resolved a collision situation.
             * we now need to work on the next line and start by setting to 1.
             */
            current_index++;
            current_length = 1;
        } else {
            /*
             * if we're on the top-most line and there's no collision
             * this means we've finished! Set solved_count to this index+1
             * Return OPERATION_OK from function.
             */
            spiral->solved_count = index + 1;
            result = SXBP_OPERATION_OK;
            return result;
        }
    }
}

/*
 * given a pointer to a spiral spiral for which the length of all its lines are
 * not yet known, a perfection threshold (0 for no perfection, or otherwise
 * the maximmum line length at which to allow aggressive optimisation), the
 * index of the highest line to plot to, a pointer to a callback function and
 * a void pointer to a user-defined data struct for use with the callback,
 * calculate the length needed for each line in the spiral up to this index
 * (to avoid line overlap) and store these in a the spiral struct that is
 * pointed to by the pointer. If the spiral has some lines already solved, the
 * algorithm will start at the next unsolved line.
 * the function pointer and the user data pointer can be NULL.
 * If the function pointer is not NULL, then it will be called every time a new
 * line of the spiral is solved. The function should be of return type void and
 * take four arguments: a pointer to a spiral_t struct, an integer specifying
 * the index of the latest solved line, an integer specifying the index of the
 * highest line that will be solved and a void pointer used for accessing the
 * user data.
 * returns a status struct (used for error information)
 *
 * Asserts:
 * - That spiral->lines is not NULL
 */
sxbp_status_t sxbp_plot_spiral(
    sxbp_spiral_t* spiral, sxbp_length_t perfection_threshold, uint64_t max_line,
    void(* progress_callback)(
        sxbp_spiral_t* spiral, uint64_t latest_line, uint64_t target_line,
        void* progress_callback_user_data
    ),
    void* progress_callback_user_data
) {
    // preconditional assertions
    assert(spiral->lines != NULL);
    // set up result status
    sxbp_status_t result;
    // get index of highest line to plot
    uint64_t max_index = (max_line > spiral->size) ? spiral->size : max_line;
    // calculate the length of each line within range solved_count -> max_index
    for(size_t i = spiral->solved_count; i < max_index; i++) {
        result = sxbp_resize_spiral(spiral, i, 1, perfection_threshold);
        // catch and return error if any
        if(result != SXBP_OPERATION_OK) {
            return result;
        }
        // call callback if given
        if(progress_callback != NULL) {
            progress_callback(spiral, i, max_index, progress_callback_user_data);
        }
    }
    // all ok
    result = SXBP_OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
