#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "solve.h"


#ifdef __cplusplus
extern "C"{
#endif

// private function, given a spiral struct, check if the latest line would
// collide with any of the others, given their current directions and jump sizes
// (using co-ords stored in cache).
// NOTE: This assumes that all co-ords except the most recent are valid and
// don't collide
static bool
spiral_collides(spiral_t spiral) {
    // if there are less than 4 lines in the spiral, then there's no way it
    // can collide, so return false early
    if (spiral.size < 4) {
        return false;
    } else {
        // check the last co-ord in the array against all the others
        size_t last = spiral.co_ord_cache.co_ords.size - 1;
        for(size_t i = 0; i < last; i++) {
            if(
                (
                    spiral.co_ord_cache.co_ords.items[i].x ==
                    spiral.co_ord_cache.co_ords.items[last].x
                )
                &&
                (
                    spiral.co_ord_cache.co_ords.items[i].y ==
                    spiral.co_ord_cache.co_ords.items[last].y
                )
            ) {
                return true;
            }
        }
        return false;
    }
}

// private function, given a spiral struct, the index of one of it's lines and
// a target length to set that line to, attempt to set the target line to that
// length, back-tacking to resize the previous line if it collides.
static spiral_t
resize_spiral(spiral_t spiral, size_t index, uint32_t length) {
    // setup state variables, these are used in place of recursion for managing
    // state of which line is being resized, and what size it should be.
    size_t current_index = index;
    size_t current_length = length;
    while(true) {
        // set the target line to the target length
        spiral.lines[current_index].length = current_length;
        // also, set cache validity to this index so we invalidate any invalid
        // entries in the co-ord cache
        spiral.co_ord_cache.validity = (
            current_index < spiral.co_ord_cache.validity
        ) ? current_index : spiral.co_ord_cache.validity;
        // update the spiral's co-ord cache
        cache_spiral_points(&spiral, current_index + 1);
        if(spiral_collides(spiral)) {
            // if we've caused a collision, we should back-track
            // and increase the size of the previous line segment
            current_index--;
            current_length = spiral.lines[current_index].length + 1;
        } else if(current_index != index) {
            // if we didn't cause a collision but we're not on the top-most
            // line, then we've just resolved a collision situation.
            // we now need to work on the next line and start by setting to 1.
            current_index++;
            current_length = 1;
        } else {
            // if we're on the top-most line and there's no collision
            // this means we've finished! Return result.
            return spiral;
        }
    }
}

// given a spiral for which the length of all its lines are not yet known,
// calculate the length needed for each line in the spiral (to avoid line overlap)
// and store these in a spiral struct and return that
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
