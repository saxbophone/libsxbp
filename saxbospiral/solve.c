// sanity check for support of 64-bit integers
#if __SIZEOF_SIZE_T__ < 8
#warning "Please compile this code for a target with 64-bit words or greater."
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "plot.h"
#include "solve.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * private function, given a spiral struct and the index of the highest line
 * to use, check if the latest line would collide with any of the others, given
 * their current directions and jump sizes (using co-ords stored in cache).
 * NOTE: This assumes that all lines except the most recent are valid and
 * don't collide.
 * Returns the index of the lowest line that the latest line collided with if
 * there are collisions, or -1 if no collisions were found.
 */
static int64_t
spiral_collides(spiral_t spiral, size_t index) {
    /*
     * if there are less than 4 lines in the spiral, then there's no way it
     * can collide, so return -1 early
     */
    if (spiral.size < 4) {
        return -1;
    } else {
        // initialise a counter to keep track of what line we're on
        int64_t line_count = 0;
        int64_t ttl = spiral.lines[line_count].length + 1; // ttl of line
        size_t last_co_ord = spiral.co_ord_cache.co_ords.size;
        line_t last_line = spiral.lines[index];
        size_t start_of_last_line = (last_co_ord - last_line.length) - 1;
        // check the co-ords of the last line segment against all the others
        for(int64_t i = 0; i < start_of_last_line; i++) {
            for(size_t j = start_of_last_line; j < last_co_ord; j++) {
                if(
                    (
                        spiral.co_ord_cache.co_ords.items[i].x ==
                        spiral.co_ord_cache.co_ords.items[j].x
                    )
                    &&
                    (
                        spiral.co_ord_cache.co_ords.items[i].y ==
                        spiral.co_ord_cache.co_ords.items[j].y
                    )
                ) {
                    return line_count;
                }
            }
            // update ttl (and counter if needed)
            ttl--;
            if(ttl == 0) {
                line_count++;
                ttl = spiral.lines[line_count].length;
            }
            /*
             * terminate the loop if the next line would be the line 2 lines
             * before the last one (these two lines can never collide with the
             * last and can be safely ignored, for a small performance increase)
             */
            if(line_count == (spiral.size - 2 - 1)) { // -1 for zero-index
                break;
            }
        }
        return -1;
    }
}

/*
 * given a spiral struct that is known to collide, the index of the 'last'
 * segment in the spiral (i.e. the one that was found to be colliding) and a
 * perfection threshold (-1 for no perfection, or otherwise the maximmum line
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
 */
static length_t
suggest_resize(spiral_t spiral, size_t index, int perfection_threshold) {
    // check if collides or not, return same size if no collision
    if(spiral.collides != -1) {
        /*
         * if the perfection threshold is -1, then we can just use our suggestion,
         * as perfection is disabled.
         * otherwise, if the colliding line's length is greater than our
         * perfection threshold, we cannot make any intelligent suggestions on
         * the length to extend the previous line to (without the high
         * likelihood of creating a line that wastes space), so we just return
         * the previous line's length +1
         */
        if(
            (perfection_threshold != -1) &&
            (spiral.lines[index].length > (length_t)perfection_threshold)
        ) {
            return spiral.lines[index - 1].length + 1;
        }
        // store the 'previous' and 'rigid' lines.
        line_t p = spiral.lines[index - 1];
        line_t r = spiral.lines[spiral.collides];
        // if pr and r are not parallel, we can return early
        if((p.direction % 2) != (r.direction % 2)) {
            return spiral.lines[index - 1].length + 1;
        }
        // create variables to store the start and end co-ords of these lines
        co_ord_t pa, pb, ra, rb;
        /*
         * We need to grab the start and end co-ords of the line previous to the
         * colliding line, and the rigid line that it collided with.
         */
        size_t p_index = sum_lines(spiral, 0, index - 1);
        size_t r_index = sum_lines(spiral, 0, spiral.collides);
        pa = spiral.co_ord_cache.co_ords.items[p_index];
        pb = spiral.co_ord_cache.co_ords.items[p_index + p.length];
        ra = spiral.co_ord_cache.co_ords.items[r_index];
        rb = spiral.co_ord_cache.co_ords.items[r_index + r.length];
        /*
         * Apply the rules mentioned in collision_resolution_rules.txt to
         * calculate the correct length to set the previous line and return it.
         */
        if(false) {
            (void)0; // no-op
        } else if((p.direction == UP) && (r.direction == UP)) {
            return (ra.y - pa.y) + r.length + 1;
        } else if((p.direction == UP) && (r.direction == DOWN)) {
            return (rb.y - pa.y) + r.length + 1;
        } else if((p.direction == RIGHT) && (r.direction == RIGHT)) {
            return (ra.x - pa.x) + r.length + 1;
        } else if((p.direction == RIGHT) && (r.direction == LEFT)) {
            return (rb.x - pa.x) + r.length + 1;
        } else if((p.direction == DOWN) && (r.direction == UP)) {
            return (pa.y - rb.y) + r.length + 1;
        } else if((p.direction == DOWN) && (r.direction == DOWN)) {
            return (pa.y - ra.y) + r.length + 1;
        } else if((p.direction == LEFT) && (r.direction == RIGHT)) {
            return (pa.x - rb.x) + r.length + 1;
        } else if((p.direction == LEFT) && (r.direction == LEFT)) {
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
 * target length to set that line to and a perfection threshold (-1 for no
 * perfection, or otherwise the maximmum line length at which to allow
 * aggressive optimisation) attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 * returns a status struct (used for error information)
 */
status_t
resize_spiral(
    spiral_t * spiral, size_t index, uint32_t length, int perfection_threshold
) {
    /*
     * setup state variables, these are used in place of recursion for managing
     * state of which line is being resized, and what size it should be.
     */
    // set result status
    status_t result = {};
    size_t current_index = index;
    length_t current_length = length;
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
        result = cache_spiral_points(spiral, current_index + 1);
        // return if errors
        if(result.diagnostic != OPERATION_OK) {
            return result;
        }
        spiral->collides = spiral_collides(*spiral, current_index);
        if(spiral->collides != -1) {
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
             * this means we've finished! Return OPERATION_OK from function.
             */
            result.diagnostic = OPERATION_OK;
            return result;
        }
    }
}

/*
 * given a pointer to a spiral spiral for which the length of all its lines are
 * not yet known and a perfection threshold (-1 for no perfection, or otherwise
 * the maximmum line length at which to allow aggressive optimisation) calculate
 * the length needed for each line in the spiral (to avoid line overlap) and
 * store these in a the spiral struct that is pointed to by the pointer
 * returns a status struct (used for error information)
 */
status_t
plot_spiral(spiral_t * spiral, int perfection_threshold) {
    // set up result status
    status_t result = {};
    // calculate the length of each line
    for(size_t i = 0; i < spiral->size; i++) {
        result = resize_spiral(spiral, i, 1, perfection_threshold);
        // catch and return error if any
        if(result.diagnostic != OPERATION_OK) {
            return result;
        }
    }
    // all ok
    result.diagnostic = OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
