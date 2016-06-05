#ifndef SAXBOPHONE_SAXBOSPIRAL_RENDER_H
#define SAXBOPHONE_SAXBOSPIRAL_RENDER_H

// sanity check for support of 64-bit integers
#if __SIZEOF_SIZE_T__ < 8
#warning "Please compile this code for a target with 64-bit words or greater."
#endif

#include <stdbool.h>
#include <stdint.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef struct bitmap_t {
    uint64_t width;
    uint64_t height;
    bool ** pixels;
} bitmap_t;

/*
 * given a spiral struct, returns a bitmap_t representing a monochromatic image
 * of the rendered spiral
 */
bitmap_t
render_spiral(spiral_t spiral);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
