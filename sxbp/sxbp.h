/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides the public data types and symbols
 * exposed by the library.
 *
 * @author Joshua Saxby <joshua.a.saxby@gmail.com>
 * @date 2018
 *
 * @copyright Copyright (C) Joshua Saxby 2016-2017, 2018
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// include guard
#ifndef SAXBOPHONE_SXBP_SXBP_H
#define SAXBOPHONE_SXBP_SXBP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Used to represent a version of sxbp
 * @details Versions are of the format <MAJOR.MINOR.patch>
 * @since v0.27.0
 */
typedef struct sxbp_version_t {
    /** @brief The major version number of the version */
    uint16_t major;
    /** @brief The minor version number of the version */
    uint16_t minor;
    /** @brief The patch version number of the version */
    uint16_t patch;
    /** @brief String form of the version (vX.Y.Z) */
    const char* string;
} sxbp_version_t;

/**
 * @brief A simple buffer type for storing arrays of bytes.
 * @since v0.27.0
 */
typedef struct sxbp_buffer_t {
    /** @brief pointer to array of bytes */
    uint8_t* bytes;
    /** @brief the size of the array of bytes */
    size_t size;
} sxbp_buffer_t;

/**
 * @brief Type for representing one of the cartesian directions.
 * @since v0.54.0
 */
typedef enum sxbp_direction_t {
    SXBP_UP = 0, /**< The cartesian direction 'UP' */
    SXBP_RIGHT = 1, /**< The cartesian direction 'RIGHT' */
    SXBP_DOWN = 2, /**< The cartesian direction 'DOWN' */
    SXBP_LEFT = 3, /**< The cartesian direction 'LEFT' */
} sxbp_direction_t;

/**
 * @brief Type for representing the length of a line segment of a spiral.
 * @note Although the width of this type is 32 bits, it is actually only 30 bits
 * when used in the sxbp_spiral_t struct type. This is because here it is a
 * bitfield field with 30 bits allocated to it.
 * @since v0.27.0
 */
typedef uint32_t sxbp_length_t;

/**
 * @brief Represents one line segment in the spiral structure.
 * @details This includes the direction of the line and it's length
 * (initially set to 0).
 * @note The whole struct uses bitfields to occupy 32 bits of memory.
 * @since v0.27.0
 */
typedef struct sxbp_line_t {
    /** @brief uses 2 bits as there's only 4 directions */
    sxbp_direction_t direction : 2;
    /** @brief uses 30 bits for the length, this is wide enough */
    sxbp_length_t length : 30;
} sxbp_line_t;

/**
 * @brief A structure representing an SXBP 'spiral' figure
 * @details The figure can be in any state of completion or non-completion
 * @since v0.54.0
 */
typedef struct sxbp_figure_t {
    /** @brief The total number of lines in the figure */
    uint32_t size;
    /** @brief The lines that make up the figure */
    sxbp_line_t* lines;
    /**
     * @brief The number of unsolved lines that are remaining
     * @details A line that has not been 'solved' is a line that hasn't been
     * shortened to its minimum possible length
     * @note While this is greater than zero, it is the index of the next line
     * that needs solving + 1
     */
    uint32_t lines_remaining;
} sxbp_figure_t;

/**
 * @brief Used to represent a basic 1-bit, pure black/white bitmap image.
 * @details The image has integer height and width, and a 2-dimensional array of
 * 1-bit pixels which are either black or white.
 * @since v0.54.0
 */
typedef struct sxbp_bitmap_t {
    /** @brief The width of the bitmap in pixels */
    uint32_t width;
    /** @brief The height of the bitmap in pixels */
    uint32_t height;
    /**
     * @brief A 2-dimensional array of pixels.
     * @details false is the background colour, true is the foreground colour
     * (the line colour)
     */
    bool** pixels;
} sxbp_bitmap_t;

/**
 * @brief Stores the current version of sxbp.
 * @since v0.27.0
 */
extern const sxbp_version_t SXBP_VERSION;

/**
 * @brief Attempts to allocate memory for the bytes of the given buffer
 * @details Attempts to allocate the amount of memory specified by the `size`
 * member of the buffer
 * @warning It is unsafe to call this function on a buffer that has already
 * been allocated
 * @returns `true` if memory was allocated successfully
 * @returns `false` if memory was not allocated successfully
 * @since v0.54.0
 */
bool sxbp_init_buffer(sxbp_buffer_t* buffer);

