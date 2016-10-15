#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include <stdbool.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a pointer to a spiral struct, the index of one of it's lines and a
 * target length to set that line to and a perfection threshold (-1 for no
 * perfection, or otherwise the maximmum line length at which to allow
 * aggressive optimisation) attempt to set the target line to that length,
 * back-tracking to resize the previous line if it collides.
 * returns a status struct (used for error information)
 */
status_t resize_spiral(
    spiral_t * spiral, size_t index, uint32_t length, int perfection_threshold
);

/*
 * given a pointer to a spiral spiral for which the length of all its lines are
 * not yet known, a perfection threshold (-1 for no perfection, or otherwise
 * the maximmum line length at which to allow aggressive optimisation) and the
 * index of the highest line to plot to, calculate the length needed for each
 * line in the spiral up to this index (to avoid line overlap) and store these
 * in a the spiral struct that is pointed to by the pointer
 * returns a status struct (used for error information)
 */
status_t plot_spiral(
    spiral_t * spiral, int perfection_threshold, uint64_t max_line
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
