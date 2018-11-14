/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of functions for serialising
 * and deserialising figure objects to and from bytes.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

// NOTE: All unsigned integer serialisation uses big-endian ordering

// private, constants related to how spiral data is packed in files
static const size_t SXBP_FILE_HEADER_SIZE = (
    4 + // 'sxbp' file magic number
    6 + // file version, 3x 16-bit uints
    4 + // total number of lines, 32 bit uint
    4 + // number of lines solved, 32 bit uint
    // NOTE: all other fields after this point are unused as of v0.54.0
    4 + // number of seconds spent solving, 32 bit uint
    4 // number of seconds accuracy of solve time, 32 bit uint
);
static const size_t SXBP_LINE_T_PACK_SIZE = 4;

// private, returns the size in bytes needed to serialise a figure
static size_t sxbp_get_figure_serialised_size(const sxbp_figure_t* figure) {
    return SXBP_FILE_HEADER_SIZE + SXBP_LINE_T_PACK_SIZE * figure->size;
}

/*
 * private, dumps a 16-bit unsigned integer of value to buffer at given index
 * increments index to point to the next free byte after that used for the
 * integer
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static void sxbp_dump_uint16_t(
    uint16_t value, sxbp_buffer_t* buffer, size_t* start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    buffer->bytes[index] = (uint8_t)(value >> 8);
    buffer->bytes[index + 1] = (uint8_t)(value % 256);
    // increment index to point to next location
    *start_index += 2;
}

/*
 * private, dumps a 32-bit unsigned integer of value to buffer at given index
 * increments index to point to the next free byte after that used for the
 * integer
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static void sxbp_dump_uint32_t(
    uint32_t value, sxbp_buffer_t* buffer, size_t* start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    for(uint8_t i = 0; i < 4; i++) {
        uint8_t shift = (8 * (3 - i));
        buffer->bytes[index + i] = (uint8_t)(
            (value & (0xffUL << shift)) >> shift
        );
    }
    // increment index to point to next location
    *start_index += 4;
}

// private, writes the header of a serialised figure to a buffer
static void sxbp_write_sxbp_data_header(
    const sxbp_figure_t* figure,
    sxbp_buffer_t* buffer,
    size_t* index
) {
    // write the magic number
    const char* magic_number = "sxbp";
    memcpy(buffer->bytes, magic_number, 4);
    // advance the index
    *index += 4;
    // write the version number out
    sxbp_dump_uint16_t(SXBP_VERSION.major, buffer, index);
    sxbp_dump_uint16_t(SXBP_VERSION.minor, buffer, index);
    sxbp_dump_uint16_t(SXBP_VERSION.patch, buffer, index);
    // write out the number of lines in the figure
    sxbp_dump_uint32_t(figure->size, buffer, index);
    /*
     * NOTE: versions prior to v0.54.0 stored the number of lines solved so far
     * at this index in the file, however the new algorithm solves figures in
     * reverse so this is no longer valid. Instead, we store the largest 32-bit
     * uint here instead.
     */
    sxbp_dump_uint32_t(UINT32_MAX, buffer, index);
    // NOTE: similarly to the previous field, seconds_spent is currently unused
    sxbp_dump_uint32_t(UINT32_MAX, buffer, index);
    /*
     * NOTE: here, we've replaced the old field seconds_accuracy to store the
     * number of lines we have left to solve instead
     */
    sxbp_dump_uint32_t(figure->lines_remaining, buffer, index);
}

// private, writes one line in serialised form to a buffer
static void sxbp_write_sxbp_data_line(
    const sxbp_line_t line,
    sxbp_buffer_t* buffer,
    size_t* start_index
) {
    size_t index = *start_index;
    // the leading 2 bits of the first byte are used for the direction
    buffer->bytes[index] = line.direction << 6u;
    // the next 6 bits of the first byte are used for the first 6 bits of length
    buffer->bytes[index] |= (line.length >> 24);
    // move on to the next byte
    index++;
    // handle remaining 3 bytes in a loop
    for(uint8_t j = 0; j < 3; j++) {
        buffer->bytes[index] = (uint8_t)(line.length >> (8 * (2 - j)));
        index++;
    }
    // update index
    *start_index = index;
}

// private, writes the body of a serialised figure to a buffer
static void sxbp_write_sxbp_data_body(
    const sxbp_figure_t* figure,
    sxbp_buffer_t* buffer,
    size_t* index
) {
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        sxbp_write_sxbp_data_line(figure->lines[i], buffer, index);
    }
}

