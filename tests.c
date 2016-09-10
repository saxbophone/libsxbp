#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral/saxbospiral.h"
#include "saxbospiral/initialise.h"
#include "saxbospiral/plot.h"
#include "saxbospiral/solve.h"
#include "saxbospiral/serialise.h"


bool
test_change_direction() {
    if(change_direction(UP, CLOCKWISE) != RIGHT) {
        return false;
    } else if(change_direction(UP, ANTI_CLOCKWISE) != LEFT) {
        return false;
    } else if(change_direction(LEFT, CLOCKWISE) != UP) {
        return false;
    } else if(change_direction(LEFT, ANTI_CLOCKWISE) != DOWN) {
        return false;
    } else if(change_direction(DOWN, CLOCKWISE) != LEFT) {
        return false;
    } else if(change_direction(RIGHT, ANTI_CLOCKWISE) != UP) {
        return false;
    } else {
        return true;
    }
}

bool
test_init_spiral() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    buffer_t buffer = { .size = 2, };
    buffer.bytes = malloc(buffer.size);
    buffer.bytes = (uint8_t[2]){ 0x6d, 0xc7, };
    // build expected output struct
    spiral_t expected = { .size = 17, };
    expected.lines = calloc(sizeof(line_t), 17);
    direction_t directions[17] = {
        UP, RIGHT, UP, LEFT, UP, LEFT, DOWN, LEFT,
        DOWN, RIGHT, UP, RIGHT, DOWN, LEFT, DOWN, RIGHT, UP
    };
    for(uint8_t i = 0; i < 17; i++) {
        expected.lines[i].direction = directions[i];
    }

    // call init_spiral with buffer and write to blank spiral
    spiral_t output;
    init_spiral(buffer, &output);

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

