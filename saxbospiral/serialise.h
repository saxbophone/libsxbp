#ifndef SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// for providing further error information specific to spiral deserialisation
typedef enum sxbp_deserialise_diagnostic_t {
    DESERIALISE_OK, // no problem
    DESERIALISE_BAD_HEADER_SIZE, // header section too small to be valid
    DESERIALISE_BAD_MAGIC_NUMBER, // wrong magic number in header section
    DESERIALISE_BAD_VERSION, // unsupported data version (according to header)
    DESERIALISE_BAD_DATA_SIZE, // data section too small to be valid
} sxbp_deserialise_diagnostic_t;

/*
 * for storing both generic and specific error information about serialisation
 * operations in one place
 */
typedef struct sxbp_serialise_result_t {
    sxbp_status_t status; // generic error information applicable to all functions
    sxbp_deserialise_diagnostic_t diagnostic; // additional specific error information
} sxbp_serialise_result_t;

// constants related to how spiral data is packed in files - measured in bytes
extern const size_t SXBP_FILE_HEADER_SIZE;
extern const size_t SXBP_LINE_T_PACK_SIZE;

/*
 * given a buffer and a pointer to a blank spiral_t, create a spiral represented
 * by the data in the struct and write to the spiral
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 */
sxbp_serialise_result_t sxbp_load_spiral(
    sxbp_buffer_t buffer, sxbp_spiral_t* spiral
);

/*
 * given a spiral_t struct and a pointer to a blank buffer_t, serialise a binary
 * representation of the spiral and write this to the data buffer pointer
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 */
sxbp_serialise_result_t sxbp_dump_spiral(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
