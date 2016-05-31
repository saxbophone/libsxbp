#ifndef SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H

#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * when facing the direction specified by current, return the direction that
 * will be faced when turning in the rotational direction specified by turn.
 */
direction_t
change_direction(direction_t current, rotation_t turn);

/*
 * initialises a spiral_t struct from an array pointer to unsigned bytes
 * this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
 * instructions which are then used to build the pattern.
 */
spiral_t
init_spiral(buffer_t buffer);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
