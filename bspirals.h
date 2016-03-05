#ifndef SAXBOPHONE_BSPIRALS_H
#define SAXBOPHONE_BSPIRALS_H

#include "stdint.h"


#ifdef __cplusplus
extern "C"{
#endif

// type for representing a cartesian direction
typedef int8_t direction_t;

// cartesian direction constants
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// type for representing a rotational direction
typedef int8_t rotation_t;

// rotational direction constants
#define CLOCKWISE 1
#define ANTI_CLOCKWISE -1

typedef struct spiral_t {
    // currently unused, TODO: Use!
} spiral_t;

// when facing the direction specified by current, return the direction that
// will be faced when turning in the rotational direction specified by turn.
direction_t
change_direction(direction_t current, rotation_t turn);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
