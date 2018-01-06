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
 * @brief A structure representing an SXBP 'spiral' figure
 * @since v0.54.0
 */
typedef struct sxbp_figure_t {
    /** @todo Remove this placeholder member */
    void* placeholder;
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

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
