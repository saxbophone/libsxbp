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
sxbp_direction_t sxbp_change_direction(
    sxbp_direction_t current, sxbp_rotation_t turn
);

/*
 * returns a spiral struct with all fields initialised to 0
 */
sxbp_spiral_t sxbp_blank_spiral();

/*
 * given a buffer_t full of data, and a pointer to a blank spiral_t
 * struct, populates the spiral struct from the data in the buffer
 * this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
 * instructions which are then used to build the pattern.
 * returns a status_t struct with error information (if needed)
 */
sxbp_status_t sxbp_init_spiral(sxbp_buffer_t buffer, sxbp_spiral_t* spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
