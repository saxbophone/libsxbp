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

#include "sxbp/figure_collides.h"
#include "sxbp/sxbp.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

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

// tweak these variables to change which range of problem sizes to test
static const uint8_t MIN_PROBLEM_SIZE = 1;
static const uint8_t MAX_PROBLEM_SIZE = 18;

// this is my estimate of the factor of complexity increase of 1 additional bit
static const double COMPLEXITY_FACTOR_ESTIMATE = 4.3;
// config variable for timing logic --maximum duration to measure with CPU clock
static const double MAX_CPU_CLOCK_TIME = 60.0; // 1 minute

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

static bool is_solution_valid_for_problem(
    uint8_t size,
    bool* solution,
    bool* problem
) {
    // create and allocate memory for a figure of the correct size
    sxbp_Figure figure = sxbp_blank_figure();
    figure.size = size + 1U; // inlcudes 1 additional starter line as orientation
    if (!sxbp_success(sxbp_init_figure(&figure))) {
        abort(); // XXX: Cheap allocation failure exit!
    }
    // hardcode the first line, which is always the same
    figure.lines[0].direction = SXBP_UP;
    figure.lines[0].length = 3;
    // set the line lengths and directions from the problem and solution
    sxbp_Direction current_direction = SXBP_UP;
    for (uint8_t i = 0; i < size; i++) {
        // if bit is 1, turn right, otherwise, turn left
        if (solution[i]) {
            current_direction = (current_direction - 1) % 4;
        } else {
            current_direction = (current_direction + 1) % 4;
        }
        figure.lines[i + 1].length = problem[i] ? 2 : 1;
        figure.lines[i + 1].direction = current_direction;
    }
    // check if figure collides and store result
    bool figure_collides = false;
    if (!sxbp_success(sxbp_figure_collides(&figure, &figure_collides))) {
        abort(); // XXX: Cheap allocation failure exit!
    }

    // XXX: DEBUGGING CODE FOR PRINTING EVERY SINGLE TESTED CANDIDATE
    // if (!figure_collides){
    //     sxbp_Bitmap bitmap = sxbp_blank_bitmap();
    //     sxbp_render_figure_to_bitmap(&figure, &bitmap);
    //     sxbp_print_bitmap(&bitmap, stdout);
    //     sxbp_free_bitmap(&bitmap);
    // }

    // free memory for figure
    sxbp_free_figure(&figure);
    // return result
    return !figure_collides;
}

static FILE* open_file_for_appending(const char* filename) {
    FILE* file_handle = fopen(filename, "a");
    // abort if file couldn't be opened
    if (file_handle == NULL) {
        fprintf(stderr, "Can't open file for appending!\n");
        abort();
    }
    return file_handle;
}

static FILE* close_file(FILE* file_handle) {
    // try to close file, abort if unsuccessful
    if (fclose(file_handle) != 0) {
        fprintf(stderr, "Can't close open file!\n");
        abort();
    }
    return NULL;
}

static double estimated_completion_time(
    double latest_run_time,
    uint8_t factors_left
) {
    double estimate = 0.0;
    for (uint8_t f = 1; f < (factors_left + 1); f++) {
        estimate += latest_run_time * pow(COMPLEXITY_FACTOR_ESTIMATE, f);
    }
    return estimate;
}

