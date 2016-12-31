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
    /** @brief The major version number of the version */
    uint8_t major;
    /** @brief The minor version number of the version */
    uint8_t minor;
    /** @brief The patch version number of the version */
    uint8_t patch;
    /** @brief String form of the version (vX.Y.Z) */
    const char* string;
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
    /** @brief uses 2 bits as there's only 4 directions */
    sxbp_direction_t direction : 2;
    /** @brief uses 30 bits for the length, this is wide enough */
    sxbp_length_t length : 30;
} sxbp_line_t;

/** @brief Type for storing one of the items of a tuple. */
typedef int64_t sxbp_tuple_item_t;

/**
 * @brief A generic Tuple type for storing a vector-based quantity.
 */
typedef struct sxbp_tuple_t {
    /** @brief The x (across) value of the tuple */
    sxbp_tuple_item_t x;
    /** @brief The y (down) value of the tuple */
    sxbp_tuple_item_t y;
} sxbp_tuple_t;

/** @brief A Vector type used for representing directions. */
typedef sxbp_tuple_t sxbp_vector_t;
/** @brief A co-ord type used for representing cartesian co-ordinates. */
typedef sxbp_tuple_t sxbp_co_ord_t;

/**
 * @brief Struct type for holding a dynamically allocated array of co-ordinates.
 */
typedef struct sxbp_co_ord_array_t {
    /** @brief pointer to the array of co-ordinates */
    sxbp_co_ord_t* items;
    /** @brief size of the array in number of items */
    size_t size;
} sxbp_co_ord_array_t;

/**
 * @brief Struct type for holding a cached set of co-ords.
 * @details This is useful for co-ords that are intended for re-use between
 * operations which require co-ords which are certified as being correct up to a
 * given line index.
 */
typedef struct sxbp_co_ord_cache_t {
    /** @brief the co-ord array containing the cached co-ords */
    sxbp_co_ord_array_t co_ords;
    /** @brief the index of the spiral line for which this set of cached co-ords
     * is valid up to */
    size_t validity;
} sxbp_co_ord_cache_t;

/**
 * @brief Struct type representing a Spiral figure, in any state of completion.
 * @details This is the most important data type in the whole library, and is
 * passed to and from many different library functions. A partially-complete
 * spiral object may be produced from binary data, and a fully-complete spiral
 * may be produced from the partially complete one via the routines in the
 * library.
 * @note The solved_count and seconds_spent fields are currently unused by the
 * solver/generator code, but they are read to and written from files. They will
 * be used by the rest of the code in future versions.
 */
typedef struct sxbp_spiral_t {
    /** @brief count of lines in the spiral */
    uint64_t size;
    /** @brief dynamic array of lines in the spiral */
    sxbp_line_t* lines;
    /**
     * @brief co-ord cache for the lines
     * @private
     */
    sxbp_co_ord_cache_t co_ord_cache;
    /** @brief whether this spiral collides or not */
    bool collides;
    /**
     * @brief the index of the line causing collision, if any
     * @private
     */
    uint64_t collider;
    /**
     * @brief the count of lines solved so far (index of next line to solve)
     * @note Currently not used by solver/generator
     */
    uint64_t solved_count;
    /**
     * @brief count of seconds spent solving the spiral
     * @note Currently not used by solver/generator
     */
    uint32_t seconds_spent;
} sxbp_spiral_t;

/** @brief A simple buffer type for storing arrays of bytes. */
typedef struct sxbp_buffer_t {
    /** @brief pointer to array of bytes */
    uint8_t* bytes;
    /** @brief the size of the array of bytes */
    size_t size;
} sxbp_buffer_t;

/**
 * @brief Vector direction constants.
 * @details This is an array of vectors representing each cartesian direction as
 * a vector direction. They can be indexed by the cartesian direction macros,
 * for example:
 * @code
 * sxbp_vector_t up = SXBP_VECTOR_DIRECTIONS[SXBP_UP];
 * @endcode
 */
extern const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4];

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
