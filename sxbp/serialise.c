/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) 2016, 2017, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "saxbospiral.h"
#include "serialise.h"


#ifdef __cplusplus
extern "C"{
#endif

// constants related to how spiral data is packed in files - measured in bytes
const size_t SXBP_FILE_HEADER_SIZE = (
    4 + // 'sxbp' file magic number
    6 + // file version, 3x 16-bit uints
    4 + // total number of lines, 32 bit uint
    4 + // number of lines solved, 32 bit uint
    4 + // number of seconds spent solving, 32 bit uint
    4 // number of seconds accuracy of solve time, 32 bit uint
);
const size_t SXBP_LINE_T_PACK_SIZE = 4;

/*
 * NOTE: The following load_x and dump_x functions all use big-endian
 * representation in their serialised forms.
 */

/*
 * loads a 16-bit unsigned integer from buffer starting at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint16_t load_uint16_t(sxbp_buffer_t* buffer, size_t start_index) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    return (
        ((uint16_t)buffer->bytes[start_index] << 8)
        + (buffer->bytes[start_index + 1])
    );
}

/*
 * dumps a 16-bit unsigned integer of value to buffer at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static void dump_uint16_t(
    uint16_t value, sxbp_buffer_t* buffer, size_t start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    buffer->bytes[start_index] = (uint8_t)(value >> 8);
    buffer->bytes[start_index + 1] = (uint8_t)(value % 256);
}

/*
 * loads a 32-bit unsigned integer from buffer starting at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint32_t load_uint32_t(sxbp_buffer_t* buffer, size_t start_index) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    uint32_t value = 0;
    for(uint8_t i = 0; i < 4; i++) {
        value |= (buffer->bytes[start_index + i]) << (8 * (3 - i));
    }
    return value;
}

/*
 * dumps a 32-bit unsigned integer of value to buffer at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static void dump_uint32_t(
    uint32_t value, sxbp_buffer_t* buffer, size_t start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    for(uint8_t i = 0; i < 4; i++) {
        uint8_t shift = (8 * (3 - i));
        buffer->bytes[start_index + i] = (uint8_t)(
            (value & (0xffUL << shift)) >> shift
        );
    }
}

sxbp_serialise_result_t sxbp_load_spiral(
    sxbp_buffer_t buffer, sxbp_spiral_t* spiral
) {
    // preconditional assertions
    assert(buffer.bytes != NULL);
    assert(spiral->lines == NULL);
    sxbp_serialise_result_t result; // build struct for returning success / failure
    // first, if header is too small for header + 1 line, then return early
    if(buffer.size < SXBP_FILE_HEADER_SIZE + SXBP_LINE_T_PACK_SIZE) {
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_HEADER_SIZE; // failure reason
        return result;
    }
    // check for magic number and return early if not right
    if(strncmp((char*)buffer.bytes, "sxbp", 4) != 0) {
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_MAGIC_NUMBER; // failure reason
        return result;
    }
    // grab file version from header
    sxbp_version_t buffer_version = {
        .major = load_uint16_t(&buffer, 4),
        .minor = load_uint16_t(&buffer, 6),
        .patch = load_uint16_t(&buffer, 8),
    };
    // we don't accept anything less than v0.25.0, so the min is v0.25.0
    sxbp_version_t min_version = { .major = 0, .minor = 25, .patch = 0, };
    // check for version compatibility
    if(sxbp_version_less_than(buffer_version, min_version)) {
        // check failed
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_VERSION; // failure reason
        return result;
    }
    // get size of spiral object contained in buffer
    uint32_t spiral_size = load_uint32_t(&buffer, 10);
    // Check that the file data section is large enough for the spiral size
    if((buffer.size - SXBP_FILE_HEADER_SIZE) != (SXBP_LINE_T_PACK_SIZE * spiral_size)) {
        // this check failed
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_DATA_SIZE; // failure reason
        return result;
    }
    // good to go
    // populate spiral struct, loading some more values
    spiral->size = spiral_size;
    spiral->solved_count = load_uint32_t(&buffer, 14);
    spiral->seconds_spent = load_uint32_t(&buffer, 18);
    // allocate memory
    spiral->lines = calloc(sizeof(sxbp_line_t), spiral->size);
    // catch allocation error
    if(spiral->lines == NULL) {
        result.status = SXBP_MALLOC_REFUSED; // flag failure
        return result;
    }
    // convert each serialised line segment in buffer into a line_t struct
    for(size_t i = 0; i < spiral_size; i++) {
        // direction is stored in 2 most significant bits of each 32-bit sequence
        spiral->lines[i].direction = (
            buffer.bytes[SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE)] >> 6
        );
        /*
         * length is stored as 30 least significant bits, so we have to unpack
         * it handle first byte on it's own as we only need least 6 bits of it
         * bit mask and shift 3 bytes to left
         */
        spiral->lines[i].length = (
            buffer.bytes[SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE)]
            & 0x3f // <= binary value is 0b00111111
        ) << 24;
        // handle remaining 3 bytes in loop
        for(uint8_t j = 0; j < 3; j++) {
            spiral->lines[i].length |= (
                buffer.bytes[SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE) + 1 + j]
            ) << (8 * (2 - j));
        }
    }
    // return ok status
    result.status = SXBP_OPERATION_OK;
    return result;
}

sxbp_serialise_result_t sxbp_dump_spiral(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer
) {
    // preconditional assertions
    assert(buffer->bytes == NULL);
    assert(spiral.lines != NULL);
    sxbp_serialise_result_t result; // build struct for returning success / failure
    // populate buffer struct, base size on header + spiral size
    buffer->size = (SXBP_FILE_HEADER_SIZE + (SXBP_LINE_T_PACK_SIZE * spiral.size));
    // allocate memory for buffer
    buffer->bytes = calloc(1, buffer->size);
    // catch memory allocation failure
    if(buffer->bytes == NULL) {
        result.status = SXBP_MALLOC_REFUSED;
        return result;
    }
    // write magic number to buffer
    sprintf((char*)buffer->bytes, "sxbp");
    // write out version info to buffer
    dump_uint16_t(LIB_SXBP_VERSION.major, buffer, 4);
    dump_uint16_t(LIB_SXBP_VERSION.minor, buffer, 6);
    dump_uint16_t(LIB_SXBP_VERSION.patch, buffer, 8);
    // write second part of data header
    dump_uint32_t(spiral.size, buffer, 10);
    dump_uint32_t(spiral.solved_count, buffer, 14);
    dump_uint32_t(spiral.seconds_spent, buffer, 18);
    // now write the data section
    for(size_t i = 0; i < spiral.size; i++) {
        /*
         * serialise each line in the spiral to 4 bytes, handle first byte first
         * map direction to 2 most significant bits
         */
        buffer->bytes[
            SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE)
        ] = (spiral.lines[i].direction << 6);
        // handle first 6 bits of the length
        buffer->bytes[
            SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE)
        ] |= (spiral.lines[i].length >> 24);
        // handle remaining 3 bytes in a loop
        for(uint8_t j = 0; j < 3; j++) {
            buffer->bytes[
                SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE) + 1 + j
            ] = (uint8_t)(spiral.lines[i].length >> (8 * (2 - j)));
        }
    }
    // return ok status
    result.status = SXBP_OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
