/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This header file provides the entire public API of sxbp
 *
 * @author Joshua Saxby <joshua.a.saxby@gmail.com>
 * @date 2016-2017, 2018
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
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
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
 * @brief Type for representing the size of an SXBP figure
 * @details In other words, this is the type for storing the count of lines in
 * the figure.
 * @since v0.54.0
 */
typedef uint32_t sxbp_figure_size_t;

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
    sxbp_figure_size_t size;
    /** @brief The lines that make up the figure */
    sxbp_line_t* lines;
    /**
     * @brief The number of unsolved lines that are remaining
     * @details A line that has not been 'solved' is a line that hasn't been
     * shortened to its minimum possible length
     * @note While this is greater than zero, it is the index of the next line
     * that needs solving + 1
     */
    sxbp_figure_size_t lines_remaining;
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
 * @brief Used to represent success/failure states for certain functions in SXBP
 * @details For functions which can encounter error conditions, this type
 * enumerates the possible error conditions that can occur
 * @note Values `SXBP_RESULT_RESERVED_START` through `SXBP_RESULT_RESERVED_END`
 * inclusive are reserved for future use. If a value equal to or greater than
 * `SXBP_RESULT_RESERVED_START` is encountered, the caller can assume that
 * either a new code for which it has no definition has been returned, or that
 * the value is garbage.
 * @since v0.54.0
 */
typedef enum sxbp_result_t {
    SXBP_RESULT_UNKNOWN = 0u, /**< unknown, the default */
    SXBP_RESULT_OK, /**< success */
    SXBP_RESULT_FAIL_MEMORY, /**< failure to allocate or reallocate memory */
    SXBP_RESULT_FAIL_PRECONDITION, /**< a preconditional check failed */
    SXBP_RESULT_FAIL_FILE, /**< a file read/write operation failed */
    SXBP_RESULT_RESERVED_START, /**< reserved for future use */
    SXBP_RESULT_RESERVED_END = 255u, /**< reserved for future use */
} sxbp_result_t;

/**
 * @brief Stores the current version of sxbp.
 * @since v0.27.0
 */
extern const sxbp_version_t SXBP_VERSION;

/**
 * @brief Creates a blank empty buffer
 * @details Ensures that all pointer fields are properly initialised to NULL
 * @returns An `sxbp_buffer_t` object with all fields set to default/blank value
 * @since v0.54.0
 */
sxbp_buffer_t sxbp_blank_buffer(void);

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
 * @note It is safe to call this function multiple times on the same buffer
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
 * @param[out] to The buffer to copy the contents to
 * @warning The buffer to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_buffer(const sxbp_buffer_t* from, sxbp_buffer_t* to);

/**
 * @brief Attempts to read the contents of the given file into the given buffer
 * @details Allocates the buffer and copies all the bytes of the file into it
 * @warning The file should have been opened in `rb` mode
 * @param file_handle The file to read data from
 * @param[out] buffer The buffer to write data to
 * @returns `true` on successfully copying the file contents
 * @returns `false` on failure to copy the file contents
 * @since v0.54.0
 */
bool sxbp_buffer_from_file(FILE* file_handle, sxbp_buffer_t* buffer);

/**
 * @brief Attempts to write the contents of the given buffer to the given file
 * @details Writes all the bytes in the buffer out to the open file
 * @warning The file should have been opened in `wb` mode
 * @param buffer The buffer to read data from
 * @param[out] file_handle The file to write data to
 * @returns `true` on successfully writing the file
 * @returns `false` on failure to write the file
 * @since v0.54.0
 */
bool sxbp_buffer_to_file(const sxbp_buffer_t* buffer, FILE* file_handle);

/**
 * @brief Creates a blank empty figure
 * @details Ensures that all pointer fields are properly initialised to NULL
 * @returns An `sxbp_figure_t` object with all fields set to default/blank value
 * @since v0.54.0
 */
sxbp_figure_t sxbp_blank_figure(void);

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
 * @note It is safe to call this function multiple times on the same figure
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
 * @param[out] to The figure to copy the contents to
 * @warning The figure to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_figure(const sxbp_figure_t* from, sxbp_figure_t* to);

/**
 * @brief Creates a blank empty bitmap
 * @details Ensures that all pointer fields are properly initialised to NULL
 * @returns An `sxbp_bitmap_t` object with all fields set to default/blank value
 * @since v0.54.0
 */
sxbp_bitmap_t sxbp_blank_bitmap(void);

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
 * @note It is safe to call this function multiple times on the same bitmap
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
 * @param[out] to The bitmap to copy the contents to
 * @warning The bitmap to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `true` if the data was copied successfully
 * @returns `false` if the data was not copied successfully, in which case `to`
 * will be empty.
 * @since v0.54.0
 */
bool sxbp_copy_bitmap(const sxbp_bitmap_t* from, sxbp_bitmap_t* to);

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
 * @todo Add options struct (at least one option, max number of lines)
 * @returns `true` if the figure could be successfully generated
 * @returns `false` if the figure could not be successfully generated
 * @since v0.54.0
 */
bool sxbp_begin_figure(const sxbp_buffer_t* data, sxbp_figure_t* figure);

/**
 * @brief Refines the line lengths of the given SXBP figure
 * @details The line lengths are adjusted to take up less space than that
 * which are generated by `sxbp_begin_figure`
 * @note This function will fail immediately if the SXBP figure that is passed
 * to it has colliding lines, or no lines
 * @warning This function may take a long time to execute
 * @todo Add extended options (as struct) including progress callback (most
 * important)
 * @returns `true` if the figure could be successfully refined
 * @returns `false` if the figure could not be successfully refined
 * @since v0.54.0
 */
bool sxbp_refine_figure(sxbp_figure_t* figure);

/**
 * @brief Serialises the given figure to data, stored in the given buffer
 * @details The buffer is populated with bytes which represent the figure and
 * from whch compatible versions of SXBP can load figures from again
 * @note The buffer is erased before being populated
 * @param figure The figure to serialise
 * @param[out] buffer The buffer to store the serialised data in
 * @returns `true` if the figure could be successfully serialised
 * @returns `false` if the figure could not be successfully serialised
 * @since v0.54.0
 */
bool sxbp_dump_figure(const sxbp_figure_t* figure, sxbp_buffer_t* buffer);

/**
 * @brief Attempts to deserialise an SXBP figure from the given buffer
 * @details A serialised SXBP figure is extracted from the buffer's data to the
 * given figure, if the buffer data is valid, containing an SXBP figure in the
 * data format this version of SXBP understands
 * @note The figure is overwritten in this process
 * @param buffer The buffer to load the SXBP figure from
 * @param[out] figure The figure to store the deserialised SXBP figure in
 * @returns `true` if the figure could be successfully deserialised
 * @returns `false` if the figure could not be successfully deserialised
 * @since v0.54.0
 */
bool sxbp_load_figure(const sxbp_buffer_t* buffer, sxbp_figure_t* figure);

/**
 * @brief Rasterises an image of the given figure to a basic bitmap object
 * @details A 1-bit black/white bitmap image of the line formed by the SXBP
 * figure is rendered to the given bitmap object
 * @note The bitmap is erased before the output is written to it
 * @param figure The SXBP figure to render
 * @param[out] bitmap The bitmap to write the output to
 * @returns `true` if the figure could be rendered successfully
 * @returns `false` if the figure could not be rendered successfully
 */
bool sxbp_render_figure(const sxbp_figure_t* figure, sxbp_bitmap_t* bitmap);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
