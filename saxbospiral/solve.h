#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include <stdint.h>

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
    spiral_t* spiral, size_t index, uint32_t length, int perfection_threshold
);

/*
 * given a pointer to a spiral spiral for which the length of all its lines are
 * not yet known, a perfection threshold (-1 for no perfection, or otherwise
 * the maximmum line length at which to allow aggressive optimisation), the
 * index of the highest line to plot to and a pointer to a callback function,
 * calculate the length needed for each line in the spiral up to this index
 * (to avoid line overlap) and store these in a the spiral struct that is
 * pointed to by the pointer
 * the function pointer can be NULL, if it is not then it will be called every
 * time a new line of the spiral is solved. The function should be of return
 * type void and take three arguments: a pointer to a spiral_t struct, an
 * integer specifying the index of the latest solved line and an integer
 * specifying the index of the highest line that will be solved.
 * returns a status struct (used for error information)
 */
status_t plot_spiral(
    spiral_t* spiral, int perfection_threshold, uint64_t max_line,
    void(* progress_callback)(
        spiral_t* spiral, uint64_t latest_line, uint64_t target_line
    )
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
