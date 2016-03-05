#ifndef SAXBOPHONE_BSPIRALS_H
#define SAXBOPHONE_BSPIRALS_H

// includes go here


#ifdef __cplusplus
extern "C"{
#endif

// enum for representing the four cartesian directions
typedef enum direction_t {
    UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3,
} direction_t;

// enum for representing a clockwise or an anti-clockwise turn
typedef enum rotation_t {
    CLOCKWISE = 1, ANTI_CLOCKWISE = -1,
} rotation_t;

// when facing the direction specified by current, return the direction that
// will be faced when turning in the rotational direction specified by turn.
direction_t
change_direction(direction_t current, rotation_t turn);

typedef struct spiral_t {
    //
} spiral_t;

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
