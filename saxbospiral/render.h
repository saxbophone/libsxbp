#ifndef SAXBOPHONE_SAXBOSPIRAL_RENDER_H
#define SAXBOPHONE_SAXBOSPIRAL_RENDER_H

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
