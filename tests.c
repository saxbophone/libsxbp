/*
 * This source file consists of the unit tests for all of libsaxbospiral, a
 * library which generates experimental 2D spiral-like shapes based on input
 * binary data.
 *
 * This compilation unit is omitted from the resulting library object, and is
 * built into a binary which is linked against the library object. The test
 * binary is not included among the install candidates.
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral/saxbospiral.h"
#include "saxbospiral/initialise.h"
#include "saxbospiral/plot.h"
#include "saxbospiral/solve.h"
#include "saxbospiral/serialise.h"


bool test_sxbp_change_direction() {
    if(sxbp_change_direction(SXBP_UP, SXBP_CLOCKWISE) != SXBP_RIGHT) {
        return false;
    } else if(sxbp_change_direction(SXBP_UP, SXBP_ANTI_CLOCKWISE) != SXBP_LEFT) {
        return false;
    } else if(sxbp_change_direction(SXBP_LEFT, SXBP_CLOCKWISE) != SXBP_UP) {
        return false;
    } else if(sxbp_change_direction(SXBP_LEFT, SXBP_ANTI_CLOCKWISE) != SXBP_DOWN) {
        return false;
    } else if(sxbp_change_direction(SXBP_DOWN, SXBP_CLOCKWISE) != SXBP_LEFT) {
        return false;
    } else if(sxbp_change_direction(SXBP_RIGHT, SXBP_ANTI_CLOCKWISE) != SXBP_UP) {
        return false;
    } else {
        return true;
    }
}

bool test_sxbp_init_spiral() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    sxbp_buffer_t buffer = { .size = 2, };
    buffer.bytes = malloc(buffer.size);
    buffer.bytes = (uint8_t[2]){ 0x6d, 0xc7, };
    // build expected output struct
    sxbp_spiral_t expected = { .size = 17, };
    expected.lines = calloc(sizeof(sxbp_line_t), 17);
    sxbp_direction_t directions[17] = {
        SXBP_UP, SXBP_RIGHT, SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_LEFT, SXBP_DOWN,
        SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN,
        SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP
    };
    for(uint8_t i = 0; i < 17; i++) {
        expected.lines[i].direction = directions[i];
    }

    // call init_spiral with buffer and write to blank spiral
    sxbp_spiral_t output = sxbp_blank_spiral();
    sxbp_init_spiral(buffer, &output);

    if(output.size != expected.size) {
        result = false;
    }
    // compare with expected struct
    for(uint8_t i = 0; i < 17; i++) {
        if(output.lines[i].direction != expected.lines[i].direction) {
            printf(
                "%u: %i != %i\n",
                i,
                output.lines[i].direction,
                expected.lines[i].direction
            );
            result = false;
        }
    }

    // free memory
    free(output.lines);
    free(expected.lines);

    return result;
}

bool test_sxbp_spiral_points() {
    // success variable
    bool success = true;
    // prepare input spiral struct
    sxbp_spiral_t input = {
        .size = 16,
        .lines = calloc(sizeof(sxbp_line_t), 16),
    };
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(size_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // prepare expected output data
    sxbp_co_ord_t expected[23] = {
        {  0,  0, }, {  0,  1, }, { -1,  1, }, { -1,  0, }, { -2,  0, },
        { -2, -1, }, { -1, -1, }, { -1, -2, }, {  0, -2, }, {  1, -2, },
        {  1, -1, }, {  1,  0, }, {  1,  1, }, {  1,  2, }, {  0,  2, },
        {  0,  3, }, {  1,  3, }, {  2,  3, }, {  2,  2, }, {  3,  2, },
        {  3,  3, }, {  3,  4, }, {  2,  4, }
    };

    // create struct for results
    sxbp_co_ord_array_t results = {0, 0};
    // call spiral_points on struct with start point and maximum limit
    sxbp_spiral_points(input, &results, expected[0], 0, 16);

    // validate data
    if(results.size != 23) {
        success = false;
    } else {
        for(size_t i = 0; i < 23; i++) {
            if(
                (results.items[i].x != expected[i].x)
                || (results.items[i].y != expected[i].y)
            ) {
                success = false;
                // break;
            }
        }
    }

    // clean up
    free(input.lines);
    free(results.items);
    return success;
}

bool test_sxbp_cache_spiral_points_blank() {
    // success variable
    bool success = true;
    // prepare input spiral struct
    sxbp_spiral_t input = {
        .size = 16,
        .lines = calloc(sizeof(sxbp_line_t), 16),
    };
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(size_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // prepare expected output data
    sxbp_co_ord_t expected[23] = {
        {  0,  0, }, {  0,  1, }, { -1,  1, }, { -1,  0, }, { -2,  0, },
        { -2, -1, }, { -1, -1, }, { -1, -2, }, {  0, -2, }, {  1, -2, },
        {  1, -1, }, {  1,  0, }, {  1,  1, }, {  1,  2, }, {  0,  2, },
        {  0,  3, }, {  1,  3, }, {  2,  3, }, {  2,  2, }, {  3,  2, },
        {  3,  3, }, {  3,  4, }, {  2,  4, }
    };

    // call spiral_points on struct with maximum limit
    sxbp_cache_spiral_points(&input, 16);

    // validate data
    if(input.co_ord_cache.co_ords.size != 23) {
        success = false;
    } else if(input.co_ord_cache.validity != 16) {
        success = false;
    } else {
        for(size_t i = 0; i < 23; i++) {
            if(
                (input.co_ord_cache.co_ords.items[i].x != expected[i].x)
                || (input.co_ord_cache.co_ords.items[i].y != expected[i].y)
            ) {
                success = false;
                // break;
            }
        }
    }

    // clean up
    free(input.lines);
    free(input.co_ord_cache.co_ords.items);
    return success;
}

bool test_sxbp_plot_spiral() {
    // success / failure variable
    bool result = true;
    // build input and output structs
    sxbp_spiral_t spiral = { .size = 16, };
    sxbp_spiral_t expected = { .size = 16, .solved_count = 16, };
    spiral.lines = calloc(sizeof(sxbp_line_t), 16);
    expected.lines = calloc(sizeof(sxbp_line_t), 16);
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        spiral.lines[i].direction = directions[i];
        spiral.lines[i].length = 0;
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call plot_spiral on spiral
    sxbp_plot_spiral(&spiral, 1, 16, NULL, NULL);

    // check solved count
    if(spiral.solved_count != expected.solved_count) {
        result = false;
    }
    // compare with expected struct
    for(uint8_t i = 0; i < 16; i++) {
        if(spiral.lines[i].length != expected.lines[i].length) {
            printf("%i != %i\n", spiral.lines[i].length, expected.lines[i].length);
            result = false;
        }
    }

    // free memory
    free(spiral.lines);
    free(expected.lines);

    return result;
}

bool test_sxbp_plot_spiral_partial() {
    // success / failure variable
    bool result = true;
    // build input and output structs
    sxbp_spiral_t spiral = { .size = 16, };
    sxbp_spiral_t expected = { .size = 16, .solved_count = 9, };
    spiral.lines = calloc(sizeof(sxbp_line_t), 16);
    expected.lines = calloc(sizeof(sxbp_line_t), 16);
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    for(uint8_t i = 0; i < 9; i++) {
        spiral.lines[i].direction = directions[i];
        spiral.lines[i].length = 0;
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call plot_spiral on spiral, with instruction to only plot up to line 9
    sxbp_plot_spiral(&spiral, 1, 9, NULL, NULL);

    // check solved count
    if(spiral.solved_count != expected.solved_count) {
        result = false;
    }
    // compare with expected struct
    for(uint8_t i = 0; i < 9; i++) {
        if(spiral.lines[i].length != expected.lines[i].length) {
            printf("%i != %i\n", spiral.lines[i].length, expected.lines[i].length);
            result = false;
        }
    }

    // free memory
    free(spiral.lines);
    free(expected.lines);

    return result;
}

/*
 * disable GCC warning about the unused parameters as this function by necessity
 * requires these arguments in its signature, but it needn't use all of them.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// test callback for next test case
static void test_progress_callback(
    sxbp_spiral_t* spiral, uint64_t latest_line, uint64_t target_line,
    void* progress_callback_user_data
) {
    // cast user data from void pointer to uint16_t pointer, deref and multiply
    *(uint16_t*)progress_callback_user_data *= 13;
}
// re-enable all warnings
#pragma GCC diagnostic pop

bool test_sxbp_plot_spiral_progress_callback() {
    // success / failure variable
    bool result = true;
    // build input structs
    sxbp_spiral_t spiral = { .size = 16, };
    spiral.lines = calloc(sizeof(sxbp_line_t), 16);
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT,
        SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    for(uint8_t i = 0; i < 16; i++) {
        spiral.lines[i].direction = directions[i];
        spiral.lines[i].length = 0;
    }
    // create user data variable
    uint16_t user_data = 17;

    /*
     * call plot_spiral on spiral, with instruction to only plot up to line 1
     * supply our progress_callback function and a user data variable
     */
    sxbp_plot_spiral(&spiral, 1, 1, test_progress_callback, (void*)&user_data);

    // user data variable should have been set to 221 (17 * 13) by callback
    if(user_data != 221) {
        result = false;
    }

    // free memory
    free(spiral.lines);

    return result;
}

