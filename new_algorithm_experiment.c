/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2019
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

#include "sxbp/sxbp.h"
#include "sxbp/sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// tweak these variables to change which range of problem sizes to test
static const size_t MIN_PROBLEM_SIZE = 1;
static const size_t MAX_PROBLEM_SIZE = 24;

static size_t two_to_the_power_of(size_t power) {
    return (size_t)powl(2.0L, (long double)power);
}

// private data structure for storing proportion of valid solutions for problems
typedef struct ValidSolutionsStatistics {
    size_t problem_size; // for what size of problem (in bits) is this data?
    // the fewest number of valid solutions found across problems of this size
    size_t lowest_validity;
    // the highest number of valid solutions found across problems of this size
    size_t highest_validity;
    // the mean number of valid solutions found across problems of this size
    long double mean_validity;
    // NOTE: to get validity rates as percentages:
    // divide validity count by 2^problem_size
} ValidSolutionsStatistics;

int main(void) {
    // pre-conditional assertions
    assert(MIN_PROBLEM_SIZE > 0); // no point testing a problem of size 0
    assert(MIN_PROBLEM_SIZE <= MAX_PROBLEM_SIZE); // max mustn't be < min
    // TODO: allocate a data structure for tallying % of valid solutions / size
    // for every size of problem...
    for (size_t z = MIN_PROBLEM_SIZE; z < (MAX_PROBLEM_SIZE + 1); z++) {
        // how many problems of that size exist
        size_t problem_size = two_to_the_power_of(z);
        // for every problem of that size...
        for (size_t p = 0; p < problem_size; p++) {
            // TODO: generate a problem for bit string p
            // for every potential solution for a problem of that size...
            for (size_t s = 0; s < problem_size; s++) {
                // printf("%zu\t%zu\t%zu\n", z, p, s);
                // TODO: generate a solution for bit string s
                // TODO: check if the solution is valid for the problem
                // TODO: increment number of solutions if valid
            }
            // TODO: update lowest, highest and cumulative total validity values
        }
        // TODO: divide cumulative total validity by number of problems tested
        // NOTE: this calculation produces the mean validity for this size
        // TODO: store lowest, highest and mean validity values with problem size
        printf("Finished %zu\n", z);
    }
    return 0;
}
