#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include <stdbool.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given two lines a and b, given as the starting co-ords a and b, each line's
 * length and it's direction, return true if the lines they make up intersect
 */
bool
segments_intersect(
    co_ord_t a, length_t al, direction_t ad,
    co_ord_t b, length_t bl, direction_t bd
);

/*
 * given a spiral for which the length of all its lines are not yet known,
 * calculate the length needed for each line in the spiral (to avoid line overlap)
 * and store these in a spiral struct and return that
 */
spiral_t
plot_spiral(spiral_t spiral);

/*
 * given a spiral struct, the index of one of it's lines and a target length to
 * set that line to, attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 */
spiral_t
resize_spiral(spiral_t spiral, size_t index, uint32_t length);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
