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
 * NOTE: This assumes that all co-ords except the most recent are valid and
 * don't collide
 */
static bool
spiral_collides(spiral_t spiral) {
    /*
     * if there are less than 4 lines in the spiral, then there's no way it
     * can collide, so return false early
     */
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

// returns true if a and b are parallel directions (on the same axis)
static bool
parallel(direction_t a, direction_t b) {
    // NOTE: This works because direction constants are ordered radially
    return ((a % 2) == (b % 2)) ? true : false;
}

/*
 * given two lines ab and cd, along with their directions,
 * return true if the lines they make up intersect
 */
bool
segments_intersect(
    co_ord_t a, co_ord_t b, direction_t ab_direction,
    co_ord_t c, co_ord_t d, direction_t cd_direction
) {
    // TODO: Refactor this function, as it's a total mess!
    // ***************************************************
    // check if they're the same direction first
    bool are_same = (ab_direction == cd_direction) ? true : false;
    // if they are the same then they're parallel, if not they might be
    bool are_parallel = (are_same) ? true : parallel(ab_direction, cd_direction);
    if(!are_parallel) {
        /*
         * if they're not parallel then they're at right angles, so we need to
         * find out the horizontal and vertical extremities
         */
        co_ord_t
        horizontal_lower, horizontal_higher, vertical_lower, vertical_higher;
        // find out which way round each line is first
        if((ab_direction % 2) == 0) {
            // AB is vertical and CD is horizontal
            // distinguish which way round the line AB is
            if(ab_direction == UP) {
                vertical_lower = b;
                vertical_higher = a;
            } else if(ab_direction == DOWN) {
                vertical_lower = a;
                vertical_higher = b;
            }
            // distinguish which way round the line CD is
            if(cd_direction == RIGHT) {
                horizontal_lower = c;
                horizontal_higher = d;
            } else if(cd_direction == LEFT) {
                horizontal_lower = d;
                horizontal_higher = c;
            }
        } else {
            // CD is vertical and AB is horizontal
            // distinguish which way round the line CD is
            if(cd_direction == UP) {
                vertical_lower = d;
                vertical_higher = c;
            } else if(cd_direction == DOWN) {
                vertical_lower = c;
                vertical_higher = d;
            }
            // distinguish which way round the line AB is
            if(ab_direction == RIGHT) {
                horizontal_lower = a;
                horizontal_higher = b;
            } else if(ab_direction == LEFT) {
                horizontal_lower = b;
                horizontal_higher = a;
            }
        }
        // do some sanity checking
        if(vertical_higher.x != vertical_lower.x) { abort(); }
        if(horizontal_higher.y != horizontal_lower.y) { abort(); }
        // now, do some simple comparisons between our four numbers
        if(
            (
                (vertical_higher.x >= horizontal_lower.x)
                &&
                (vertical_higher.x <= horizontal_higher.x)
            )
            &&
            (
                (horizontal_lower.y >= vertical_lower.y)
                &&
                (horizontal_lower.y <= vertical_higher.y)
            )
        ) {
            return true;
        } else {
            return false;
        }
    } else {
        // compare lines for overlap in the axis opposite to their common direction
        if((ab_direction % 2) == 0) {
            // both lines are vertical, so check horizontal correlation
            if((a.x != c.x) || (b.x != d.x)) {
                return false;
            }
        } else {
            // both lines are horizontal, so check vertical correlation
            if((a.y != c.y) || (b.y != d.y)) {
                return false;
            }
        }
        /*
         * we only need to compare one axis, depending on orientation of the
         * lines, so only store one of x or y for each co-ord
         */
        // get the highest and lowest point of each line
        tuple_item_t ab_lowest, ab_highest, cd_lowest, cd_highest;
        switch(ab_direction) {
            case UP:
                ab_lowest = a.y;
                ab_highest = b.y;
                break;
            case RIGHT:
                ab_lowest = a.x;
                ab_highest = b.x;
                break;
            case DOWN:
                ab_lowest = b.y;
                ab_highest = a.y;
                break;
            case LEFT:
                ab_lowest = b.x;
                ab_highest = a.x;
                break;
            default:
                // should not reach here!
                abort();
        }
        switch(cd_direction) {
            case UP:
                cd_lowest = c.y;
                cd_highest = d.y;
                break;
            case RIGHT:
                cd_lowest = c.x;
                cd_highest = d.x;
                break;
            case DOWN:
                cd_lowest = d.y;
                cd_highest = c.y;
                break;
            case LEFT:
                cd_lowest = d.x;
                cd_highest = c.x;
                break;
            default:
                // should not reach here!
                abort();
        }
        // now work out which line is 'further' than the other
        tuple_item_t lowest_lower, highest_lower, lowest_higher, highest_higher;
        if((ab_highest > cd_highest) || (cd_lowest < ab_lowest)) {
            highest_higher = ab_highest;
            lowest_higher = ab_lowest;
            highest_lower = cd_highest;
            lowest_lower = cd_lowest;
        } else if((cd_highest > ab_highest) || (ab_lowest < cd_lowest)) {
            highest_higher = cd_highest;
            lowest_higher = cd_lowest;
            highest_lower = ab_highest;
            lowest_lower = ab_lowest;
        } else {
            // they definitely collide!
            return true;
        }
        // now finally, check if they collide!
        if(
            (lowest_lower <= lowest_higher) && (lowest_higher <= highest_lower) &&
            (highest_lower <= highest_higher) // this last one probably not needed
        ) {
            return true;
        } else {
            return false;
        }
    }
    return false;
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
    // NOTE: Get the co-ordinates out by jumping each line's length along co-ord cache
    /*
     * TODO: For every segment up to and excluding the one at index, check if
     * it intersects with the segment at index, using segments_intersect()
     * break from loop when the two that intersect have been found, and store
     * the index. NOTE: Ignore the 2 segments before the one at index, as these
     * cannot intersect with it (and it would break the algorithm if we tested
     * them, as they will appear to intersect with it).
     */
    // grab co-ords of last line (one that is the collider)
    co_ord_t a = spiral.co_ord_cache.co_ords.items[
        spiral.co_ord_cache.co_ords.size - 1 - spiral.lines[index].length
    ];
    co_ord_t b = spiral.co_ord_cache.co_ords.items[
        spiral.co_ord_cache.co_ords.size - 1
    ];
    // grab co-ords of the start of the previous line
    co_ord_t e = spiral.co_ord_cache.co_ords.items[
        spiral.co_ord_cache.co_ords.size - 1 - spiral.lines[index].length -
        spiral.lines[index - 1].length
    ];
    // reserved names for storing the currently tested line
    co_ord_t c, d;
    size_t cache_index = spiral.co_ord_cache.co_ords.size - 1;
    size_t i;
    for(i = (index); i > -1; i--) {
        // skip the last two as these definitely don't collide
        if(i > (index - 2)) {
            cache_index -= spiral.lines[i].length;
            continue;
        } else {
            // grab co-ords of currently tested line
            c = spiral.co_ord_cache.co_ords.items[
                cache_index - spiral.lines[i].length
            ];
            d = spiral.co_ord_cache.co_ords.items[
                cache_index
            ];
            if(
                segments_intersect(
                    a, b, spiral.lines[index].direction,
                    c, d, spiral.lines[i].direction
                )
            ) {
                // if they collide, then quit the loop
                break;
            }
            cache_index -= spiral.lines[i].length;
        }
    }
    // now, size_t i stores the index of the line that collided
    if(i > -1) {
        /*
         * TODO: Apply the rules mentioned in collision_resolution_rules.txt to
         * calculate the correct length to set the previous line to. Return this.
         */
        if(spiral.lines[index - 1].direction != spiral.lines[i].direction) {
            // if directions are not equal they might still be parallel
            if(
                parallel(
                    spiral.lines[index - 1].direction, spiral.lines[i].direction
                )
            ) {
                // for each of these clauses, the other line will be opposite direction
                switch(spiral.lines[i].direction) {
                    case UP:
                        return (e.y - d.y) + 1;
                    case DOWN:
                        return (d.y - e.y) + 1;
                    case LEFT:
                        return (e.x - d.x) + 1;
                    case RIGHT:
                        return (d.x - e.x) + 1;
                }
            } else {
                return spiral.lines[index - 1].length + 1;
            }
        } else {
            printf("DIRECTIONS EQUAL\n");
            switch(spiral.lines[i].direction) {
                case UP:
                    return (e.y - d.y) + 1;
                case DOWN:
                    return (d.y - e.y) + 1;
                case LEFT:
                    return (e.x - d.x) + 1;
                case RIGHT:
                    return (d.x - e.x) + 1;
            }
        }
    } else {
        /*
         * If we got here, then i is -1 which means that no collisions could be
         * found, which means we don't have to extend the previous segment.
         */
        // TODO: Change this block to return the same size
        return spiral.lines[index - 1].length + 1; // couldn't work it out :/
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
        if(spiral_collides(spiral)) {
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
