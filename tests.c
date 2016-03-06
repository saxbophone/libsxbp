#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bspirals.h"


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
    uint8_t buffer[] = { 0b01101101, 0b11000111 };
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
    spiral_t output = init_spiral(buffer, 2);

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

int
main(int argc, char const *argv[]) {
    // run tests
    int result = 0;
    printf("test_change_direction: ");
    fflush(stdout);
    if(!test_change_direction()) {
        printf("FAIL\n");
        result = 1;
    } else {
        printf("PASS\n");
    }
    printf("test_init_spiral: ");
    fflush(stdout);
    if(!test_init_spiral()) {
        printf("FAIL\n");
        result = 1;
    } else {
        printf("PASS\n");
    }
    printf("test_plot_spiral: ");
    fflush(stdout);
    if(!test_plot_spiral()) {
        printf("FAIL\n");
        result = 1;
    } else {
        printf("PASS\n");
    }
    return result;
}