int main(int argc, char const *argv[]) {
    // pre-conditional assertions
    assert(MIN_PROBLEM_SIZE > 0); // no point testing a problem of size 0
    assert(MIN_PROBLEM_SIZE <= MAX_PROBLEM_SIZE); // max mustn't be < min
    // this program works on problem sizes up to 32 bits
    assert(MAX_PROBLEM_SIZE <= 32);
    // we need one additional argument --for the file name to output to
    if (argc < 2) {
        fprintf(stderr, "Need filename argument!\n");
        return -1;
    }
    const char* filename = argv[1]; // grab filename out of arguments
    // allocate a data structure for tallying % of valid solutions / size
    ValidSolutionsStatistics* statistics = calloc(
        (size_t)((MAX_PROBLEM_SIZE - MIN_PROBLEM_SIZE) + 1),
        sizeof(ValidSolutionsStatistics)
    );
    // allocate data structure for storing problem and solution bit strings
    bool* problem = calloc(MAX_PROBLEM_SIZE, sizeof(bool));
    bool* solution = calloc(MAX_PROBLEM_SIZE, sizeof(bool));
    // let it abort if any memory allocations were refused
    assert(statistics != NULL);
    assert(problem != NULL);
    assert(solution != NULL);

    /*
     * by keeping track of how long previous runs take, we can estimate how long
     * it will take to complete the entire experiment
     */
    // double previous_time = 0.0;

    // write out the CSV file row headings
    FILE* csv_file = open_file_for_appending(filename);
    fprintf(
        csv_file,
        "Timestamp,Bits,Problem Size,Lowest Validity,Highest Validity,Mean Validity\n"
    );
    csv_file = close_file(csv_file);
    // for every size of problem...
    for (uint8_t z = MIN_PROBLEM_SIZE; z < (MAX_PROBLEM_SIZE + 1); z++) {
        // XXX: Timing logic
        clock_t sub_second_start_time = clock();
        time_t start_time = time(NULL);
        // how many problems of that size exist
        uint32_t problem_size = two_to_the_power_of(z);
        // init highest, lowest and cumulative validity counters
        uint64_t lowest_validity = UINT64_MAX;
        uint64_t highest_validity = 0;
        uint64_t cumulative_validity = 0;
        // for every problem of that size...
        for (uint32_t p = 0; p < problem_size; p++) {
            uint64_t solutions_to_problem = 0;
            // generate a problem for bit string p
            integer_to_bit_string(p, problem, z);
            // for every potential solution for a problem of that size...
            for (uint32_t s = 0; s < problem_size; s++) {
                // generate a solution for bit string s
                integer_to_bit_string(s, solution, z);
                // check if the solution is valid for the problem
                if (is_solution_valid_for_problem(z, solution, problem)) {
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

        // XXX: Timing logic
        time_t now = time(NULL);
        char time_buffer[21];
        strftime(time_buffer, sizeof(time_buffer), "%FT%TZ", gmtime(&now));

        FILE* csv_file = open_file_for_appending(filename);
        fprintf(
            csv_file,
            "%s,%" PRIu8 ",%" PRIu32 ",%" PRIu64 ",%" PRIu64 ",%Lf\n",
            time_buffer,
            z,
            two_to_the_power_of(statistics[z].problem_size),
            statistics[z].lowest_validity,
            statistics[z].highest_validity,
            statistics[z].mean_validity
        );
        csv_file = close_file(csv_file);
        printf("============================= %s =============================\n", time_buffer);
        double seconds_elapsed = difftime(now, start_time);
        if (seconds_elapsed < MAX_CPU_CLOCK_TIME) {
            seconds_elapsed = (double)(
                clock() - sub_second_start_time
            ) / CLOCKS_PER_SEC;
        }
        // printf("Time Factor: %f\n", seconds_elapsed / previous_time);
        printf("Solved problem size: %" PRIu8 " - Time taken:\t%f\n", z, seconds_elapsed);
        printf(
            "Estimated time til completion:\t\t%f\n",
            estimated_completion_time(seconds_elapsed, MAX_PROBLEM_SIZE - z)
        );
        if (z < MAX_PROBLEM_SIZE) {
            printf(
                "Estimated time til next solved:\t\t%f\n",
                estimated_completion_time(seconds_elapsed, 1)
            );
        }
        printf("================================================================================\n\n");
        // previous_time = seconds_elapsed;
    }
    // deallocate memory
    // free(statistics); // FIXME: for some reason, this causes a corruption/double-free...
    return 0;
}
