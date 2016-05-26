#ifndef SAXBOPHONE_SAXBOSPIRAL_SOLVE_H
#define SAXBOPHONE_SAXBOSPIRAL_SOLVE_H

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// given a spiral for which the length of all its lines are not yet known,
// calculate the length needed for each line in the spiral (to avoid line overlap)
// and store these in a spiral struct and return that
spiral_t
plot_spiral(spiral_t spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
