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
    SXBP_UP = 0u, /**< The cartesian direction 'UP' */
    SXBP_RIGHT = 1u, /**< The cartesian direction 'RIGHT' */
    SXBP_DOWN = 2u, /**< The cartesian direction 'DOWN' */
    SXBP_LEFT = 3u, /**< The cartesian direction 'LEFT' */
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
     * that needs solving
     */
    sxbp_figure_size_t lines_remaining;
} sxbp_figure_t;

/**
 * @brief A structure used for providing options to `sxbp_begin_figure()`
 * @since v0.54.0
 */
typedef struct sxbp_begin_figure_options_t {
    /**
     * @brief The maximum number of lines to create in the figure
     * @details If `0`, then the maximum number of lines possible (based on the
     * input data size) is used.
     * @note If this is greater than the maximum number of lines possible, then
     * the latter is used.
     */
    sxbp_figure_size_t max_lines;
} sxbp_begin_figure_options_t;

/**
 * @brief Used to specify which figure refinement method should be used
 * @details There are a few different ways that a figure can be 'refined' to be
 * smaller, this type is used to represent all of the methods currently
 * implemented by the library
 * @note Values `SXBP_REFINE_METHOD_RESERVED_START` through
 * `SXBP_REFINE_METHOD_RESERVED_END` inclusive are reserved for future use.
 * If a value equal to or greater than `SXBP_REFINE_METHOD_RESERVED_START` is
 * encountered, the caller can assume that either a new code for which it has no
 * definition has been returned, or that the value is garbage.
 * @since v0.54.0
 */
typedef enum sxbp_refine_method_t {
    SXBP_REFINE_METHOD_ANY = 0u, /**< use any method, the default */
    SXBP_REFINE_METHOD_GROW_FROM_START, /**< the original refinement method */
    SXBP_REFINE_METHOD_SHRINK_FROM_END, /**< the current refinement method */
    SXBP_REFINE_METHOD_RESERVED_START = 10u, /**< reserved for future use */
    SXBP_REFINE_METHOD_RESERVED_END = 255u, /**< reserved for future use */
} sxbp_refine_method_t;

/**
 * @brief A structure used for providing options to `sxbp_refine_figure()`
 * @since v0.54.0
 */
typedef struct sxbp_refine_figure_options_t {
    /**
     * @begin The method to be used to refine the figure
     */
    sxbp_refine_method_t refine_method;
    /**
     * @brief An optional callback to be called every time a new line is solved.
     */
    void(* progress_callback)(
        const sxbp_figure_t* const figure,
        void* callback_context
    );
    /**
     * @brief An optional context to be given to the `progress_callback` when
     * called.
     * @details Use this to provide any state required by your callback, if you
     * have one. A common pattern is to pass a pointer to a struct containing
     * state variables you might need to write out a message, save a file, etc.
     */
    void* callback_context;
} sxbp_refine_figure_options_t;

/**
 * @brief A structure used for providing options to `sxbp_render_figure()`
 * @todo Add more options such as background and line colours (RGBA),
 * alternative colour for the starting dot, etc...
 * @since v0.54.0
 */
typedef struct sxbp_render_options_t {
    /** @brief The scale factor to render the image to */
    size_t scale;
} sxbp_render_options_t;

/**
 * @brief Type for representing the width or height of a rendered SXBP figure
 * @details This type is that used for representing the width and height of
 * rendered images of sxbp figures.
 * @since v0.54.0
 */
typedef uint32_t sxbp_figure_dimension_t;

/**
 * @brief Used to represent a basic 1-bit, pure black/white bitmap image.
 * @details The image has integer height and width, and a 2-dimensional array of
 * 1-bit pixels which are either black or white.
 * @since v0.54.0
 */
typedef struct sxbp_bitmap_t {
    /** @brief The width of the bitmap in pixels */
    sxbp_figure_dimension_t width;
    /** @brief The height of the bitmap in pixels */
    sxbp_figure_dimension_t height;
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
    SXBP_RESULT_FAIL_IO, /**< an input/output operation failed */
    SXBP_RESULT_FAIL_UNIMPLEMENTED, /**< requested action is not implemented */
    SXBP_RESULT_RESERVED_START, /**< reserved for future use */
    SXBP_RESULT_RESERVED_END = 255u, /**< reserved for future use */
} sxbp_result_t;

/**
 * @brief A convenience typedef for a callback function that renders a figure
 * @details This is used in `sxbp_render_figure()` and should render a figure to
 * an image, serialised and stored in the given buffer.
 * @todo Try and use Doxygen's `at-param` syntax to document the arguments and
 * use `at-returns` to document the return details.
 */
