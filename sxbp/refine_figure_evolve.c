/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides the definition of
 * `sxbp_refine_figure_evolve`, a public function which attempts to refine a
 * figure using an evolutionary algorithm to evolve a solution which is most
 * compact.
 *
 * NOTE: This algorithm is experimental!
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018-2019
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

/*
 * private datatype used for representing a figure candidate solution from the
 * point of view of the genetic algorithm
 */
typedef struct figure_solution {
    // the number of lines in the figure this solution is for
    sxbp_figure_size_t size;
    // bit string containing the lengths of all lines in this solution
    bool* bit_string;
    // the measured fitness of this solution
    double fitness;
} figure_solution;

/*
 * private, allocates memory for the given solution
 * returns whether or not this was done successfully
 */
static bool sxbp_init_figure_solution(figure_solution* solution) {
    solution->bit_string = calloc(
        solution->size * 30, // each line length is stored as 30 bits
        sizeof(bool)
    );
    return (solution->bit_string != NULL);
}

// private, deallocates memory for the given solution
static void sxbp_free_figure_solution(figure_solution* solution) {
    free(solution->bit_string);
}

/*
 * private, copies the line lengths of a figure into a solution
 * NOTE: size must be the same for both data structures
 */
static void sxbp_copy_figure_to_solution(
    const sxbp_figure_t* figure,
    figure_solution* solution
) {
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        // extract all 30 bits, in big-endian order
        for (size_t j = 0; j < 30; j++) {
            size_t shift = 30 - 1 - j;
            sxbp_length_t mask = 1u << shift;
            // set to true if the bit in this location is set
            solution->bit_string[i * 30 + j] =
                (figure->lines[i].length & mask) >> shift;
        }
    }
}

/*
 * private, copies the line lengths from a solution into a figure
 * NOTE: size must be the same for both data structures
 */
static void sxbp_copy_solution_to_figure(
    const figure_solution* solution,
    sxbp_figure_t* figure
) {
    for (sxbp_figure_size_t i = 0; i < solution->size; i++) {
        // clear the line length to 0 (we are setting bits when 1)
        figure->lines[i].length = 0;
        // line lengths are 30 bits, packed in big-endian order
        for (size_t j = 0; j < 30; j++) {
            // if this bit is set
            if (solution->bit_string[i * 30 + j]) {
                size_t shift = 30 - 1 - j;
                sxbp_length_t mask = 1u << shift;
                // or-mask to set the bit in this position
                figure->lines[i].length |= mask;
            }
        }
    }
}

// private, copies one solution to another. NOTE: size must match!
static void sxbp_copy_solution_to_solution(
    const figure_solution* restrict from,
    figure_solution* restrict to
) {
    memcpy(to->bit_string, from->bit_string, to->size * 30 * sizeof(bool));
}

// private, fitness function for scoring figure candidate solutions
static double sxbp_solution_fitness_function(const sxbp_figure_t* figure) {
    // first, check if the figure collides
    bool collided = false;
    if (!sxbp_success(sxbp_figure_collides(figure, &collided))) {
        // if there was an error, then return negative
        return -1.0;
    } else if (collided) {
        // figures that collide are invalid so we need to return the worst value
        return 0.0;
    } else {
        // calculate figure area and return 1/area
        sxbp_bounds_t bounds = sxbp_get_bounds(figure, 1);
        sxbp_figure_dimension_t dimensions[2];
        sxbp_get_size_from_bounds(bounds, &dimensions[0], &dimensions[1]);
        /*
         * we would calculate 1/area as 1/(x*y), but to reduce error, instead we
         * rearrange to (1/x) * (1/y) as follows:
         */
        return 1.0 / dimensions[0] * 1.0 / dimensions[1];
    }
}

/*
 * private, produces new offspring from two parents using uniform crossover
 * NOTE: all solutions MUST be the same size
 */
static void sxbp_crossover_breed(
    const figure_solution* restrict parent_a,
    const figure_solution* restrict parent_b,
    figure_solution* restrict offspring_a,
    figure_solution* restrict offspring_b
) {
    for (sxbp_figure_size_t i = 0; i < parent_a->size * 30; i++) {
        // flip a coin
        bool flip = rand() > (RAND_MAX / 2); // equal chance of choosing parents
        // allocate the alleles accordingly
        offspring_a->bit_string[i] = flip ? parent_a->bit_string[i]
                                          : parent_b->bit_string[i];
        offspring_b->bit_string[i] = flip ? parent_b->bit_string[i]
                                          : parent_a->bit_string[i];
    }
}

static void sxbp_mutate_solution(
    figure_solution* solution,
    double mutation_rate
) {
    // mutate each bit of the solution according to the mutation rate
    for (sxbp_figure_size_t i = 0; i < solution->size * 30; i++) {
        bool flip = ((double)rand() / RAND_MAX) < mutation_rate;
        if (flip) {
            solution->bit_string[i] = !solution->bit_string[i];
        }
    }
}

sxbp_result_t sxbp_refine_figure_evolve(
    sxbp_figure_t* figure,
    const sxbp_refine_figure_options_t* options
) {
    /*
     * EVOLUTIONARY ALGORITHM STEPS:
     * - INITIALISE Population (randomly or seeded from initial figure)
     * - THEN FOR EACH GENERATION:
     *   - SELECT individuals for breeding
     *   - CROSSOVER (breed) random pairs of breeding individuals
     *   - MUTATE offspring
     */
    const size_t population_size = 50;
    const size_t generations = 100;
    const double mutation_rate = 0.05;
    const double breeding_rate = 0.5;
    figure_solution* population = calloc(
        population_size,
        sizeof(figure_solution)
    );
    if (population == NULL) {
        fprintf(stderr, "Can't allocate memory for population.\n");
        return SXBP_RESULT_FAIL_MEMORY;
    }
    figure_solution starting_solution = { .size = figure->size, };
    if (!sxbp_init_figure_solution(&starting_solution)) {
        fprintf(stderr, "Can't allocate memory for starting solution.\n");
        return SXBP_RESULT_FAIL_MEMORY;
    }
    sxbp_figure_t temporary_figure = sxbp_blank_figure();
    sxbp_result_t status = SXBP_RESULT_UNKNOWN;
    if (!sxbp_check(sxbp_copy_figure(figure, &temporary_figure), &status)) {
        fprintf(stderr, "Can't copy figure into temporary figure.\n");
        return status;
    }
    // extract the figure as it is currently --this is the seed
    sxbp_copy_figure_to_solution(figure, &starting_solution);
    // initialise the population with mutated versions of the starting figure
    for (size_t i = 0; i < population_size; i++) {
        // allocate each figure in turn
        population[i].size = starting_solution.size;
        if (!sxbp_init_figure_solution(&population[i])) {
            fprintf(stderr, "Can't allocate memory for individual #%zu.\n", i);
            return SXBP_RESULT_FAIL_MEMORY;
        }
        // copy the starting figure into it and mutate it
        sxbp_copy_solution_to_solution(&starting_solution, &population[i]);
        sxbp_mutate_solution(&population[i], mutation_rate);
        // store the fitness value of the new individual
        sxbp_copy_solution_to_figure(&population[i], &temporary_figure);
        population[i].fitness = sxbp_solution_fitness_function(&temporary_figure);
    }
    // TODO: now, tidy up your memory!
    // XXX: allow dummy implementation to compile by calling the callback
    if (options != NULL && options->progress_callback != NULL) {
        options->progress_callback(figure, options->callback_context);
    }
    // do nothing, unsuccessfully
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
