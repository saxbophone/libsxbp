#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

const version_t VERSION = {
    .major = 0, .minor = 2, .patch = 0,
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
            uint8_t e = 7-b; // which power of two to use with bit mask
            uint8_t bit = (buffer.bytes[s] & (1 << e)) >> e; // the currently accessed bit
            size_t index = (s*8) + b; // line index
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

// given a pointer to a spiral struct an limit, which is the index of the last
// line to use, calculate and store the co-ordinates of all line segments that
// would make up the spiral if the current lengths and directions were used.
// each line segment is only one unit long, meaning multiple ones are needed for
// lines longer than one unit. The co-ords are stored in the spiral's co_ord_cache
// member and are re-used if they are still valid
void
spiral_points(spiral_t * spiral, size_t limit) {
    // the amount of space needed is the sum of all line lengths:
    size_t size = 1;
    for(size_t i = 0; i < limit; i++) {
        size += spiral->lines[i].length;
    }
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
    size_t smallest = (
        limit < spiral->co_ord_cache.validity
    ) ? limit : spiral->co_ord_cache.validity;
    if(spiral->co_ord_cache.validity != 0) {
        for(size_t i = 0; i < smallest; i++) {
            result_index += spiral->lines[i].length;
        }
        // update current to be at latest known co-ord
        current.x = spiral->co_ord_cache.co_ords.items[result_index].x;
        current.y = spiral->co_ord_cache.co_ords.items[result_index].y;
    } else {
        // otherwise, start at 0
        spiral->co_ord_cache.co_ords.items[0].x = current.x;
        spiral->co_ord_cache.co_ords.items[0].y = current.y;
    }
    // iterate over all lines and add the co-ords
    // start at the highest known good co-ord in cache
    for(size_t i = spiral->co_ord_cache.validity; i < limit; i++) {
        // get current direction
        vector_t direction = VECTOR_DIRECTIONS[spiral->lines[i].direction];
        // make as many jumps in this direction as this lines legth
        for(uint32_t j = 0; j < spiral->lines[i].length; j++) {
            current.x += direction.x;
            current.y += direction.y;
            spiral->co_ord_cache.co_ords.items[result_index+1].x = current.x;
            spiral->co_ord_cache.co_ords.items[result_index+1].y = current.y;
            result_index++;
        }
        // update validity
    }
    spiral->co_ord_cache.validity = limit;
}

// private function, given a spiral struct, check if any of its lines would
// collide given their current directions and jump sizes, checking up to limit
// number of lines
static bool
spiral_collides(spiral_t * spiral, size_t limit) {
    // update the spiral's co-ord cache
    spiral_points(spiral, limit);
    // check for duplicates
    // false if there are not.
    bool duplicates = false;
    for(size_t i = 0; i < spiral->co_ord_cache.co_ords.size; i++) {
        for(size_t j = 0; j < spiral->co_ord_cache.co_ords.size; j++) {
            if(i != j) {
                if(
                    (spiral->co_ord_cache.co_ords.items[i].x == spiral->co_ord_cache.co_ords.items[j].x)
                    && (spiral->co_ord_cache.co_ords.items[i].y == spiral->co_ord_cache.co_ords.items[j].y)
                ) {
                    duplicates = true;
                    break;
                }
            }
        }
        if(duplicates) {
            break;
        }
    }
    return duplicates;
}

// private function, given a spiral struct, the index of one of it's lines and
// a target length to set that line to, attempt to set the target line to that
// length, recursively calling self to resize the previous line if that is not
// possible.
static spiral_t
resize_spiral(spiral_t spiral, size_t index, uint32_t length) {
    // first, set the target line to the target length
    spiral.lines[index].length = length;
    // also, set cache validity to this index so we invalidate any invalid
    // entries in the co-ord cache
    spiral.co_ord_cache.validity = (
        index < spiral.co_ord_cache.validity
    ) ? index : spiral.co_ord_cache.validity;
    // now, check for collisions
    bool collides = spiral_collides(&spiral, index+1);
    if(collides) {
        // there were collisions, so reset the target line to 1
        spiral.lines[index].length = 1;
        while(collides) {
            // recursively call resize_spiral(), increasing the size of the
            // previous line until we get something that doesn't collide
            spiral = resize_spiral(
                spiral, index-1, spiral.lines[index-1].length+1
            );
            // check if it still collides
            collides = spiral_collides(&spiral, index+1);
        }
    }
    return spiral;
}

// given a spiral for which the length of all its lines are not yet known,
// calculate the length needed for each line in the spiral (to avoid line overlap)
// and store these in a spiral struct and return that
spiral_t
plot_spiral(spiral_t input) {
    // allocate new struct as copy of input struct
    spiral_t output = { .size = input.size, };
    output.lines = calloc(sizeof(line_t), output.size);
    // copy across line directions and lengths
    for(size_t i = 0; i < output.size; i++) {
        output.lines[i].direction = input.lines[i].direction;
        output.lines[i].length = input.lines[i].length;
    }
    // calculate the length of each line
    for(size_t i = 0; i < output.size; i++) {
        output = resize_spiral(output, i, 1);
    }
    // free the co_ord_cache member's dynamic memory
    if(output.co_ord_cache.co_ords.size > 0) {
        free(output.co_ord_cache.co_ords.items);
        output.co_ord_cache.co_ords.size = 0;
    }
    return output;
}

// given a buffer, return a spiral represented by the data in the struct
// returns a spiral of length 0 if the data could not be interpreted correctly
spiral_t
load_spiral(buffer_t buffer) {
    // create initial output spiral, a spiral of length 0 (shows failure)
    spiral_t output = { .size = 0, };
    // Check for buffer size (must be at least the size of the header)
    // Check for magic number
    if((strncmp(buffer.bytes, "SAXBOSPIRAL", 11) == 0) && (buffer.size >= 29)) {
        // good to go
        // TODO: Add checks for buffer data version compatibility
        /*
        version_t data_version = {
            buffer.bytes[12], buffer.bytes[13], buffer.bytes[13],
        };
        */
        // get size of spiral object contained in buffer
        size_t spiral_size = 0;
        for(size_t i = 0; i < 8; i++) {
            spiral_size |= (buffer.bytes[16+i]) << (8*(7-i));
        }
        // Check that the file data section is large enough for the spiral size
        if((buffer.size-25) != (sizeof(line_t)*spiral_size)) {
            // this check failed, so return it as it is
            return output;
        }
        // init spiral struct
        output.size = spiral_size;
        // allocate memory
        output.lines = calloc(sizeof(line_t), output.size);
        // convert each serialised line segment in buffer into a line_t struct
        for(size_t i = 0; i < spiral_size; i++) {
            // direction is stored in 2 most significant bits of each 32-bit sequence
            output.lines[i].direction = (buffer.bytes[25+(i*4)] >> 6);
            // length is stored as 30 least significant bits, so we have to unpack it
            // handle first byte on it's own as we only need least 6 bits of it
            // bit mask and shift 3 bytes to left
            output.lines[i].length = (buffer.bytes[25+(i*4)] & 0b00111111) << 24;
            // handle remaining 3 bytes in loop
            for(uint8_t j = 0; j < 3; j++) {
                output.lines[i].length |= (buffer.bytes[25+(i*4)+1+j]) << (8*(2-j));
            }
        }
    }
    // return the output struct, this will be a struct of size 0 if the magic
    // number check failed, else it will be a valid spiral
    return output;
}

// given a spiral, return a buffer of the raw bytes used to represent and store it
buffer_t
dump_spiral(spiral_t spiral) {
    // build output buffer struct, base size on header + spiral size
    buffer_t output = { .size = (25 + (sizeof(line_t)*spiral.size)), };
    // allocate memory
    output.bytes = calloc(1, output.size);
    // write first part of data header (magic number and version info)
    sprintf(
        output.bytes, "SAXBOSPIRAL\n%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch
    );
    // write second part of data header (spiral size as 64 bit uint)
    for(uint8_t i = 0; i < 8; i++) {
        uint8_t shift = (8*(7-i));
        output.bytes[16+i] = (uint8_t)(((uint64_t)spiral.size & (0xff << shift)) >> shift);
    }
    // write final newline at end of header
    output.bytes[24] = '\n';
    // now write the data section
    for(size_t i = 0; i < spiral.size; i++) {
        // serialise each line in the spiral to 4 bytes, handle first byte first
        // map direction to 2 most significant bits
        output.bytes[25+(i*4)] = (spiral.lines[i].direction << 6);
        // handle first 6 bits of the length
        output.bytes[25+(i*4)] |= (spiral.lines[i].length >> 24);
        // handle remaining 3 bytes in a loop
        for(uint8_t j = 0; j < 3; j++) {
            output.bytes[25+(i*4)+1+j] = (uint8_t)(spiral.lines[i].length >> (8*(2-j)));
        }
    }
    return output;
}

#ifdef __cplusplus
} // extern "C"
#endif
