/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for serialising spirals to and from
 * byte sequences (which may be stored in files or any other form).
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License (version 3),
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// for providing further error information specific to spiral deserialisation
typedef enum sxbp_deserialise_diagnostic_t {
    SXBP_DESERIALISE_OK, // no problem
    SXBP_DESERIALISE_BAD_HEADER_SIZE, // header section too small to be valid
    SXBP_DESERIALISE_BAD_MAGIC_NUMBER, // wrong magic number in header section
    SXBP_DESERIALISE_BAD_VERSION, // unsupported data version (according to header)
    SXBP_DESERIALISE_BAD_DATA_SIZE, // data section too small to be valid
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

// some macros for making file serialisation code more readable
#define SXBP_SOH "\x01"
#define SXBP_STX "\x02"
#define SXBP_ETX "\x03"
#define SXBP_US "\x1f"

/*
 * given a buffer and a pointer to a blank spiral_t, create a spiral represented
 * by the data in the struct and write to the spiral
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 *
 * Asserts:
 * - That buffer.bytes is not NULL
 * - That spiral->lines is NULL
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
 *
 * Asserts:
 * - That spiral.lines is not NULL
 * - That buffer->bytes is NULL
 */
sxbp_serialise_result_t sxbp_dump_spiral(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
