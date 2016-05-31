#ifndef SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H

#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// constants related to how spiral data is packed in files - measured in bytes
extern const size_t FILE_HEADER_SIZE;
extern const size_t LINE_T_PACK_SIZE;

/*
 * given a buffer, return a spiral represented by the data in the struct
 * returns a spiral of length 0 if the data could not be interpreted correctly
 */
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
