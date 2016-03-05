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
spiral_t
init_spiral(uint8_t * buffer, size_t size) {
    size_t line_count = size * 8;
    spiral_t result = { .size = line_count, };
    result.lines = calloc(sizeof(line_t), line_count);
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
