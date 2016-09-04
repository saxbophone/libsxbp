#ifndef SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// constants related to how spiral data is packed in files - measured in bytes
extern const size_t FILE_HEADER_SIZE;
extern const size_t LINE_T_PACK_SIZE;

/*
 * given a buffer and a pointer to a blank spiral_t, create a spiral represented
 * by the data in the struct and write to the spiral
 * TODO: Check if the return type of this can be void, or if we should return
 * something else as a status report.
 */
void
load_spiral(buffer_t buffer, spiral_t * spiral);

/*
 * given a spiral_t struct and a pointer to a blank buffer_t, serialise a binary
 * representation of the spiral and write this to the data buffer pointer
 * TODO: Check if the return type of this can be void, or if we should return
 * something else as a status report.
 */
void
dump_spiral(spiral_t spiral, buffer_t * buffer);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
