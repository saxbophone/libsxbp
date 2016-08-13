#include <stdint.h>
#include <stdlib.h>

#include "initialise.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * when facing the direction specified by current, return the direction that
 * will be faced when turning in the rotational direction specified by turn.
 */
direction_t
change_direction(direction_t current, rotation_t turn) {
    return (current + turn) % 4U;
}

/*
 * initialises a spiral_t struct from an array pointer to unsigned bytes
 * this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
 * instructions which are then used to build the pattern.
 */
spiral_t
init_spiral(buffer_t buffer) {
    // number of lines is number of bits of the data
    size_t line_count = buffer.size * 8;
    // create spiral_t struct
    spiral_t result = {
        .size = line_count,
        .collides = -1,
        // allocate enough memory for a line_t struct for each bit
        .lines = calloc(sizeof(line_t), line_count),
    };
    /*
     * first direction is a one-off, pre-prepare the stored direction for this
     * if first bit is 0, then first direction is UP, else if 1 then it's RIGHT
     */
    direction_t current = ((buffer.bytes[0] & 0b10000000) == 0) ? LEFT : DOWN;
    /*
     * now, iterate over all the bits in the data and convert to directions
     * that make the spiral pattern, storing these directions in the result lines
     */
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

#ifdef __cplusplus
} // extern "C"
#endif
