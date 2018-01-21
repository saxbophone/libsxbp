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
    size_t* index
) {
    size_t start_index = *index;
    // the leading 2 bits of the first byte are used for the direction
    buffer->bytes[start_index] = (line.direction << 6);
    // the next 6 bits of the first byte are used for the first 6 bits of length
    buffer->bytes[start_index] |= (line.length >> 24);
    // move on to the next byte
    start_index++;
    // handle remaining 3 bytes in a loop
    for(uint8_t j = 0; j < 3; j++) {
        buffer->bytes[start_index] = (uint8_t)(line.length >> (8 * (2 - j)));
        start_index++;
    }
    // update index
    *index = start_index;
}

// private, writes the body of a serialised figure to a buffer
static void sxbp_write_sxbp_data_body(
    const sxbp_figure_t* figure,
    sxbp_buffer_t* buffer,
    size_t* index
) {
    for (uint32_t i = 0; i < figure->size; i++) {
        sxbp_write_sxbp_data_line(figure->lines[i], buffer, index);
    }
}

#pragma GCC diagnostic pop
#pragma GCC diagnostic ignored "-Wunused-function"
/*
 * private, loads a 16-bit unsigned integer from buffer starting at given index
 * increments index to point to the next byte after those read for the integer
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint16_t sxbp_load_uint16_t(sxbp_buffer_t* buffer, size_t* start_index) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    uint16_t value = (
        ((uint16_t)buffer->bytes[index] << 8) + buffer->bytes[index + 1]
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
static uint32_t sxbp_load_uint32_t(sxbp_buffer_t* buffer, size_t* start_index) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    size_t index = *start_index;
    // extract the value
    uint32_t value = 0;
    for(uint8_t i = 0; i < 4; i++) {
        value |= (buffer->bytes[index + i]) << (8 * (3 - i));
    }
    // increment index to point to next location
    *start_index += 4;
    return value;
}
#pragma GCC diagnostic push

bool sxbp_dump_figure(const sxbp_figure_t* figure, sxbp_buffer_t* buffer) {
    // erase the buffer first of all just in case
    sxbp_free_buffer(buffer);
    // set buffer size to that needed for figure
    buffer->size = sxbp_get_figure_serialised_size(figure);
    // try and allocate memory for the buffer
    if (!sxbp_init_buffer(buffer)) {
        // handle error
        return false;
    } else {
        // initialise an index used to assist writing to the buffer
        size_t index = 0;
        // now write the header to the buffer
        sxbp_write_sxbp_data_header(figure, buffer, &index);
        // now write the file body -serialisation of the lines - to the buffer
        sxbp_write_sxbp_data_body(figure, buffer, &index);
        // all done!
        return true;
    }
}

bool sxbp_load_figure(const sxbp_buffer_t* buffer, sxbp_figure_t* figure);

#ifdef __cplusplus
} // extern "C"
#endif
