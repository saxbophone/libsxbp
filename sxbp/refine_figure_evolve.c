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
#include <float.h>
#include <inttypes.h>
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
    // array containing the lengths of all lines in this solution
    sxbp_length_t* lengths;
    // the measured fitness of this solution
    double fitness;
} figure_solution;

/*
 * private, allocates memory for the given solution
 * returns whether or not this was done successfully
 */
static bool sxbp_init_figure_solution(figure_solution* solution) {
    solution->lengths = calloc(solution->size, sizeof(sxbp_length_t));
    return (solution->lengths != NULL);
}

// private, deallocates memory for the given solution
static void sxbp_free_figure_solution(figure_solution* solution) {
    free(solution->lengths);
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
        solution->lengths[i] = figure->lines[i].length;
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
        figure->lines[i].length = solution->lengths[i];
    }
}

// private, copies one solution to another. NOTE: size must match!
static void sxbp_copy_solution_to_solution(
    const figure_solution* restrict from,
    figure_solution* restrict to
) {
    to->fitness = from->fitness;
    memcpy(to->lengths, from->lengths, to->size * sizeof(sxbp_length_t));
}

// private, fitness function for scoring figure candidate solutions
static double sxbp_solution_fitness_function(const sxbp_figure_t* figure) {
    // first, get the figure size --if it's too large, we won't check collision
    sxbp_bounds_t bounds = sxbp_get_bounds(figure, 1);
    sxbp_figure_dimension_t dimensions[2];
    sxbp_get_size_from_bounds(bounds, &dimensions[0], &dimensions[1]);
    // printf("%"PRIu32"x%"PRIu32"\n", dimensions[0], dimensions[1]);
    /*
     * we would calculate 1/area as 1/(x*y), but to reduce error, instead we
     * rearrange to (1/x) * (1/y) as follows:
     */
    double area_score = 1.0 / dimensions[0] * 1.0 / dimensions[1];
    if (dimensions[0] > 92681 || dimensions[1] > 92681) {
        // TOO BIG --assume it collided
        return -area_score;
    }
    // next, check if the figure collides
    bool collided = false;
    if (!sxbp_success(sxbp_figure_collides(figure, &collided))) {
        // if there was an error, it was because the figure is too big
        return -area_score; // assume that a figure that's too big is unworkable
    } else if (collided) {
        // figures that collide are invalid --return area score with penalty
        return -area_score;
    } else {
        // printf("SMALL ENOUGH!\n");
        return area_score;
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
    for (sxbp_figure_size_t i = 0; i < parent_a->size; i++) {
        // flip a coin
        bool flip = rand() > (RAND_MAX / 2); // equal chance of choosing parents
        // allocate the alleles accordingly
        offspring_a->lengths[i] = flip ? parent_a->lengths[i]
                                          : parent_b->lengths[i];
        offspring_b->lengths[i] = flip ? parent_b->lengths[i]
                                          : parent_a->lengths[i];
    }
}

// private, sorts the population array by fitness
static bool sxbp_sort_population_by_fitness(
    figure_solution* population,
    size_t size
) {
    /*
     * XXX: uses bubble-sort for now because it is simple and quick to write,
     * but in the future this should be optimised by replacing it with a
     * different, more efficient sorting algorithm
     */
    // we need a temporary figure for swapping
    figure_solution temporary = { .size = (population[0]).size, };
    // if this allocation fails then we can't go on
    if (!sxbp_init_figure_solution(&temporary)) {
        return false;
    }
    for (size_t end = size; end > 0; end--) {
        for (size_t i = 0; i < end - 1; i++) {
            // NOTE: we are sorting for descending order
            if ((population[i]).fitness < (population[i + 1]).fitness) {
                // swap through temporary
                sxbp_copy_solution_to_solution(&population[i], &temporary);
                sxbp_copy_solution_to_solution(&population[i + 1], &population[i]);
                sxbp_copy_solution_to_solution(&temporary, &population[i + 1]);
            }
        }
    }
    // deallocate memory, very important!
    sxbp_free_figure_solution(&temporary);
    return true;
}

static void sxbp_mutate_solution(
    figure_solution* solution,
    double mutation_rate
) {
    // mutate each length according to the mutation rate, by a constrained delta
    for (sxbp_figure_size_t i = 0; i < solution->size; i++) {
        bool flip = ((double)rand() / RAND_MAX) < mutation_rate;
        if (flip) {
            // // we are mutating this line, we need to specify the range of values
            // const sxbp_length_t min_length = 1;
            // const sxbp_length_t max_length = solution->size / 2;
            // // pick a new random value (this is uniform mutation)
            // solution->lengths[i] =
            //     min_length + ((double)rand() / RAND_MAX) * max_length;
            // XXX: experimental gaussian mutation
            // pick a delta from the set { -2, -1, +1, +2, }
            const int deltas[] = { -2, -1, +1, +2, };
            size_t picked = ((double)rand() / RAND_MAX) * 4;
            solution->lengths[i] += (sxbp_length_t)deltas[picked];
            // clip the length
            if (solution->lengths[i] < 1) {
                solution->lengths[i] = 1;
            } else if (solution->lengths[i] > solution->size / 2) {
                solution->lengths[i] = solution->size / 2;
            }
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
    const size_t population_size = 1000;
    const size_t generations = 1000000;
    const double mutation_rate = 0.25;
    const double breeding_rate = 0.333;
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
        printf("initialise individual #%zu\n", i);
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
    // sort the population by fitness
    if (!sxbp_sort_population_by_fitness(population, population_size)) {
        fprintf(stderr, "Could not allocate memory for initial sort\n");
        return SXBP_RESULT_FAIL_MEMORY;
    }
    double best = population[0].fitness;
    printf("Fittest: %.*e\n", DECIMAL_DIG, best);
    // now, simulate each generation of evolution
    for (size_t g = 0; g < generations; g++) {
        if (g % 1000 == 0) {
            printf("\n%zu\n", g);
        }
        printf(".");
        fflush(stdout);
        // NOTE: This is truncation selection and possibly not a good idea
        // select breeding rate % of fittest individuals
        size_t breeding_size = (size_t)(population_size * breeding_rate);
        // select half as many pairs of parents from top fittest to breed
        for (size_t b = 0; b < breeding_size / 2; b++) {
            size_t p_a = (size_t)(((double)rand() / RAND_MAX) * breeding_size);
            size_t p_b = (size_t)(((double)rand() / RAND_MAX) * breeding_size);
            size_t o_a = population_size - b * 2 - 1;
            size_t o_b = population_size - b * 2 - 2;
            // breed the parents and replace weakest with offspring
            sxbp_crossover_breed(
                &population[p_a], &population[p_b], // parents
                &population[o_a], &population[o_b] // offspring
            );
            // mutate the offspring
            sxbp_mutate_solution(&population[o_a], mutation_rate);
            sxbp_mutate_solution(&population[o_b], mutation_rate);
            // calculate new offspring fitnesses
            sxbp_copy_solution_to_figure(&population[o_a], &temporary_figure);
            population[o_a].fitness = sxbp_solution_fitness_function(&temporary_figure);
            // printf("b");
            fflush(stdout);
            sxbp_copy_solution_to_figure(&population[o_b], &temporary_figure);
            population[o_b].fitness = sxbp_solution_fitness_function(&temporary_figure);
            // printf("b");
            // fflush(stdout);
        }
        // sort the population by fitness
        if (!sxbp_sort_population_by_fitness(population, population_size)) {
            fprintf(stderr, "Could not allocate memory for initial sort\n");
            return SXBP_RESULT_FAIL_MEMORY;
        }
        // after every generation has been generated, call callback with fittest
        if (options != NULL && options->progress_callback != NULL) {
            if (population[0].fitness > 0.0 && population[0].fitness != best) { // if the fittest is not invalid
                sxbp_copy_solution_to_figure(&population[0], &temporary_figure);
                options->progress_callback(&temporary_figure, options->callback_context);
                best = population[0].fitness;
                printf("Generations: %zu\n", g);
                printf("Area: %zu\n", (size_t)(1.0 / best));
            }
        }
    }
    // now that we're finished, store the fittest in the original figure
    sxbp_copy_solution_to_figure(&population[0], figure);
    // TODO: now, tidy up your memory!
    return SXBP_RESULT_OK;
}