bool test_sxbp_load_spiral() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    sxbp_buffer_t buffer = { .size = 101, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    sprintf(
        (char*)buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        LIB_SXBP_VERSION.major, LIB_SXBP_VERSION.minor, LIB_SXBP_VERSION.patch,
        0, 0, 0, 0, 0, 0, 0, 16, // size serialised as 64-bit
        0, 0, 0, 0, 0, 0, 0, 5, // solved_count serialised as 64-bit
        0, 0, 12, 53 // seconds_spent serialised as 32-bit
    );
    // construct data section - each line of the array is one line of the spiral
    uint8_t data[64] = {
        0x00, 0x00, 0x00, 0x01,
        0xc0, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0xc0, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x04,
        0xc0, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x02,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x02,
        0xc0, 0x00, 0x00, 0x01,
    };
    // write data to buffer
    for(size_t i = 0; i < 64; i++) {
        buffer.bytes[i + SXBP_FILE_HEADER_SIZE] = data[i];
    }
    // build expected output struct
    sxbp_spiral_t expected = {
        .size = 16, .solved_count = 5, .seconds_spent = 3125,
    };
    expected.lines = calloc(sizeof(sxbp_line_t), 16);
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call load_spiral with buffer and write to output spiral
    sxbp_spiral_t output;
    sxbp_load_spiral(buffer, &output);

    if(output.size != expected.size) {
        result = false;
    } else if(output.solved_count != expected.solved_count) {
        result = false;
    } else if(output.seconds_spent != expected.seconds_spent) {
        result = false;
    } else {
        // compare with expected struct
        for(uint8_t i = 0; i < 16; i++) {
            if(
                (output.lines[i].direction != expected.lines[i].direction) ||
                (output.lines[i].length != expected.lines[i].length)
            ) {
                result = false;
            }
        }
    }

    // free memory
    free(output.lines);
    free(expected.lines);

    return result;
}

