#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral.h"


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
    buffer.bytes = (uint8_t[2]){ 0b01101101, 0b11000111, };
    // build expected output struct
    spiral_t expected = { .size = 16, };
    expected.lines = calloc(sizeof(line_t), 16);
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    for(uint8_t i = 0; i < 16; i++) {
        expected.lines[i].direction = directions[i];
    }

    // call init_spiral with buffer and store result
    spiral_t output = init_spiral(buffer);

    // compare with expected struct
    for(uint8_t i = 0; i < 16; i++) {
        if(output.lines[i].direction != expected.lines[i].direction) {
            result = false;
        }
    }

    // free memory
    free(output.lines);
    free(expected.lines);

    return result;
}

bool
test_plot_spiral() {
    // success / failure variable
    bool result = true;
    // build input and output structs
    spiral_t input = { .size = 16, };
    spiral_t expected = { .size = 16, };
    input.lines = calloc(sizeof(line_t), 16);
    expected.lines = calloc(sizeof(line_t), 16);
    direction_t directions[16] = {
        UP, LEFT, DOWN, LEFT, DOWN, RIGHT, DOWN, RIGHT,
        UP, LEFT, UP, RIGHT, DOWN, RIGHT, UP, LEFT,
    };
    length_t lengths[16] = {
        1, 1, 1, 1, 1, 1, 1, 2, 4, 1, 1, 2, 1, 1, 2, 1,
    };
    for(uint8_t i = 0; i < 16; i++) {
        input.lines[i].direction = directions[i];
        input.lines[i].length = 1;
        expected.lines[i].direction = directions[i];
        expected.lines[i].length = lengths[i];
    }

    // call init_spiral with buffer and store result
    spiral_t output = plot_spiral(input);

    // compare with expected struct
    for(uint8_t i = 0; i < 16; i++) {
        if(output.lines[i].length != expected.lines[i].length) {
            result = false;
        }
    }

    // free memory
    free(input.lines);
    free(output.lines);
    free(expected.lines);

    return result;
}

bool
test_load_spiral() {
    // success / failure variable
    bool result = true;
    // build buffer of bytes for input data
    buffer_t buffer = { .size = 88, };
    buffer.bytes = calloc(1, buffer.size);
    // construct data header
    sprintf(
        buffer.bytes,
        "SAXBOSPIRAL\n%c%c%c\n%c%c%c%c%c%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch, 0, 0, 0, 0, 0, 0, 0, 16
    );
    // construct data section
    uint8_t data[64] = {
        0b00000000, 0b00000000, 0b00000000, 0b00000001,
        0b11000000, 0b00000000, 0b00000000, 0b00000001,
        0b10000000, 0b00000000, 0b00000000, 0b00000001,
        0b11000000, 0b00000000, 0b00000000, 0b00000001,
        0b10000000, 0b00000000, 0b00000000, 0b00000001,
        0b01000000, 0b00000000, 0b00000000, 0b00000001,
        0b10000000, 0b00000000, 0b00000000, 0b00000001,
        0b01000000, 0b00000000, 0b00000000, 0b00000010,
        0b00000000, 0b00000000, 0b00000000, 0b00000100,
        0b11000000, 0b00000000, 0b00000000, 0b00000001,
        0b00000000, 0b00000000, 0b00000000, 0b00000001,
        0b01000000, 0b00000000, 0b00000000, 0b00000010,
        0b10000000, 0b00000000, 0b00000000, 0b00000001,
        0b01000000, 0b00000000, 0b00000000, 0b00000001,
        0b00000000, 0b00000000, 0b00000000, 0b00000010,
        0b11000000, 0b00000000, 0b00000000, 0b00000001,
    };
    // write data to buffer
    for(size_t i = 0; i < 64; i++) {
        buffer.bytes[i+24] = data[i];
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

    // call load_spiral with buffer and store result
    spiral_t output = load_spiral(buffer);

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
    buffer.bytes = "not the header you were looking for";

    // call load_spiral with buffer and store result
    spiral_t output = load_spiral(buffer);

    if(output.size != 0) {
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
    buffer.bytes = "SAXBOSPIRAL";

    // call load_spiral with buffer and store result
    spiral_t output = load_spiral(buffer);

    if(output.size != 0) {
        result = false;
    }

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
    return result ? 0 : 1;
}