/*
 * private, loads a 16-bit unsigned integer from buffer starting at given index
 * increments index to point to the next byte after those read for the integer
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint16_t sxbp_load_uint16_t(
    const sxbp_buffer_t* buffer,
    size_t* start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    uint16_t value = (
        (uint16_t)((uint16_t)buffer->bytes[index] << 8) +
        buffer->bytes[index + 1]
    );
    // increment index to point to next location
    *start_index += 2;
    return value;
}

/*
 * private, loads a 32-bit unsigned integer from buffer starting at given index
 * increments index to point to the next byte after those read for the integer
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint32_t sxbp_load_uint32_t(
    const sxbp_buffer_t* buffer,
    size_t* start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    uint32_t value = 0;
    for(uint8_t i = 0; i < 4; i++) {
        value |= (uint8_t)((buffer->bytes[index + i]) << (8 * (3 - i)));
    }
    // increment index to point to next location
    *start_index += 4;
    return value;
}

// private, checks that the version of the data in the buffer is compatible
static bool sxbp_check_sxbp_data_version(const sxbp_buffer_t* buffer) {
    // the version number is encoded starting at index 4 into the file
    size_t index = 4;
    // extract file version from header
    sxbp_version_t buffer_version = {
        .major = sxbp_load_uint16_t(buffer, &index),
        .minor = sxbp_load_uint16_t(buffer, &index),
        .patch = sxbp_load_uint16_t(buffer, &index),
    };
    // this is the minimum version supported by this version of sxbp
    sxbp_version_t min_version = { .major = 0, .minor = 54, .patch = 0, };
    // compare the extracted version with the minimum one
    return (
        buffer_version.major == min_version.major &&
        buffer_version.minor >= min_version.minor
    );
}

// private, checks if the data in given buffer is a valid serialised sxbp figure
static bool sxbp_check_sxbp_data_is_valid(const sxbp_buffer_t* buffer) {
    /*
     * first, check if the file is big enough to contain the header
     * if it is, then check the magic number
     * finally, if both of those are valid, theh check the data version
     */
    if (
        buffer->size < SXBP_FILE_HEADER_SIZE ||
        strncmp((char*)buffer->bytes, "sxbp", 4) != 0 ||
        !sxbp_check_sxbp_data_version(buffer)
    ) {
        return false;
    } else {
        return true;
    }
}

// private, extracts one line from a data buffer to a line object
static void sxbp_read_sxbp_data_line(
    const sxbp_buffer_t* buffer,
    sxbp_line_t* line,
    size_t* start_index
) {
    size_t index = *start_index;
    // direction is stored in 2 most significant bits of each 32-bit sequence
    line->direction = (
        buffer->bytes[index] >> 6
    );
    /*
     * length is stored as 30 least significant bits, so we have to unpack it
     * handle first byte on it's own as we only need least 6 bits of it
     * bit mask and shift 3 bytes to left
     */
    line->length = (buffer->bytes[index] & 0x3fU) << 24; // 0x3f = 0b00111111
    // next byte
    index++;
    // handle remaining 3 bytes in loop
    for(uint8_t j = 0; j < 3; j++) {
        line->length |= buffer->bytes[index] << (8 * (2 - j));
        index++;
    }
    // update index
    *start_index = index;
}

// private, reads in serialised figure lines from a buffer to a figure
static void sxbp_read_sxbp_data_body(
    const sxbp_buffer_t* buffer,
    sxbp_figure_t* figure,
    size_t* index
) {
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        sxbp_read_sxbp_data_line(buffer, &figure->lines[i], index);
    }
}

sxbp_result_t sxbp_dump_figure(
    const sxbp_figure_t* figure,
    sxbp_buffer_t* buffer
) {
    // figure and buffer must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(figure);
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    // erase the buffer first of all just in case
    sxbp_free_buffer(buffer);
    // set buffer size to that needed for figure
    buffer->size = sxbp_get_figure_serialised_size(figure);
    // try and allocate memory for the buffer
    if (!sxbp_success(sxbp_init_buffer(buffer))) {
        // handle error - this can only be a memory error
        return SXBP_RESULT_FAIL_MEMORY;
    } else {
        // initialise an index used to assist writing to the buffer
        size_t index = 0;
        // now write the header to the buffer
        sxbp_write_sxbp_data_header(figure, buffer, &index);
        // now write the file body -serialisation of the lines - to the buffer
        sxbp_write_sxbp_data_body(figure, buffer, &index);
        // all done!
        return SXBP_RESULT_OK;
    }
}

sxbp_result_t sxbp_load_figure(
    const sxbp_buffer_t* buffer,
    sxbp_figure_t* figure
) {
    // buffer and figure must not be NULL
    SXBP_RETURN_FAIL_IF_NULL(buffer);
    SXBP_RETURN_FAIL_IF_NULL(figure);
    // erase the figure first of all just in case
    sxbp_free_figure(figure);
    // check that the buffer contains valid sxbp data
    if (!sxbp_check_sxbp_data_is_valid(buffer)) {
        // exit early as it's not valid
        return SXBP_RESULT_FAIL_PRECONDITION;
    } else {
        // the data body starts at index 10
        size_t index = 10;
        /*
         * figure is apparently valid, but to be sure we finally need to extract
         * the size field and compare this with the size of the buffer
         */
        figure->size = sxbp_load_uint32_t(buffer, &index);
        if (buffer->size < sxbp_get_figure_serialised_size(figure)) {
            // return early and signal error
            return false;
        } else {
            // skip the next two 32-bit uints
            index += 4 * 2;
            // extract the lines_remaining field
            figure->lines_remaining = sxbp_load_uint32_t(buffer, &index);
            // allocate memory for the lines
            if (!sxbp_success(sxbp_init_figure(figure))) {
                // handle error - this can only be a memory error
                return SXBP_RESULT_FAIL_MEMORY;
            } else {
                // finally, load all the lines
                sxbp_read_sxbp_data_body(buffer, figure, &index);
                return SXBP_RESULT_OK;
            }
        }
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