typedef sxbp_result_t(* sxbp_figure_renderer_t)(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
);

/**
 * @brief Stores the current version of sxbp.
 * @since v0.27.0
 */
extern const sxbp_version_t SXBP_VERSION;

/**
 * @brief The maximum size of buffer that can be used to begin a figure
 * @details This ensures that a figure will not produce a line which will exceed
 * the maximum bounds of the grid in which it is plotted. It is derived like so:
 * `(((2^32 - 1) × 2) - 1) ÷ 8 = 1073741823.625` and this is due to 32-bit
 * bounds being used for the figure's grid. This means no file that is 1GiB in
 * size or larger can be used to create a figure, but less than 1GiB is fine.
 */
extern const size_t SXBP_BEGIN_BUFFER_MAX_SIZE;

/**
 * @brief The default options used for `sxbp_begin_figure()`
 */
extern const sxbp_begin_figure_options_t SXBP_BEGIN_FIGURE_OPTIONS_DEFAULT;

/**
 * @brief The default figure refinement method used by `sxbp_refine_figure()`
 */
extern const sxbp_refine_method_t SXBP_REFINE_METHOD_DEFAULT;

/**
 * @brief Returns if a given `sxbp_result_t` is successful or not
 * @details This is intended to be used to easily check the return status of
 * functions in SXBP that can raise errors.
 * @param state The state to check for success/failure
 * @returns `true` if the given status code was success
 * @returns `false` if the given status code was not success
 */
bool sxbp_success(sxbp_result_t state);

/**
 * @brief Checks if a given `sxbp_result_t` is successful or not
 * @details This is intended to be used to easily check the return status of
 * functions in SXBP that can raise errors.
 * @param state The state to check for success/failure
 * @param[out] report_to An optional pointer to a `sxbp_result_t` to store the
 * result in, if it was failure. This is ignored if `NULL`.
 * @returns `true` if the given status code was success
 * @returns `false` if the given status code was not success
 */
bool sxbp_check(sxbp_result_t state, sxbp_result_t* const report_to);

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
 * @returns `SXBP_RESULT_OK` if memory was allocated successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if memory was not allocated successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `buffer` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_init_buffer(sxbp_buffer_t* const buffer);

