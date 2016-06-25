#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "plot.h"
#include "solve.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * private function, given a spiral struct, check if the latest line would
 * collide with any of the others, given their current directions and jump sizes
 * (using co-ords stored in cache).
 * NOTE: This assumes that all lines except the most recent are valid and
 * don't collide.
 * Returns the index of the line that the latest line collided with if there are
 * collisions, or -1 if no collisions were found.
 */
static int64_t
spiral_collides(spiral_t spiral) {
    /*
     * if there are less than 4 lines in the spiral, then there's no way it
     * can collide, so return false early
     */
    if (spiral.size < 4) {
        return false;
    } else {
        size_t last_co_ord = spiral.co_ord_cache.co_ords.size;
        line_t last_line = spiral.lines[spiral.size - 1];
        size_t start_of_last_line = (last_co_ord - last_line.length) - 1;
        // check the co-ords of the last line segment against all the others
        for(size_t i = 0; i < start_of_last_line; i++) {
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
                    return (int64_t)i;
                }
            }
        }
        return -1;
    }
}

// returns true if a and b are parallel directions (on the same axis)
static bool
parallel(direction_t a, direction_t b) {
    // NOTE: This works because direction constants are ordered radially
    return ((a % 2) == (b % 2)) ? true : false;
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
 */
static length_t
suggest_resize(spiral_t spiral, size_t index) {
    // check if collides or not, return same size if no collision
    if(spiral.collides != -1) {
        // store the 'previous' and 'rigid' lines.
        line_t previous = spiral.lines[index - 1];
        line_t rigid = spiral.lines[spiral.collides];
        /*
         * We need to grab the start and end co-ords of the line previous to the
         * colliding line, and the rigid line that it collided with.
         */
        // TODO: Loop over co-ord cache and grab the start and end co-ords
        /*
         * TODO: Apply the rules mentioned in collision_resolution_rules.txt to
         * calculate the correct length to set the previous line to. Return this.
         */
        return spiral.lines[index - 1].length + 1;
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
        spiral.collides = spiral_collides(spiral);
        if(spiral.collides != -1) {
            /*
             * if we've caused a collision, we need to call the suggest_resize()
             * function to get the suggested length to resize the previous
             * segment to
             */
            current_length = suggest_resize(spiral, current_index);
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