/**
 * @brief Deallocates any allocated memory for the bytes of the given buffer
 * @details It is safe to call this function multiple times on the same buffer
 * @warning It is unsafe to call this function on a buffer that has been
 * deallocated without setting the `bytes` pointer member to `NULL`.
 * @returns `true` if memory was deallocated
 * @returns `false` if memory did not need to be deallocated (this is not an
 * error condition)
 * @since v0.54.0
 */
bool sxbp_free_buffer(sxbp_buffer_t* buffer);

/**
 * @brief Attempts to copy one buffer to another
 * @details All the data is copied from the buffer such that both are entirely
 * separate at the completion of the operation. Any data in the `to` buffer
 * will be overwritten or erased.
 * @param from The buffer to copy the contents of
 * @param[out] The buffer to copy the contents to
 * @warning The buffer to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_buffer(sxbp_buffer_t* from, sxbp_buffer_t* to);

/**
 * @brief Attempts to allocate memory for dynamic members of the given figure
 * @details Attempts to allocate the number of lines specified by the `size`
 * member of the figure, and memory for other private fields of the structure
 * as needed.
 * @warning It is unsafe to call this function on a figure that has already
 * been allocated
 * @returns `true` if all memory was allocated successfully
 * @returns `false` if any memory was not allocated successfully
 * @since v0.54.0
 */
bool sxbp_init_figure(sxbp_figure_t* figure);

/**
 * @brief Deallocates any allocated memory for the given figure
 * @details It is safe to call this function multiple times on the same figure
 * @warning It is unsafe to call this function on a figure that has had any
 * dynamically allocated members deallocated without setting their respective
 * pointers to `NULL`.
 * @returns `true` if any memory was deallocated
 * @returns `false` if no memory needed to be deallocated (this is not an error
 * condition)
 * @since v0.54.0
 */
bool sxbp_free_figure(sxbp_figure_t* figure);

/**
 * @brief Attempts to copy one figure to another
 * @details All the data is copied from the figure such that both are entirely
 * separate at the completion of the operation. Any data in the `to` figure
 * will be overwritten or erased.
 * @param from The figure to copy the contents of
 * @param[out] The figure to copy the contents to
 * @warning The figure to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_figure(sxbp_figure_t* from, sxbp_figure_t* to);

/**
 * @brief Attempts to allocate memory for the pixels of the given bitmap
 * @details Attempts to allocate the memory for the amount of pixels specified
 * by the `width` and `height` members of the bitmap
 * @warning It is unsafe to call this function on a bitmap that has already
 * been allocated
 * @returns `true` if memory was allocated successfully
 * @returns `false` if memory was not allocated successfully
 * @since v0.54.0
 */
bool sxbp_init_bitmap(sxbp_bitmap_t* bitmap);

/**
 * @brief Deallocates any allocated memory for the pixels of the given bitmap
 * @details It is safe to call this function multiple times on the same bitmap
 * @warning It is unsafe to call this function on a bitmap that has been
 * deallocated without setting the `pixels` pointer member to `NULL`.
 * @returns `true` if memory was deallocated
 * @returns `false` if memory did not need to be deallocated (this is not an
 * error condition)
 * @since v0.54.0
 */
bool sxbp_free_bitmap(sxbp_bitmap_t* bitmap);

/**
 * @brief Attempts to copy one bitmap to another
 * @details All the data is copied from the bitmap such that both are entirely
 * separate at the completion of the operation. Any data in the `to` bitmap
 * will be overwritten or erased.
 * @param from The bitmap to copy the contents of
 * @param[out] The bitmap to copy the contents to
 * @warning The bitmap to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_bitmap(sxbp_bitmap_t* from, sxbp_bitmap_t* to);

/**
 * @brief Converts the given binary data into an early-draft SXBP figure
 * @details The data in the given buffer is converted into a sequence of spiral
 * directions and from these an unrefined rudimentary line is plotted in the
 * figure (by setting the directions and lengths of the figure's lines).
 * @param data The buffer containing data to generate the figure from
 * @param[out] figure The figure in which to generate the line. This will be
 * erased before data is written to it.
 * @note The shape that can be derived from this data will waste a lot of visual
 * space and should be refined by calling `sxbp_refine_figure`
 * @returns `true` if the figure could be successfully generated
 * @returns `false` if the figure could not be successfully generated
 * @since v0.54.0
 */
bool sxbp_begin_figure(sxbp_buffer_t* data, sxbp_figure_t* figure);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
