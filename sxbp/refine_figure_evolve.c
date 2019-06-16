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

#include "sxbp.h"
#include "sxbp_internal.h"


#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

/*
 * private datatype used for representing a figure candidate solution from the
 * point of view of the genetic algorithm
 */
typedef struct figure_chromosome {
    // the number of lines in the figure this solution is for
    sxbp_figure_size_t size;
    // bit string containing the lengths of all lines in this solution
    bool* bit_string;
} figure_chromosome;

/*
 * private, allocates memory for the given chromosome
 * returns whether or not this was done successfully
 */
static bool sxbp_init_figure_chromosome(figure_chromosome* chromosome) {
    chromosome->bit_string = calloc(
        chromosome->size * 30, // each line length is stored as 30 bits
        sizeof(bool)
    );
    return (chromosome->bit_string != NULL);
}

// private, deallocates memory for the given chromosome
static void sxbp_free_figure_chromosome(figure_chromosome* chromosome) {
    free(chromosome->bit_string);
}

/*
 * private, copies the line lengths of a figure into a chromosome
 * NOTE: size must be the same for both data structures
 */
static void sxbp_copy_figure_to_chromosome(
    const sxbp_figure_t* figure,
    figure_chromosome* chromosome
) {
    for (sxbp_figure_size_t i = 0; i < figure->size; i++) {
        // extract all 30 bits, in big-endian order
        for (size_t j = 0; j < 30; j++) {
            size_t shift = 30 - 1 - j;
            sxbp_length_t mask = 1u << shift;
            // set to true if the bit in this location is set
            chromosome->bit_string[i * 30 + j] =
                (figure->lines[i].length & mask) >> shift;
        }
    }
}

/*
 * private, copies the line lengths from a chromosome into a figure
 * NOTE: size must be the same for both data structures
 */
static void sxbp_copy_chromosome_to_figure(
    const figure_chromosome* chromosome,
    sxbp_figure_t* figure
) {
    return;
}

// private, fitness function for scoring figure candidate solutions
static double sxbp_chromosome_fitness_function(const sxbp_figure_t* figure) {
    return 0.0;
}

/*
 * private, produces new offspring from two parents using uniform crossover
 * NOTE: all chromosomes MUST be the same size
 */
static void sxbp_crossover_breed(
    const figure_chromosome* restrict parent_a,
    const figure_chromosome* restrict parent_b,
    figure_chromosome* restrict offspring_a,
    figure_chromosome* restrict offspring_b
) {
    for (sxbp_figure_size_t i = 0; i < parent_a->size * 30; i++) {
        // flip a coin
        bool flip = rand() > (RAND_MAX / 2); // equal chance of choosing parents
        // allocate the alleles accordingly
        offspring_a.bit_string[i] = flip ? parent_a.bit_string[i]
                                         : parent_b.bit_string[i];
        offspring_b.bit_string[i] = flip ? parent_b.bit_string[i]
                                         : parent_a.bit_string[i];
    }
}

static void sxbp_mutate_chromosome(
    figure_chromosome* chromosome,
    double mutation_rate
) {
    // mutate each bit of the chromosome according to the mutation rate
    for (sxbp_figure_size_t i = 0; i < chromosome->size * 30; i++) {
        bool flip = ((double)rand() / RAND_MAX) < mutation_rate;
        if (flip) {
            chromosome->bit_string[i] = !chromosome->bit_string[i];
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
    // XXX: allow dummy implementation to compile by calling the callback
    if (options != NULL && options->progress_callback != NULL) {
        options->progress_callback(figure, options->callback_context);
    }
    // do nothing, unsuccessfully
    return SXBP_RESULT_FAIL_UNIMPLEMENTED;
}