bool test_sxbp_load_spiral_rejects_missing_magic_number() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    sxbp_buffer_t buffer = { .size = 59, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    buffer.bytes = (uint8_t*)(
        "not the header you were looking for eh? I think not surely?"
    );

    // call load_spiral with buffer and blank spiral, store result
    sxbp_spiral_t output = sxbp_blank_spiral();
    sxbp_serialise_result_t serialise_result = sxbp_load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != SXBP_OPERATION_FAIL) ||
        (serialise_result.diagnostic != SXBP_DESERIALISE_BAD_MAGIC_NUMBER)
    ) {
        result = false;
    }

    return result;
}

bool test_sxbp_load_spiral_rejects_too_small_for_header() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data - should be smaller than 26
    sxbp_buffer_t buffer = { .size = 12, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    buffer.bytes = (uint8_t*)"SAXBOSPIRAL";

    // call load_spiral with buffer and blank spiral, store result
    sxbp_spiral_t output = sxbp_blank_spiral();
    sxbp_serialise_result_t serialise_result = sxbp_load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != SXBP_OPERATION_FAIL) ||
        (serialise_result.diagnostic != SXBP_DESERIALISE_BAD_HEADER_SIZE)
    ) {
        result = false;
    }

    return result;
}

bool test_sxbp_load_spiral_rejects_too_small_data_section() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    sxbp_buffer_t buffer = { .size = 0xffffffffffffffffu, }; // max 64-bit uint
    buffer.bytes = calloc(1, 41); // set allocation size to actual data size
    // construct data header
    sprintf(
        (char*)buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        LIB_SXBP_VERSION.major, LIB_SXBP_VERSION.minor, LIB_SXBP_VERSION.patch,
        0, 0, 0, 0, 0, 0, 0, 16, // size serialised as 64-bit
        0, 0, 0, 0, 0, 0, 0, 5, // solved_count serialised as 64-bit
        0, 0, 12, 53 // seconds_spent serialised as 32-bit
    );
    // construct data section - make it deliberately too short
    uint8_t data[16] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    // write data to buffer
    for(size_t i = 0; i < 16; i++) {
        buffer.bytes[i+25] = data[i];
    }
    // call load_spiral with buffer and blank spiral, store result
    sxbp_spiral_t output = sxbp_blank_spiral();
    sxbp_serialise_result_t serialise_result = sxbp_load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != SXBP_OPERATION_FAIL) ||
        (serialise_result.diagnostic != SXBP_DESERIALISE_BAD_DATA_SIZE)
    ) {
        result = false;
    }

    return result;
}