/**
 * @brief Attempts to resize the given buffer
 * @details The buffer will be grown or shrunk depending on if `size` is bigger
 * than or less than it's existing size. If grown, the bytes beyond the extent
 * of its previous size will be of indeterminate value. If shrunk, the bytes
 * that lie beyond the new extent after shrinking will be lost.
 * @warning It is unsafe to call this function on a buffer that has not been
 * allocated with `sxbp_init_buffer` before.
 * @returns `SXBP_RESULT_OK` if memory was allocated successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if memory was not reallocated successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `buffer` or `buffer->bytes` are
 * `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_resize_buffer(sxbp_buffer_t* const buffer, size_t size);

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
bool sxbp_free_buffer(sxbp_buffer_t* const buffer);

/**
 * @brief Attempts to copy one buffer to another
 * @details All the data is copied from the buffer such that both are entirely
 * separate at the completion of the operation. Any data in the `to` buffer
 * will be overwritten or erased.
 * @param from The buffer to copy the contents of
 * @param[out] to The buffer to copy the contents to
 * @warning The buffer to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `SXBP_RESULT_OK` if the data was copied successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the data was not copied successfully,
 * in which case `to` will be empty.
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `from` or `to` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_copy_buffer(
    const sxbp_buffer_t* const from,
    sxbp_buffer_t* const to
);

/**
 * @brief Attempts to read the contents of the given file into the given buffer
 * @details Allocates the buffer and copies all the bytes of the file into it
 * @warning The file should have been opened in `rb` mode
 * @param file_handle The file to read data from
 * @param[out] buffer The buffer to write data to
 * @returns `SXBP_RESULT_OK` on successfully copying the file contents
 * @returns `SXBP_RESULT_FAIL_MEMORY` or `SXBP_RESULT_FAIL_IO` on failure to copy the file contents
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `file_handle` or `buffer` is
 * `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_buffer_from_file(
    FILE* file_handle,
    sxbp_buffer_t* const buffer
);

/**
 * @brief Attempts to write the contents of the given buffer to the given file
 * @details Writes all the bytes in the buffer out to the open file
 * @warning The file should have been opened in `wb` mode
 * @param buffer The buffer to read data from
 * @param[out] file_handle The file to write data to
 * @returns `SXBP_RESULT_OK` on successfully writing the file
 * @returns `SXBP_RESULT_FAIL_IO` on failure to write the file
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `buffer` or `file_handle` is
 * `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_buffer_to_file(
    const sxbp_buffer_t* const buffer,
    FILE* file_handle
);

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
 * @returns `SXBP_RESULT_OK` if all memory was allocated successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if any memory was not allocated
 * successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_init_figure(sxbp_figure_t* const figure);

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
bool sxbp_free_figure(sxbp_figure_t* const figure);

/**
 * @brief Attempts to copy one figure to another
 * @details All the data is copied from the figure such that both are entirely
 * separate at the completion of the operation. Any data in the `to` figure
 * will be overwritten or erased.
 * @param from The figure to copy the contents of
 * @param[out] to The figure to copy the contents to
 * @warning The figure to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `SXBP_RESULT_OK` if the data was copied successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the data was not copied successfully,
 * in which case `to` will be empty.
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `from` or `to` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_copy_figure(
    const sxbp_figure_t* const from,
    sxbp_figure_t* const to
);

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
 * @returns `SXBP_RESULT_OK` if memory was allocated successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if memory was not allocated successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `bitmap` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_init_bitmap(sxbp_bitmap_t* const bitmap);

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
bool sxbp_free_bitmap(sxbp_bitmap_t* const bitmap);

/**
 * @brief Attempts to copy one bitmap to another
 * @details All the data is copied from the bitmap such that both are entirely
 * separate at the completion of the operation. Any data in the `to` bitmap
 * will be overwritten or erased.
 * @param from The bitmap to copy the contents of
 * @param[out] to The bitmap to copy the contents to
 * @warning The bitmap to copy the contents to must be in a consistent state,
 * that is it must either not be allocated yet, or must be properly allocated.
 * @returns `SXBP_RESULT_OK` if the data was copied successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the data was not copied successfully,
 * in which case `to` will be empty.
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `from` or `to` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_copy_bitmap(
    const sxbp_bitmap_t* const from,
    sxbp_bitmap_t* const to
);

/**
 * @brief Converts the given binary data into an early-draft SXBP figure
 * @details The data in the given buffer is converted into a sequence of spiral
 * directions and from these an unrefined rudimentary line is plotted in the
 * figure (by setting the directions and lengths of the figure's lines).
 * @param data The buffer containing data to generate the figure from
 * @param options An optional options struct to specify additional options for
 * starting the figure. This can be `NULL`, in which case the default options
 * are used.
 * @param[out] figure The figure in which to generate the line. This will be
 * erased before data is written to it.
 * @note The shape that can be derived from this data will waste a lot of visual
 * space and should be refined by calling `sxbp_refine_figure`
 * @warning Figures cannot be created from buffers larger than
 * `SXBP_BEGIN_BUFFER_MAX_SIZE` -- an error will be returned if this is
 * attempted.
 * @returns `SXBP_RESULT_OK` if the figure could be successfully generated
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the figure could not be successfully
 * generated
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if the given buffer was too large
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `data` or `figure` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_begin_figure(
    const sxbp_buffer_t* const data,
    const sxbp_begin_figure_options_t* const options,
    sxbp_figure_t* const figure
);

/**
 * @brief Refines the line lengths of the given SXBP figure
 * @details The line lengths are adjusted to take up less space than that
 * which are generated by `sxbp_begin_figure`
 * @param[out] figure The figure to refine. This will be mutated.
 * @param options Optional options struct, currently used to supply a progress
 * callback and callback context.
 * @note This function will fail immediately if the SXBP figure that is passed
 * to it has colliding lines, or no lines
 * @warning This function may take a long time to execute
 * @returns `SXBP_RESULT_OK` if the figure could be successfully refined
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` is `NULL`
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if called on a figure with no lines
 * allocated
 * @returns `SXBP_RESULT_FAIL_UNIMPLEMENTED` if `options` specifies an
 * unimplemented refinement method.
 * @returns `SXBP_RESULT_FAIL_MEMORY` if a memory allocation error occurred when
 * refining the figure
 * @since v0.54.0
 */
sxbp_result_t sxbp_refine_figure(
    sxbp_figure_t* const figure,
    const sxbp_refine_figure_options_t* const options
);