bool
test_spiral_points() {
    // success variable
    bool success = true;
    // prepare input spiral struct
    spiral_t input = {
        .size = 16,
        .lines = calloc(sizeof(line_t), 16),
    };
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(size_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // prepare expected output data
    co_ord_t expected[23] = {
        {  0,  0, }, {  0,  1, }, { -1,  1, }, { -1,  0, }, { -2,  0, },
        { -2, -1, }, { -1, -1, }, { -1, -2, }, {  0, -2, }, {  1, -2, },
        {  1, -1, }, {  1,  0, }, {  1,  1, }, {  1,  2, }, {  0,  2, },
        {  0,  3, }, {  1,  3, }, {  2,  3, }, {  2,  2, }, {  3,  2, },
        {  3,  3, }, {  3,  4, }, {  2,  4, }
    };

    // create struct for results
    co_ord_array_t results = {0};
    // call spiral_points on struct with start point and maximum limit
    spiral_points(input, &results, expected[0], 0, 16);

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

bool
test_cache_spiral_points_blank() {
    // success variable
    bool success = true;
    // prepare input spiral struct
    spiral_t input = {
        .size = 16,
        .lines = calloc(sizeof(line_t), 16),
    };
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(size_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // prepare expected output data
    co_ord_t expected[23] = {
        {  0,  0, }, {  0,  1, }, { -1,  1, }, { -1,  0, }, { -2,  0, },
        { -2, -1, }, { -1, -1, }, { -1, -2, }, {  0, -2, }, {  1, -2, },
        {  1, -1, }, {  1,  0, }, {  1,  1, }, {  1,  2, }, {  0,  2, },
        {  0,  3, }, {  1,  3, }, {  2,  3, }, {  2,  2, }, {  3,  2, },
        {  3,  3, }, {  3,  4, }, {  2,  4, }
    };

    // call spiral_points on struct with maximum limit
    cache_spiral_points(&input, 16);

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

bool
test_plot_spiral() {
    // success / failure variable
    bool result = true;
    // build input and output structs
    spiral_t spiral = { .size = 16, };
    spiral_t expected = { .size = 16, };
    spiral.lines = calloc(sizeof(line_t), 16);
    expected.lines = calloc(sizeof(line_t), 16);
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        spiral.lines[i].direction = directions[i];
        spiral.lines[i].length = 0;
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call plot_spiral on spiral
    plot_spiral(&spiral, 1);

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

bool
test_load_spiral() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    buffer_t buffer = { .size = 89, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    sprintf(
        (char *)buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch, 0, 0, 0, 0, 0, 0, 0, 16
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
        buffer.bytes[i+25] = data[i];
    }
    // build expected output struct
    spiral_t expected = { .size = 16, };
    expected.lines = calloc(sizeof(line_t), 16);
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call load_spiral with buffer and write to output spiral
    spiral_t output;
    load_spiral(buffer, &output);

    if(output.size != expected.size) {
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

bool
test_load_spiral_rejects_missing_magic_number() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    buffer_t buffer = { .size = 36, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    buffer.bytes = (uint8_t *)"not the header you were looking for";

    // call load_spiral with buffer and blank spiral, store result
    spiral_t output;
    serialise_result_t serialise_result = load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != OPERATION_FAIL) ||
        (serialise_result.diagnostic != DESERIALISE_BAD_MAGIC_NUMBER)
    ) {
        result = false;
    }

    return result;
}

bool
test_load_spiral_rejects_too_small_for_header() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data - should be smaller than 26
    buffer_t buffer = { .size = 12, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    buffer.bytes = (uint8_t *)"SAXBOSPIRAL";

    // call load_spiral with buffer and blank spiral, store result
    spiral_t output;
    serialise_result_t serialise_result = load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != OPERATION_FAIL) ||
        (serialise_result.diagnostic != DESERIALISE_BAD_HEADER_SIZE)
    ) {
        result = false;
    }

    return result;
}

bool
test_load_spiral_rejects_too_small_data_section() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    buffer_t buffer = { .size = 41, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    sprintf(
        (char *)buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch, 0, 0, 0, 0, 0, 0, 0, 16
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
    spiral_t output;
    serialise_result_t serialise_result = load_spiral(buffer, &output);

    if(
        (serialise_result.status.diagnostic != OPERATION_FAIL) ||
        (serialise_result.diagnostic != DESERIALISE_BAD_DATA_SIZE)
    ) {
        result = false;
    }

    return result;
}

bool
test_dump_spiral() {
    // success / failure variable
    bool result = true;
    // build input struct
    spiral_t input = { .size = 16, };
    input.lines = calloc(sizeof(line_t), 16);
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = lengths[i];
    }
    // build buffer of bytes for expected output data
    buffer_t expected = { .size = 89, };
    expected.bytes = calloc(1, expected.size);
    // construct expected data header
    sprintf(
        (char *)expected.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch, 0, 0, 0, 0, 0, 0, 0, 16
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
        expected.bytes[i+25] = data[i];
    }

    // call dump_spiral with spiral and write to output buffer
    buffer_t output;
    dump_spiral(input, &output);

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
bool
run_test_case(
    bool test_suite_state, bool (*test_case_func)(), char * test_case_name
) {
    printf("%s: ", test_case_name);
    fflush(stdout);
    bool test_result = (*test_case_func)();
    printf("%s\n", (test_result ? "PASS" : "FAIL"));
    return test_suite_state && test_result;
}

int
main(int argc, char const *argv[]) {
    // set up test suite status flag
    bool result = true;
    // call run_test_case() for each test case
    result = run_test_case(result, test_change_direction, "test_change_direction");
    result = run_test_case(result, test_init_spiral, "test_init_spiral");
    result = run_test_case(
        result, test_spiral_points, "test_spiral_points"
    );
    result = run_test_case(
        result, test_cache_spiral_points_blank, "test_cache_spiral_points_blank"
    );
    result = run_test_case(result, test_plot_spiral, "test_plot_spiral");
    result = run_test_case(result, test_load_spiral, "test_load_spiral");
    result = run_test_case(
        result, test_load_spiral_rejects_missing_magic_number,
        "test_load_spiral_rejects_missing_magic_number"
    );
    result = run_test_case(
        result, test_load_spiral_rejects_too_small_for_header,
        "test_load_spiral_rejects_too_small_for_header"
    );
    result = run_test_case(
        result, test_load_spiral_rejects_too_small_data_section,
        "test_load_spiral_rejects_too_small_data_section"
    );
    result = run_test_case(result, test_dump_spiral, "test_dump_spiral");
    return result ? 0 : 1;
}