bool test_sxbp_load_spiral_rejects_wrong_version() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    sxbp_buffer_t buffer = { .size = 41, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    sprintf(
        (char*)buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        0, 12, 255,
        0, 0, 0, 0, 0, 0, 0, 16, // size serialised as 64-bit
        0, 0, 0, 0, 0, 0, 0, 5, // solved_count serialised as 64-bit
        0, 0, 12, 53 // seconds_spent serialised as 32-bit
    );
    // construct data section - make it deliberately too short
    uint8_t data[16] = {
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    // write data to buffer
    for(size_t i = 0; i < 16; i++) {
        buffer.bytes[i+25] = data[i];
    }
    // call load_spiral with buffer and blank spiral, store result
    sxbp_spiral_t output = sxbp_blank_spiral();
    sxbp_serialise_result_t serialise_result = sxbp_load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != SXBP_OPERATION_FAIL) ||
        (serialise_result.diagnostic != SXBP_DESERIALISE_BAD_VERSION)
    ) {
        result = false;
    }

    return result;
}

bool test_sxbp_dump_spiral() {
    // success / failure variable
    bool result = true;
    // build input struct
    sxbp_spiral_t input = {
        .size = 16,
        .solved_count = 5,
        .seconds_spent = 3125,
    };
    input.lines = calloc(sizeof(sxbp_line_t), 16);
    sxbp_direction_t directions[16] = {
        SXBP_UP, SXBP_LEFT, SXBP_DOWN, SXBP_LEFT, SXBP_DOWN, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT,
        SXBP_UP, SXBP_LEFT, SXBP_UP, SXBP_RIGHT, SXBP_DOWN, SXBP_RIGHT, SXBP_UP, SXBP_LEFT,
    };
    sxbp_length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // build buffer of bytes for expected output data
    sxbp_buffer_t expected = { .size = 101, };
    expected.bytes = calloc(1, expected.size);
    // construct expected data header
    sprintf(
        (char*)expected.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        LIB_SXBP_VERSION.major, LIB_SXBP_VERSION.minor, LIB_SXBP_VERSION.patch,
        0, 0, 0, 0, 0, 0, 0, 16, // size serialised as 64-bit
        0, 0, 0, 0, 0, 0, 0, 5, // solved_count serialised as 64-bit
        0, 0, 12, 53 // seconds_spent serialised as 32-bit
    );
    // construct expected data section
    uint8_t data[64] = {
        0x00, 0x00, 0x00, 0x01,
        0xc0, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0xc0, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x01,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x04,
        0xc0, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x02,
        0x80, 0x00, 0x00, 0x01,
        0x40, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x02,
        0xc0, 0x00, 0x00, 0x01,
    };
    // write data to expected buffer
    for(size_t i = 0; i < 64; i++) {
        expected.bytes[i + SXBP_FILE_HEADER_SIZE] = data[i];
    }

    // call dump_spiral with spiral and write to output buffer
    sxbp_buffer_t output = { .size = 0, .bytes = NULL, };
    sxbp_dump_spiral(input, &output);

    if(output.size != expected.size) {
        result = false;
    } else {
        // compare with expected buffer
        for(size_t i = 0; i < expected.size; i++) {
            if(output.bytes[i] != expected.bytes[i]) {
                result = false;
            }
        }
    }

    // free memory
    free(input.lines);
    free(expected.bytes);
    free(output.bytes);

    return result;
}