/**
 * @brief Serialises the given figure to data, stored in the given buffer
 * @details The buffer is populated with bytes which represent the figure and
 * from whch compatible versions of SXBP can load figures from again
 * @note The buffer is erased before being populated
 * @param figure The figure to serialise
 * @param[out] buffer The buffer to store the serialised data in
 * @returns `SXBP_RESULT_OK` if the figure could be successfully serialised
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the figure could not be successfully
 * serialised
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` or `buffer` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_dump_figure(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer
);

/**
 * @brief Attempts to deserialise an SXBP figure from the given buffer
 * @details A serialised SXBP figure is extracted from the buffer's data to the
 * given figure, if the buffer data is valid, containing an SXBP figure in the
 * data format this version of SXBP understands
 * @note The figure is overwritten in this process
 * @param buffer The buffer to load the SXBP figure from
 * @param[out] figure The figure to store the deserialised SXBP figure in
 * @returns `SXBP_RESULT_OK` if the figure could be successfully deserialised
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the figure could not be deserialised
 * due to a memory error
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if the figure could not be
 * deserialised because the buffer contains invalid data or data for a version
 * of SXBP that this version cannot read
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `buffer` or `figure` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_load_figure(
    const sxbp_buffer_t* const buffer,
    sxbp_figure_t* const figure
);

/**
 * @brief Rasterises an image of the given figure to a basic bitmap object
 * @details A 1-bit black/white bitmap image of the line formed by the SXBP
 * figure is rendered to the given bitmap object
 * @note The bitmap is erased before the output is written to it
 * @param figure The SXBP figure to render
 * @param[out] bitmap The bitmap to write the output to
 * @returns `SXBP_RESULT_OK` if the figure could be rendered successfully
 * @returns `SXBP_RESULT_FAIL_MEMORY` if the figure could not be rendered
 * successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` or `bitmap` is `NULL`
 * @since v0.54.0
 */
sxbp_result_t sxbp_render_figure_to_bitmap(
    const sxbp_figure_t* const figure,
    sxbp_bitmap_t* const bitmap
);

/**
 * @brief Renders an image of the given figure, using the given render callback
 * @details The render callback should write out the bytes of the rendered image
 * to the given buffer.
 * @note The buffer is erased before the output is written to it
 * @param figure The SXBP figure to render
 * @param[out] buffer The buffer to write the image data out to
 * @param render_callback A callback function which can render figures out to
 * a specific image format, outputting the serialised image data to the given
 * buffer
 * @param render_options An optional pointer to options affecting the rendered
 * output
 * @param render_callback_options Optional type-agnostic pointer to additional
 * options that are specific to this particular render callback.
 * @warn No type-checking is done for `render_callback_options`, as the accepted
 * type (if any) is entirely dependent on the render callback being used. Care
 * must be taken to only pass a pointer to a type accepted by the used render
 * callback.
 * @returns `SXBP_RESULT_OK` if the figure could be rendered successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure`, `buffer` or
 * `render_callback` are `NULL`
 * @returns Any other valid value for type `sxbp_result_t`, according to all
 * the possible error codes that can be returned by the given render callback.
 * @since v0.54.0
 */
sxbp_result_t sxbp_render_figure(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    sxbp_figure_renderer_t render_callback,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
);

/**
 * @brief A dummy renderer function that does nothing
 * @details This exists for testing purposes only
 * @warn This function is currently unusable because it always returns the
 * "not implemented" failure error code
 * @returns `SXBP_RESULT_FAIL_UNIMPLEMENTED`
 * @since v0.54.0
 */
sxbp_result_t sxbp_render_figure_to_null(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
);

/**
 * @brief Renders figures to PBM images
 * @details If successful, the buffer will be filled with data which represents
 * a binary format PBM image (P4 format).
 * @returns `SXBP_RESULT_OK` if the figure could be rendered successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` or `buffer` are `NULL`
 * @returns `SXBP_RESULT_FAIL_MEMORY` if a memory allocation error occurred
 * @since v0.54.0
 */
sxbp_result_t sxbp_render_figure_to_pbm(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
);

/**
 * @brief Renders figures to SVG images
 * @details If successful, the buffer will be filled with data which represents
 * an SVG image.
 * @returns `SXBP_RESULT_OK` if the figure could be rendered successfully
 * @returns `SXBP_RESULT_FAIL_PRECONDITION` if `figure` or `buffer` are `NULL`
 * @returns `SXBP_RESULT_FAIL_MEMORY` if a memory allocation error occurred
 * @since v0.54.0
 */
sxbp_result_t sxbp_render_figure_to_svg(
    const sxbp_figure_t* const figure,
    sxbp_buffer_t* const buffer,
    const sxbp_render_options_t* const render_options,
    const void* render_callback_options
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
