#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bspirals.h"


#ifdef __cplusplus
extern "C"{
#endif

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
init_spiral(uint8_t * buffer, size_t size) {
    // number of lines is number of bits of the data
    size_t line_count = size * 8;
    // create spiral_t struct
    spiral_t result = { .size = line_count, };
    // allocate enough memory for a line_t struct for each bit
    result.lines = calloc(sizeof(line_t), line_count);
    // first direction is a one-off, pre-prepare the stored direction for this
    // if first bit is 0, then first direction is UP, else if 1 then it's RIGHT
    direction_t current = ((buffer[0] & 0b10000000) == 0) ? LEFT : DOWN;
    // now, iterate over all the bits in the data and convert to directions
    // that make the spiral pattern, storing these directions in the result lines
    for(size_t s = 0; s < size; s++) {
        // byte-level loop
        for(size_t b = 0; b < 8; b++) {
            // bit level loop
            uint8_t e = 7-b; // which power of two to use with bit mask
            uint8_t bit = (buffer[s] & (1 << e)) >> e; // the currently accessed bit
            size_t index = (s*8) + b; // line index
            rotation_t rotation; // the rotation we're going to make
            // set rotation direction based on the current bit
            if(bit == 0) {
                rotation = CLOCKWISE;
            } else {
                rotation = ANTI_CLOCKWISE;
            }
            // calculate the change of direction
            current = change_direction(current, rotation);
            // store direction in result struct
            result.lines[index].direction = current;
        }
    }
    return result;
}

// private function, returns a dynamically allocated array of co_ord_t structs
// that represent all the points in cartesian space occupied by the given spiral
static co_ord_t * spiral_points(spiral_t spiral, size_t limit) {
    co_ord_t current = { 0, 0, };
    // TODO: rest of function...
}

// private function, given a spiral struct, check if any of its lines would
// collide given their current directions and jump sizes, checking up to limit
// number of lines
static bool spiral_collides(spiral_t spiral, size_t limit) {
    return false;
}

// private function, given a spiral struct, the index of one of it's lines and
// a target length to set that line to, attempt to set the target line to that
// length, recursively calling self to resize the previous line if that is not
// possible.
static spiral_t
resize_spiral(spiral_t input, size_t index, uint32_t length) {
    // TODO: Clean these up!
    // printf(".");
    // fflush(stdout);
    // allocate new struct as copy of input struct
    spiral_t output = { .size = input.size, };
    output.lines = calloc(sizeof(line_t), output.size);
    // copy across line directions and lengths
    for(size_t i = 0; i < output.size; i++) {
        output.lines[i].direction = input.lines[i].direction;
        output.lines[i].length = input.lines[i].length;
    }
    // first, set the target line to the target length
    output.lines[index].length = length;
    // now, check for collisions (NOT IMPLEMENTED YET)
    bool collides = spiral_collides(output, index);
    if(collides) {
        // there were collisions, so reset the target line to 1
        output.lines[index].length = 1;
        while(collides) {
            // recursively call resize_spiral(), increasing the size of the
            // previous line until we get something that doesn't collide
            output = resize_spiral(
                output, index-1, output.lines[index-1].length+1
            );
            collides = spiral_collides(output, index);
        }
    }
    return output;
}

// given a spiral for which the length of all its lines are not yet known,
// calculate the length needed for each line in the spiral (to avoid line overlap)
// and store these in a spiral struct and return that
spiral_t
plot_spiral(spiral_t input) {
    // allocate new struct as copy of input struct
    spiral_t output = { .size = input.size, };
    output.lines = calloc(sizeof(line_t), output.size);
    // copy across line directions
    for(size_t i = 0; i < output.size; i++) {
        output.lines[i].direction = input.lines[i].direction;
    }
    // calculate the length of each line
    for(size_t i = 0; i < output.size; i++) {
        output = resize_spiral(output, i, 1);
    }
    return output;
}

#ifdef __cplusplus
} // extern "C"
#endif
