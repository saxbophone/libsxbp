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
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sxbp/sxbp.h"
#include "sxbp/sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

// tweak these variables to change which range of problem sizes to test
static const uint8_t MIN_PROBLEM_SIZE = 1;
static const uint8_t MAX_PROBLEM_SIZE = 24;

static uint32_t two_to_the_power_of(uint8_t power) {
    return (uint32_t)powl(2.0L, (long double)power);
}

// unpacks all the bits up to `size` from the given `source` integer into `dest`
static void integer_to_bit_string(uint32_t source, bool* dest, uint8_t size) {
    // NOTE: we handle integers big-endian, but only handle the x lowest bits
    for (uint8_t i = 0; i < size; i++) {
        uint32_t mask = 1U << (size - i - 1);
        if ((source & mask) != 0) {
            dest[i] = true;
        } else {
            dest[i] = false;
        }
    }
}

// private data structure for storing proportion of valid solutions for problems
typedef struct ValidSolutionsStatistics {
    uint8_t problem_size; // for what size of problem (in bits) is this data?
    // the fewest number of valid solutions found across problems of this size
    uint64_t lowest_validity;
    // the highest number of valid solutions found across problems of this size
    uint64_t highest_validity;
    // the mean number of valid solutions found across problems of this size
    long double mean_validity;
    // NOTE: to get validity rates as percentages:
    // divide validity count by 2^problem_size
} ValidSolutionsStatistics;

int main(void) {
    // pre-conditional assertions
    assert(MIN_PROBLEM_SIZE > 0); // no point testing a problem of size 0
    assert(MIN_PROBLEM_SIZE <= MAX_PROBLEM_SIZE); // max mustn't be < min
    // this program works on problem sizes up to 32 bits
    assert(MAX_PROBLEM_SIZE <= 32);
    // allocate a data structure for tallying % of valid solutions / size
    ValidSolutionsStatistics* statistics = calloc(
        (MAX_PROBLEM_SIZE - MIN_PROBLEM_SIZE) + 1,
        sizeof(ValidSolutionsStatistics)
    );
    // allocate a data structure for storing the bits strings of all sizes
    bool* bit_string = calloc(MAX_PROBLEM_SIZE, sizeof(bool));
    // let it abort if any memory allocations were refused
    assert(statistics != NULL);
    assert(bit_string != NULL);
    // for every size of problem...
    for (uint8_t z = MIN_PROBLEM_SIZE; z < (MAX_PROBLEM_SIZE + 1); z++) {
        clock_t start = clock();
        // how many problems of that size exist
        uint32_t problem_size = two_to_the_power_of(z);
        // init highest, lowest and cumulative validity counters
        uint64_t lowest_validity = UINT64_MAX;
        uint64_t highest_validity = 0;
        uint64_t cumulative_validity = 0;
        // for every problem of that size...
        for (uint32_t p = 0; p < problem_size; p++) {
            uint64_t solutions_to_problem = 0;
            // TODO: generate a problem for bit string p
            integer_to_bit_string(p, bit_string, z);
            for (size_t i = 0; i < z; i++) {
                printf("%i", bit_string[i]);
            }
            printf("\n");
            // for every potential solution for a problem of that size...
            for (uint32_t s = 0; s < problem_size; s++) {
                // printf("%zu\t%zu\t%zu\n", z, p, s);
                // TODO: generate a solution for bit string s
                // TODO: check if the solution is valid for the problem
                bool solution_is_valid = true;
                if (solution_is_valid) {
                    // increment number of solutions if valid
                    solutions_to_problem++;
                }
            }
            // update lowest, highest and cumulative total validity values
            if (solutions_to_problem < lowest_validity) {
                lowest_validity = solutions_to_problem;
            }
            if (solutions_to_problem > highest_validity) {
                highest_validity = solutions_to_problem;
            }
            cumulative_validity += solutions_to_problem;
        }
        // TODO: store lowest, highest and mean validity values with problem size
        statistics[z].problem_size = z; // store size in bits, not raw size!
        statistics[z].lowest_validity = lowest_validity;
        statistics[z].highest_validity = highest_validity;
        /*
         * divide cumulative total validity by number of problems tested
         * this calculation produces the mean validity for this size
         */
        statistics[z].mean_validity = (long double)cumulative_validity / problem_size;
        clock_t end = clock();
        printf("Finished %" PRIu8 "\t(%Lf)\n", z, (long double)(end - start) / CLOCKS_PER_SEC);
        printf(
            "problem_size = %" PRIu32
            "\nlowest_validity = %" PRIu64
            "\nhighest_validity = %" PRIu64
            "\nmean_validity = %Lf\n",
            two_to_the_power_of(statistics[z].problem_size),
            statistics[z].lowest_validity,
            statistics[z].highest_validity,
            statistics[z].mean_validity
        );
    }
    // deallocate memory
    free(statistics);
    return 0;
}
