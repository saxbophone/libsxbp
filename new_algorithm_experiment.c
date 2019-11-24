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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

typedef struct CommandLineOptions {
    uint8_t start_problem_size; // size of problem to start with in bits
    uint8_t end_problem_size; // size of problem to end with in bits
    size_t max_ram_per_process; // max RAM allowed per process in bytes
} CommandLineOptions;

/*
 * the data type used to store problems and solutions
 * NOTE: this must be able to store at least 2^N values where N is the maximum
 * desired problem size in bits
 */
typedef RepresentationBase uint64_t;
typedef Problem RepresentationBase;
typedef Solution RepresentationBase;

/*
 * stores all the valid solutions for a given problem
 * (problem is not stored in this particular struct)
 */
typedef struct SolutionSet {
    uint8_t bits; // how many bits wide these solutions are
    size_t count; // how many solutions there are
    Solution* solutions; // dynamically allocated array of count many solutions
} SolutionSet;

typedef struct ProblemSet {
    uint8_t bits; // how many bits wide these problems are
    size_t count; // how many problems there are
    SolutionSet* problem_solutions; // dynamic array, problem is index number
} ProblemSet;

// private functions which are used directly by main()

static CommandLineOptions parse_command_line_options(
    int argc,
    char const *argv[]
);

/*
 * generate all the problems and solutions of a given bit size and populate the
 * given problem_set with them
 */
static bool generate_problems_and_solutions(
    ProblemSet* problem_set,
    uint8_t bits
);

int main(int argc, char const *argv[]) {
    CommandLineOptions options = parse_command_line_options(argc, argv);
    return 0;
}

/*
 * private functions which are used only by other private functions which are
 * used directly by main()
 */

/*
 * returns the expected number of mean valid solutions per problem for the given
 * problem size in bits
 */
static size_t predict_number_of_valid_solutions(uint8_t problem_size);

/*
 * uses A-B-exponential using magic constants derived from regression of
 * existing exhaustive test data to get the validity percentage for a problem of
 * a given size in bits
 */
static long double mean_validity(uint8_t problem_size);

// implementations of all private functions
