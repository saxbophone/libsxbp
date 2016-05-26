#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

const version_t VERSION = {
    .major = 0, .minor = 5, .patch = 11,
};

// vector direction constants
const vector_t VECTOR_DIRECTIONS[4] = {
    // UP       RIGHT       DOWN        LEFT
    { 0, 1, }, { 1, 0, }, { 0, -1, }, { -1, 0, },
};

// when facing the direction specified by current, return the direction that
// will be faced when turning in the rotational direction specified by turn.
direction_t
change_direction(direction_t current, rotation_t turn) {
    return (current + turn) % 4U;
}

// initialises a spiral_t struct from an array pointer to unsigned bytes
// this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
// instructions which are then used to build the pattern.
spiral_t
init_spiral(buffer_t buffer) {
    // number of lines is number of bits of the data
    size_t line_count = buffer.size * 8;
    // create spiral_t struct
    spiral_t result = { .size = line_count, };
    // allocate enough memory for a line_t struct for each bit
    result.lines = calloc(sizeof(line_t), line_count);
    // first direction is a one-off, pre-prepare the stored direction for this
    // if first bit is 0, then first direction is UP, else if 1 then it's RIGHT
    direction_t current = ((buffer.bytes[0] & 0b10000000) == 0) ? LEFT : DOWN;
    // now, iterate over all the bits in the data and convert to directions
    // that make the spiral pattern, storing these directions in the result lines
    for(size_t s = 0; s < buffer.size; s++) {
        // byte-level loop
        for(size_t b = 0; b < 8; b++) {
            // bit level loop
            uint8_t e = 7 - b; // which power of two to use with bit mask
            uint8_t bit = (buffer.bytes[s] & (1 << e)) >> e; // the currently accessed bit
            size_t index = (s * 8) + b; // line index
            rotation_t rotation; // the rotation we're going to make
            // set rotation direction based on the current bit
            rotation = (bit == 0) ? CLOCKWISE : ANTI_CLOCKWISE;
            // calculate the change of direction
            current = change_direction(current, rotation);
            // store direction in result struct
            result.lines[index].direction = current;
            // set length to 0 initially
            result.lines[index].length = 0;
        }
    }
    return result;
}

// return the sum of all line lengths within the given indexes
static size_t
sum_lines(spiral_t spiral, size_t start, size_t end) {
    size_t size = 1;
    for(size_t i = start; i < end; i++) {
        size += spiral.lines[i].length;
    }
    return size;
}

// given a spiral_t struct, a pair of co-ords specifying the start point and
// indexes of the lowest and highest line segments to use, return a
// co_ord_array_t struct containing all the co-ordinates of the line segments of
// the struct according to the current directions and lengths of the lines in
// the spiral.
// each line segment is only one unit long, meaning multiple ones are needed for
// lines longer than one unit.
co_ord_array_t
spiral_points(spiral_t spiral, co_ord_t start_point, size_t start, size_t end) {
    // the amount of space needed is the sum of all line lengths:
    size_t size = sum_lines(spiral, start, end);
    // allocate memory
    co_ord_array_t results = {
        .items = calloc(sizeof(co_ord_t), size),
        .size = size,
    };
    // start current co-ordinate at the given start point
    co_ord_t current = start_point;
    // initialise independent result index
    size_t result_index = 0;
    results.items[result_index] = current;
    // calculate all the specified co-ords
    for(size_t i = start; i < end; i++) {
        // get current direction
        vector_t direction = VECTOR_DIRECTIONS[spiral.lines[i].direction];
        // make as many jumps in this direction as this lines length
        for(uint32_t j = 0; j < spiral.lines[i].length; j++) {
            current.x += direction.x;
            current.y += direction.y;
            results.items[result_index + 1] = current;
            result_index++;
        }
    }
    return results;
}

// given a pointer to a spiral struct an limit, which is the index of the last
// line to use, calculate and store the co-ordinates of all line segments that
// would make up the spiral if the current lengths and directions were used.
// each line segment is only one unit long, meaning multiple ones are needed for
// lines longer than one unit. The co-ords are stored in the spiral's co_ord_cache
// member and are re-used if they are still valid
void
cache_spiral_points(spiral_t * spiral, size_t limit) {
    // the amount of space needed is the sum of all line lengths:
    size_t size = sum_lines(*spiral, 0, limit);
    // allocate / reallocate memory
    if(spiral->co_ord_cache.co_ords.items == NULL) {
        // if no memory has been allocated for the co-ords yet, then do this now
        // allocate enough memory to store these
        spiral->co_ord_cache.co_ords.items = calloc(sizeof(co_ord_t), size);
    } else if(spiral->co_ord_cache.co_ords.size != size) {
        // if there isn't enough memory allocated, re-allocate memory instead
        spiral->co_ord_cache.co_ords.items = realloc(
            spiral->co_ord_cache.co_ords.items, sizeof(co_ord_t) * size
        );
    }
    spiral->co_ord_cache.co_ords.size = size;
    // start at (0, 0) as origin
    co_ord_t current = { 0, 0, };
    size_t result_index = 0; // maintain independent index for co-ords array
    // if we're not going to re-calculate the whole array, skip forward the index
    // find the smallest of limit and the spirals' cache validity
    size_t smallest = (
        limit < spiral->co_ord_cache.validity
    ) ? limit : spiral->co_ord_cache.validity;
    if(spiral->co_ord_cache.validity != 0) {
        // get index of the latest known co-ord
        result_index += (sum_lines(*spiral, 0, smallest) - 1);
        // update current to be at latest known co-ord
        current = spiral->co_ord_cache.co_ords.items[result_index];
    } else {
        // otherwise, start at 0
        spiral->co_ord_cache.co_ords.items[0] = current;
    }
    // calculate the missing co-ords
    co_ord_array_t missing = spiral_points(*spiral, current, smallest, limit);
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
}

#ifdef __cplusplus
} // extern "C"
#endif
