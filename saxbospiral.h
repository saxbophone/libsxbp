#ifndef SAXBOPHONE_SAXBOSPIRAL_H
#define SAXBOPHONE_SAXBOSPIRAL_H

#include <stdint.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C"{
#endif

typedef struct version_t {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} version_t;

extern const version_t VERSION;

// type for representing a cartesian direction
typedef uint8_t direction_t;

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

// type for representing the length of a line segment of a spiral
typedef uint32_t length_t;

// struct for representing one line segment in the spiral structure, including
// the direction of the line and it's length (initially set to 0)
// the whole struct uses bitfields to occupy 32 bits of memory
typedef struct line_t {
    direction_t direction : 2; // as there are only 4 directions, use 2 bits
    length_t length : 30; // use 30 bits for the length, nice and long
} line_t;

typedef struct spiral_t {
    size_t size;
    line_t * lines;
} spiral_t;

typedef struct tuple_t {
    int64_t x;
    int64_t y;
} tuple_t;

typedef tuple_t vector_t;
typedef tuple_t co_ord_t;

typedef struct co_ord_array_t {
    co_ord_t * items;
    size_t size;
} co_ord_array_t;

typedef struct buffer_t {
    uint8_t * bytes;
    size_t size;
} buffer_t;

// vector direction constants
extern const vector_t VECTOR_DIRECTIONS[4];

// when facing the direction specified by current, return the direction that
// will be faced when turning in the rotational direction specified by turn.
direction_t
change_direction(direction_t current, rotation_t turn);

// initialises a spiral_t struct from an array pointer to unsigned bytes
// this converts the 0s and 1s in the data into UP, LEFT, DOWN, RIGHT
// instructions which are then used to build the pattern.
spiral_t
init_spiral(buffer_t buffer);

// given a spiral for which the length of all its lines are not yet known,
// calculate the length needed for each line in the spiral (to avoid line overlap)
// and store these in a spiral struct and return that
spiral_t
plot_spiral(spiral_t spiral);

// given a buffer, return a spiral represented by the data in the struct
// returns a spiral of length 0 if the data could not be interpreted correctly
spiral_t
load_spiral(buffer_t buffer);

// given a spiral, return a buffer of the raw bytes used to represent and store it
buffer_t
dump_spiral(spiral_t spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