// this function takes a bool containing the test suite status,
// a function pointer to a test case function, and a string containing the
// test case's name. it will run the test case function and return the success
// or failure status, which should be stored in the test suite status bool.
bool run_test_case(
    bool test_suite_state, bool (*test_case_func)(), char* test_case_name
) {
    printf("%s: ", test_case_name);
    fflush(stdout);
    bool test_result = (*test_case_func)();
    printf("%s\n", (test_result ? "PASS" : "FAIL"));
    return test_suite_state && test_result;
}

int main() {
    // set up test suite status flag
    bool result = true;
    // call run_test_case() for each test case
    result = run_test_case(
        result, test_sxbp_change_direction, "test_sxbp_change_direction"
    );
    result = run_test_case(
        result, test_sxbp_init_spiral, "test_sxbp_init_spiral"
    );
    result = run_test_case(
        result, test_sxbp_spiral_points, "test_sxbp_spiral_points"
    );
    result = run_test_case(
        result, test_sxbp_cache_spiral_points_blank,
        "test_sxbp_cache_spiral_points_blank"
    );
    result = run_test_case(
        result, test_sxbp_plot_spiral, "test_sxbp_plot_spiral"
    );
    result = run_test_case(
        result, test_sxbp_plot_spiral_partial, "test_sxbp_plot_spiral_partial"
    );
    result = run_test_case(
        result, test_sxbp_plot_spiral_progress_callback,
        "test_sxbp_plot_spiral_progress_callback"
    );
    result = run_test_case(
        result, test_sxbp_load_spiral, "test_sxbp_load_spiral"
    );
    result = run_test_case(
        result, test_sxbp_load_spiral_rejects_missing_magic_number,
        "test_sxbp_load_spiral_rejects_missing_magic_number"
    );
    result = run_test_case(
        result, test_sxbp_load_spiral_rejects_too_small_for_header,
        "test_sxbp_load_spiral_rejects_too_small_for_header"
    );
    result = run_test_case(
        result, test_sxbp_load_spiral_rejects_too_small_data_section,
        "test_sxbp_load_spiral_rejects_too_small_data_section"
    );
    result = run_test_case(
        result, test_sxbp_load_spiral_rejects_wrong_version,
        "test_sxbp_load_spiral_rejects_wrong_version"
    );
    result = run_test_case(
        result, test_sxbp_dump_spiral, "test_sxbp_dump_spiral"
    );
    return result ? 0 : 1;
}
