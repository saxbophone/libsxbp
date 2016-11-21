/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for serialising spirals to and from
 * byte sequences (which may be stored in files or any other form).
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License (version 3),
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
const size_t SXBP_FILE_HEADER_SIZE = 37;
const size_t SXBP_LINE_T_PACK_SIZE = 4;

/*
 * loads a 64-bit unsigned integer from buffer starting at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static uint64_t load_uint64_t(sxbp_buffer_t* buffer, size_t start_index) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    uint64_t value = 0;
    for(size_t i = 0; i < 8; i++) {
        value |= (buffer->bytes[start_index + i]) << (8 * (7 - i));
    }
    return value;
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
    for(size_t i = 0; i < 4; i++) {
        value |= (buffer->bytes[start_index + i]) << (8 * (3 - i));
    }
    return value;
}

/*
 * dumps a 64-bit unsigned integer of value to buffer at given index
 *
 * Asserts:
 * - That buffer->bytes is not NULL
 */
static void dump_uint64_t(
    uint64_t value, sxbp_buffer_t* buffer, size_t start_index
) {
    // preconditional assertions
    assert(buffer->bytes != NULL);
    for(size_t i = 0; i < 8; i++) {
        size_t shift = (8 * (7 - i));
        buffer->bytes[start_index + i] = (uint8_t)(
            (value & (0xffUL << shift)) >> shift
        );
    }
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
    for(size_t i = 0; i < 4; i++) {
        size_t shift = (8 * (3 - i));
        buffer->bytes[start_index + i] = (uint8_t)(
            (value & (0xffUL << shift)) >> shift
        );
    }
}

/*
 * given a buffer and a pointer to a blank spiral_t, create a spiral represented
 * by the data in the struct and write to the spiral
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 *
 * Asserts:
 * - That buffer.bytes is not NULL
 * - That spiral->lines is NULL
 */
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
    if(strncmp((char*)buffer.bytes, "SAXBOSPIRAL", 11) != 0) {
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_MAGIC_NUMBER; // failure reason
        return result;
    }
    // grab file version from header
    sxbp_version_t buffer_version = {
        .major = buffer.bytes[12],
        .minor = buffer.bytes[13],
        .patch = buffer.bytes[14],
    };
    // we don't accept anything less than v0.13.0, so the min is v0.13.0
    sxbp_version_t min_version = { .major = 0, .minor = 13, .patch = 0, };
    // check for version compatibility
    if(sxbp_version_hash(buffer_version) < sxbp_version_hash(min_version)) {
        // check failed
        result.status = SXBP_OPERATION_FAIL; // flag failure
        result.diagnostic = SXBP_DESERIALISE_BAD_VERSION; // failure reason
        return result;
    }
    // get size of spiral object contained in buffer
    uint64_t spiral_size = load_uint64_t(&buffer, 16);
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
    spiral->solved_count = load_uint64_t(&buffer, 24);
    spiral->seconds_spent = load_uint32_t(&buffer, 32);
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
        for(size_t j = 0; j < 3; j++) {
            spiral->lines[i].length |= (
                buffer.bytes[SXBP_FILE_HEADER_SIZE + (i * SXBP_LINE_T_PACK_SIZE) + 1 + j]
            ) << (8 * (2 - j));
        }
    }
    // return ok status
    result.status = SXBP_OPERATION_OK;
    return result;
}

/*
 * given a spiral_t struct and a pointer to a blank buffer_t, serialise a binary
 * representation of the spiral and write this to the data buffer pointer
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 *
 * Asserts:
 * - That spiral.lines is not NULL
 * - That buffer->bytes is NULL
 */
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
    // write first part of data header (magic number and version info)
    sprintf(
        (char*)buffer->bytes, "SAXBOSPIRAL\n%c%c%c\n",
        LIB_SXBP_VERSION.major, LIB_SXBP_VERSION.minor, LIB_SXBP_VERSION.patch
    );
    // write second part of data header (size, solved count and seconds fields)
    dump_uint64_t(spiral.size, buffer, 16);
    dump_uint64_t(spiral.solved_count, buffer, 24);
    dump_uint32_t(spiral.seconds_spent, buffer, 32);
    // write final newline at end of header
    buffer->bytes[SXBP_FILE_HEADER_SIZE - 1] = '\n';
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
        for(size_t j = 0; j < 3; j++) {
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
