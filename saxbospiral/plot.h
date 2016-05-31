#ifndef SAXBOPHONE_SAXBOSPIRAL_PLOT_H
#define SAXBOPHONE_SAXBOSPIRAL_PLOT_H

#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a spiral_t struct, a pair of co-ords specifying the start point and
 * indexes of the lowest and highest line segments to use, return a
 * co_ord_array_t struct containing all the co-ordinates of the line segments of
 * the struct according to the current directions and lengths of the lines in
 * the spiral.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit.
 */
co_ord_array_t
spiral_points(spiral_t spiral, co_ord_t start_point, size_t start, size_t end);

/*
 * given a pointer to a spiral struct an limit, which is the index of the last
 * line to use, calculate and store the co-ordinates of all line segments that
 * would make up the spiral if the current lengths and directions were used.
 * each line segment is only one unit long, meaning multiple ones are needed for
 * lines longer than one unit. The co-ords are stored in the spiral's co_ord_cache
 * member and are re-used if they are still valid
 */
void
cache_spiral_points(spiral_t * spiral, size_t limit);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
