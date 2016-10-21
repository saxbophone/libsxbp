/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides most of the data types used by the library, as
 * well as defining some library constants and versioning functionality.
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
#ifndef SAXBOPHONE_SAXBOSPIRAL_SAXBOSPIRAL_H
#define SAXBOPHONE_SAXBOSPIRAL_SAXBOSPIRAL_H

// sanity check for support of 64-bit integers
#if __SIZEOF_SIZE_T__ < 8
#warning "Please compile this code for a target with 64-bit words or greater."
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"{
#endif

typedef struct sxbp_version_t {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    const char* string;
} sxbp_version_t;

extern const sxbp_version_t LIB_SXBP_VERSION;

// used for indexing and comparing different versions in order
typedef uint32_t sxbp_version_hash_t;

/*
 * computes a version_hash_t for a given version_t,
 * to be used for indexing and ordering
 */
sxbp_version_hash_t sxbp_version_hash(sxbp_version_t version);

// struct for storing the location of a DEBUG invocation
typedef struct sxbp_debug_t {
    size_t line;
    char* file;
    const char* function;
} sxbp_debug_t;

/*
 * handy short-hand for debugging purposes
 * usage: debug_t debug = DEBUG;
 */
#define SXBP_DEBUG (sxbp_debug_t) { .line = __LINE__, .file = __FILE__, .function = __func__, }

// enum for function error information
typedef enum sxbp_diagnostic_t {
    SXBP_STATE_UNKNOWN = 0, // unknown, the default state
    SXBP_OPERATION_FAIL, // generic failure state
    SXBP_MALLOC_REFUSED, // memory allocation or re-allocation was refused
    SXBP_IMPOSSIBLE_CONDITION, // condition thought to be impossible detected
    SXBP_OPERATION_OK, // no problem
} sxbp_diagnostic_t;

// struct for storing generic diagnostics about function failure reasons
typedef struct sxbp_status_t {
    sxbp_debug_t location; // for storing location of error
    sxbp_diagnostic_t diagnostic; // for storing error information (if any)
} sxbp_status_t;

// type for representing a cartesian direction
typedef uint8_t sxbp_direction_t;

// cartesian direction constants
#define SXBP_UP 0
#define SXBP_RIGHT 1
#define SXBP_DOWN 2
#define SXBP_LEFT 3

// type for representing a rotational direction
typedef int8_t sxbp_rotation_t;

// rotational direction constants
#define SXBP_CLOCKWISE 1
#define SXBP_ANTI_CLOCKWISE -1

// type for representing the length of a line segment of a spiral
typedef uint32_t sxbp_length_t;

/* 
 * struct for representing one line segment in the spiral structure, including
 * the direction of the line and it's length (initially set to 0) 
 * the whole struct uses bitfields to occupy 32 bits of memory
 */
typedef struct sxbp_line_t {
    sxbp_direction_t direction : 2; // as there are only 4 directions, use 2 bits
    sxbp_length_t length : 30; // use 30 bits for the length, nice and long
} sxbp_line_t;

// type for representing the range of one tuple item
typedef int64_t sxbp_tuple_item_t;

typedef struct sxbp_tuple_t {
    sxbp_tuple_item_t x;
    sxbp_tuple_item_t y;
} sxbp_tuple_t;

typedef sxbp_tuple_t sxbp_vector_t;
typedef sxbp_tuple_t sxbp_co_ord_t;

typedef struct sxbp_co_ord_array_t {
    sxbp_co_ord_t* items;
    size_t size;
} sxbp_co_ord_array_t;

typedef struct sxbp_co_ord_cache_t {
    sxbp_co_ord_array_t co_ords;
    size_t validity;
} sxbp_co_ord_cache_t;

typedef struct sxbp_spiral_t {
    uint64_t size; // count of lines
    sxbp_line_t* lines; // dynamic array of lines
    sxbp_co_ord_cache_t co_ord_cache; // co-ord cache for lines
    bool collides; // whether this spiral collides or not
    uint64_t collider; // the index of the line causing collision, if any
    /*
     * NOTE: The remaining fields are currently unused by the solver/generator
     * code, but they are read to and written from files. They will be used
     * by the rest of the code in future versions.
     */
    uint64_t solved_count; // the count of lines solved so far (index of next)
    uint32_t seconds_spent; // count of seconds spent solving the spiral
} sxbp_spiral_t;

typedef struct sxbp_buffer_t {
    uint8_t* bytes;
    size_t size;
} sxbp_buffer_t;

// vector direction constants
extern const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4];

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
