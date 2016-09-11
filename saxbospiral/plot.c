#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "plot.h"


#ifdef __cplusplus
extern "C"{
#endif

// returns the sum of all line lengths within the given indexes
size_t
sum_lines(spiral_t spiral, size_t start, size_t end) {
    size_t size = 0;
    for(size_t i = start; i < end; i++) {
        size += spiral.lines[i].length;
    }
    return size;
}

/*
 * given a spiral_t struct, a pointer to a co_ord_array_t, a pair of co-ords
 * specifying the start point and indexes of the lowest and highest line
 * segments to use, write to the co_ord_array_t struct all the co-ordinates of
 * the line segments of the struct according to the current directions and
 * lengths of the lines in the spiral.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit.
 * returns a status struct with error information (if any)
 */
status_t
spiral_points(
    spiral_t spiral, co_ord_array_t * output, co_ord_t start_point,
    size_t start, size_t end
) {
    // prepare result status
    status_t result = {{0, 0, 0}, 0};
    // the amount of space needed is the sum of all line lengths + 1 for end
    size_t size = sum_lines(spiral, start, end) + 1;
    // allocate memory
    output->items = calloc(sizeof(co_ord_t), size);
    // catch malloc error
    if(output->items == NULL) {
        // set error information then early return
        result.location = DEBUG;
        result.diagnostic = MALLOC_REFUSED;
        return result;
    }
    output->size = size;
    // start current co-ordinate at the given start point
    co_ord_t current = start_point;
    // initialise independent result index
    size_t result_index = 0;
    output->items[result_index] = current;
    // calculate all the specified co-ords
    for(size_t i = start; i < end; i++) {
        // get current direction
        vector_t direction = VECTOR_DIRECTIONS[spiral.lines[i].direction];
        // make as many jumps in this direction as this lines length
        for(length_t j = 0; j < spiral.lines[i].length; j++) {
            current.x += direction.x;
            current.y += direction.y;
            output->items[result_index + 1] = current;
            result_index++;
        }
    }
    // all good
    result.diagnostic = OPERATION_OK;
    return result;
}

/*
 * given a pointer to a spiral struct an limit, which is the index of the last
 * line to use, calculate and store the co-ordinates of all line segments that
 * would make up the spiral if the current lengths and directions were used.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit. The co-ords are stored in the spiral's co_ord_cache
 * member and are re-used if they are still valid
 * returns a status struct with error information (if any)
 */
status_t
cache_spiral_points(spiral_t * spiral, size_t limit) {
    // prepare result status
    status_t result = {{0, 0, 0}, 0};
    // the amount of space needed is the sum of all line lengths + 1 for end
    size_t size = sum_lines(*spiral, 0, limit) + 1;
    // allocate / reallocate memory
    if(spiral->co_ord_cache.co_ords.items == NULL) {
        /*
         * if no memory has been allocated for the co-ords yet, then do this now
         * allocate enough memory to store these
         */
        spiral->co_ord_cache.co_ords.items = calloc(sizeof(co_ord_t), size);
    } else if(spiral->co_ord_cache.co_ords.size != size) {
        // if there isn't enough memory allocated, re-allocate memory instead
        spiral->co_ord_cache.co_ords.items = realloc(
            spiral->co_ord_cache.co_ords.items, sizeof(co_ord_t) * size
        );
    }
    // catch malloc failure
    if(spiral->co_ord_cache.co_ords.items == NULL) {
        // set error information then early return
        result.location = DEBUG;
        result.diagnostic = MALLOC_REFUSED;
        return result;
    }
    spiral->co_ord_cache.co_ords.size = size;
    // start at (0, 0) as origin
    co_ord_t current = { 0, 0, };
    size_t result_index = 0; // maintain independent index for co-ords array
    /*
     * if we're not going to re-calculate the whole array, skip forward the index
     * find the smallest of limit and the spirals' cache validity
     */
    size_t smallest = (
        limit < spiral->co_ord_cache.validity
    ) ? limit : spiral->co_ord_cache.validity;
    if(spiral->co_ord_cache.validity != 0) {
        // get index of the latest known co-ord
        result_index += sum_lines(*spiral, 0, smallest);
        // update current to be at latest known co-ord
        current = spiral->co_ord_cache.co_ords.items[result_index];
    } else {
        // otherwise, start at 0
        spiral->co_ord_cache.co_ords.items[0] = current;
    }
    // calculate the missing co-ords
    co_ord_array_t missing= {0, 0};
    status_t calculate_result = spiral_points(
        *spiral, &missing, current, smallest, limit
    );
    // return errors from previous call if needed
    if(calculate_result.diagnostic != OPERATION_OK) {
        return calculate_result;
    }
    // add the missing co-ords to the cache
    for(size_t i = result_index; i < size; i++) {
        spiral->co_ord_cache.co_ords.items[i] = missing.items[i-result_index];
    }
    // free dynamically allocated memory, if any was allocated
    if(missing.items != NULL) {
        free(missing.items);
    }
    // set validity to the largest of limit and current validity
    spiral->co_ord_cache.validity = (
        limit > spiral->co_ord_cache.validity
    ) ? limit : spiral->co_ord_cache.validity;
    // return ok
    result.diagnostic = OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
