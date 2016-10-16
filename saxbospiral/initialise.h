#ifndef SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_INITIALISE_H

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * when facing the direction specified by current, return the direction that
 * will be faced when turning in the rotational direction specified by turn.
 */
direction_t change_direction(direction_t current, rotation_t turn);

/*
 * returns a spiral struct with all fields initialised to 0
 */
spiral_t blank_spiral();

/*
 * given a buffer_t full of data, and a pointer to a blank spiral_t
 * struct, populates the spiral struct from the data in the buffer
 * this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
 * instructions which are then used to build the pattern.
 * returns a status_t struct with error information (if needed)
 */
status_t init_spiral(buffer_t buffer, spiral_t* spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
