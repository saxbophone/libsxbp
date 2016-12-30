/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides most of the data types used by the
 * library, as well as defining some library constants and versioning
 * functionality.
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016
 *
 * @copyright Copyright (C) Joshua Saxby 2016
 *
 * @copyright This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License
 * (version 3), as published by the Free Software Foundation.
 *
 * @copyright This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * @copyright You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_SAXBOSPIRAL_H
#define SAXBOPHONE_SAXBOSPIRAL_SAXBOSPIRAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Used to represent a version of libsxbp
 * @details Versions are of the format <MAJOR.MINOR.patch>
 */
typedef struct sxbp_version_t {
    uint8_t major; /**< The major version number of the version */
    uint8_t minor; /**< The minor version number of the version */
    uint8_t patch; /**< The patch version number of the version */
    const char* string; /**< String form of the version (vX.Y.Z) */
} sxbp_version_t;

/** @brief Stores the current version of libsxbp. */
extern const sxbp_version_t LIB_SXBP_VERSION;

/**
 * @brief Used for indexing and comparing different versions in order.
 * @details Versions that are behind a given version will compare as less than
 * that version. Versions that are ahead will compare as greater than.
 */
typedef uint32_t sxbp_version_hash_t;

/**
 * @brief Computes a comparison value for a given version.
 *
 * @param version The version to be computed.
 * @return A scalar value which can be used to order or index any two versions.
 */
sxbp_version_hash_t sxbp_version_hash(sxbp_version_t version);

/**
 * @brief Represents the success or failure status of an executed function.
 * @details Not all functions have this as their return type, but most do.
 * Functions that may encounter error conditions which need to be propagated
 * back to the caller will have this type as their return type.
 */
typedef enum sxbp_status_t {
    SXBP_STATE_UNKNOWN = 0, /**< unknown, the default state */
    SXBP_OPERATION_OK, /**< no problem */
    SXBP_OPERATION_FAIL, /**< generic failure state */
    SXBP_MALLOC_REFUSED, /**< memory allocation or re-allocation was refused */
    SXBP_IMPOSSIBLE_CONDITION, /**< condition thought to be impossible detected */
    SXBP_NOT_IMPLEMENTED, /**< function is not implemented / enabled */
} sxbp_status_t;

/**
 * @brief Type for representing one of the cartesian directions.
 * @details This can be one of SXBP_UP, SXBP_RIGHT, SXBP_DOWN or SXBP_LEFT.
 * @todo This should be changed to an enum.
 */
typedef uint8_t sxbp_direction_t;

#define SXBP_UP 0 /**< The cartesian direction 'UP' */
#define SXBP_RIGHT 1 /**< The cartesian direction 'RIGHT' */
#define SXBP_DOWN 2 /**< The cartesian direction 'DOWN' */
#define SXBP_LEFT 3 /**< The cartesian direction 'LEFT' */

/**
 * @brief Type for representing a rotational direction.
 * @details This can be SXBP_CLOCKWISE or SXBP_ANTI_CLOCKWISE.
 * @todo This might need to be changed to an enum.
 */
typedef int8_t sxbp_rotation_t;

#define SXBP_CLOCKWISE 1 /**< The rotational direction 'CLOCKWISE' */
#define SXBP_ANTI_CLOCKWISE -1 /**< The rotational direction 'ANTI-CLOCKWISE' */

/**
 * @brief Type for representing the length of a line segment of a spiral.
 * @note Although the width of this type is 32 bits, it is actually only 30 bits
 * when used in the sxbp_spiral_t struct type. This is because here it is a
 * bitfield field with 30 bits allocated to it.
 */
typedef uint32_t sxbp_length_t;

/**
 * @brief Represents one line segment in the spiral structure.
 * @details This includes the direction of the line and it's length
 * (initially set to 0).
 * @note The whole struct uses bitfields to occupy 32 bits of memory.
 */
typedef struct sxbp_line_t {
    sxbp_direction_t direction : 2; /**< uses 2 bits as there's only 4 directions*/
    sxbp_length_t length : 30; /**< uses 30 bits for the length */
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
