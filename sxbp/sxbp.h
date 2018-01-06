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

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
