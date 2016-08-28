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
        // TODO: Check that the ttl needs to be length+1 and not just length
        int64_t ttl = spiral.lines[line_count].length + 1; // ttl of line
        /*
         * TODO: Make the loop ignore the two lines before the latest.
         * These can NEVER collide with it and this will improve performance.
         */
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
        }
        return -1;
    }
}

/*
 * given a spiral struct that is known to collide and the index of the 'last'
 * segment in the spiral (i.e. the one that was found to be colliding), return
 * a suggested length to set the segment before this line to.
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
suggest_resize(spiral_t spiral, size_t index) {
    // check if collides or not, return same size if no collision
    if(spiral.collides != -1) {
        // store the 'previous' and 'rigid' lines.
        line_t p = spiral.lines[index - 1];
        line_t r = spiral.lines[spiral.collides];
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
 * given a spiral struct, the index of one of it's lines and a target length to
 * set that line to, attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 */
spiral_t
resize_spiral(spiral_t spiral, size_t index, length_t length) {
    /*
     * setup state variables, these are used in place of recursion for managing
     * state of which line is being resized, and what size it should be.
     */
    size_t current_index = index;
    length_t current_length = length;
    while(true) {
        // set the target line to the target length
        spiral.lines[current_index].length = current_length;
        /*
         * also, set cache validity to this index so we invalidate any invalid
         * entries in the co-ord cache
         */
        spiral.co_ord_cache.validity = (
            current_index < spiral.co_ord_cache.validity
        ) ? current_index : spiral.co_ord_cache.validity;
        // update the spiral's co-ord cache
        cache_spiral_points(&spiral, current_index + 1);
        spiral.collides = spiral_collides(spiral, current_index);
        if(spiral.collides != -1) {
            /*
             * if we've caused a collision, we need to call the suggest_resize()
             * function to get the suggested length to resize the previous
             * segment to
             */
            if(current_length == 1) {
                current_length = suggest_resize(spiral, current_index);
            } else {
                current_length = spiral.lines[current_index - 1].length + 1;
            }
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
             * this means we've finished! Return result.
             */
            return spiral;
        }
    }
}

/*
 * given a spiral for which the length of all its lines are not yet known,
 * calculate the length needed for each line in the spiral (to avoid line overlap)
 * and store these in a spiral struct and return that
 */
spiral_t
plot_spiral(spiral_t input) {
    // allocate new struct as copy of input struct
    spiral_t output = { .size = input.size, };
    output.lines = calloc(sizeof(line_t), output.size);
    // copy across the spiral lines
    for(size_t i = 0; i < output.size; i++) {
        output.lines[i] = input.lines[i];
    }
    // calculate the length of each line
    for(size_t i = 0; i < output.size; i++) {
        output = resize_spiral(output, i, 1);
    }
    // free the co_ord_cache member's dynamic memory if required
    if(output.co_ord_cache.co_ords.size > 0) {
        free(output.co_ord_cache.co_ords.items);
        output.co_ord_cache.co_ords.size = 0;
    }
    return output;
}

#ifdef __cplusplus
} // extern "C"
#endif
