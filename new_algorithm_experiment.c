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
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

#include "sxbp/figure_collides.h"
#include "sxbp/sxbp.h"

#ifdef __cplusplus
#error "This file is ISO C99. It should not be compiled with a C++ Compiler."
#endif

/*
 * stores book-keeping data about the MPI cluster this process is operating in,
 * and about this process itself. The data stored in here is specific to the MPI
 * cluster computing library that this search program uses.
 */
typedef struct ClusterBookKeeping {
    size_t world_size; // the total number of processes in this cluster
    size_t rank; // our process' rank number in this cluster
    char name[MPI_MAX_PROCESSOR_NAME]; // the name of this processor
    size_t name_length; // the length of this processor's name
} ClusterBookKeeping;

typedef uint_fast8_t ProblemSize;

typedef struct CommandLineOptions {
    ProblemSize start_problem_size; // size of problem to start with in bits
    ProblemSize end_problem_size; // size of problem to end with in bits
    size_t max_ram_per_process; // max RAM allowed per process in bytes
} CommandLineOptions;

/*
 * the data type used to store problems and solutions
 * NOTE: this must be able to store at least 2^N values where N is the maximum
 * desired problem size in bits
 * XXX: Changed to exact 32-bit to use less memory
 */
typedef uint32_t RepresentationBase;
typedef RepresentationBase Problem;
typedef RepresentationBase Solution;

/*
 * stores all the valid solutions for a given problem, excluding terminals,
 * however terminals are still counted
 */
typedef struct SolutionSet {
    size_t allocated_size; // how many solutions memory has been allocated for
    size_t count; // how many non-terminal solutions there are
    size_t all_count; // how many solutions there are (terminals included)
    Problem problem; // the problem bit string that these solutions are for
    Solution* solutions; // dynamically allocated array of count many solutions
} SolutionSet;

typedef struct ProblemSet {
    ProblemSize bits; // how many bits wide these problems are
    size_t count; // how many problems there are
    SolutionSet* problem_solutions; // dynamic array of solutions to problems
} ProblemSet;

typedef struct ProblemStatistics {
    ProblemSize bits; // what size of problem these statistics are for
    // the fewest number of valid solutions found across problems of this size
    size_t lowest_validity;
    // the highest number of valid solutions found across problems of this size
    size_t highest_validity;
    // the mean number of valid solutions found across problems of this size
    long double mean_validity;
    // NOTE: to get validity rates as percentages:
    // divide validity count by the number of samples in the data
} ProblemStatistics;

// private constants

// these constants are calculated from A-B-exponential regression on search data
static const long double MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_A = 1.56236069184829962203;
static const long double MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_B = 0.8329257011252032045966;
// a size of problem that we can guarantee we can store and is fast to solve
static const ProblemSize SMALL_REASONABLY_FAST_CACHEABLE_PROBLEM_SIZE = 10U;
// how much extra memory we allocate for solution sets when they require more space
static const size_t SOLUTION_SET_OVER_ALLOCATE_AMOUNT = 1024U;

// this one is not constant, but it is private --MPI book-keeping data
ClusterBookKeeping CLUSTER_METADATA = {0};

// private functions which are used directly by main()

/*
 * retrieves data about this cluster and this process, stores it in a private
 * global variable for book-keeping purposes
 * NOTE: it is unsafe to call this function before MPI_Init() has been called.
 */
static void init_cluster_book_keeping(void);

/*
 * these are our custom wrappers of the stdio printf() family functions
 * --wrapping them allows us to prefix output with the node name and rank to
 * allow log messages to be made better sense of
 */
static int cluster_printf(const char*restrict format, ...);
static int cluster_fprintf(
    FILE*restrict stream, const char*restrict format, ...
);

// attempts to parse command line and return options, calls exit() if bad args
static CommandLineOptions parse_command_line_options(
    int argc,
    char *argv[]
);

// returns the number of problems that there are in the given inclusive range
static size_t count_problems_in_range(ProblemSize start, ProblemSize end);

/*
 * finds the largest problem size (in bits) which can be represented with the
 * given RAM limit per-process
 */
static ProblemSize find_largest_cacheable_problem_size(size_t ram_limit);

/*
 * returns the number of bytes needed to cache the solutions to all problems of
 * given problem size
 */
static size_t get_cache_size_of_problem(ProblemSize problem_size);

/*
 * populates the given problem set with this process' share of the problems and
 * solutions for the given problem size.
 * if statistics is not NULL, this will be populated with the statistics for
 * this part of the problem
 * returns false if error, this will be due to memory allocation failing
 */
static bool generate_initial_cache(
    ProblemSet* problem_cache,
    ProblemSize problem_size,
    ProblemStatistics* statistics
);

/*
 * collates statistics across nodes from statistics, into all_statistics on the
 * master node
 */
static void collect_statistics(
    const ProblemStatistics* statistics,
    ProblemStatistics* all_statistics
);

// initialises the given statistics object if given, does nothing if NULL
static void init_statistics(ProblemStatistics* statistics, ProblemSize bits);

/*
 * calculates the mean validity statistics if not NULL, to be called once after
 * testing of a given problem is finished
 */
static void finalise_statistics(ProblemStatistics* statistics);

/*
 * using the existing problem cache, generates additional problems deriving from
 * them and finds all their solutions, counting all the valid ones but storing
 * only those that are guaranteed to have at least one child
 */
static bool generate_next_problem_solutions_from_current(
    ProblemSet* problem_set, ProblemStatistics* statistics
);

// deallocates any dynamically allocated memory in the given problem set
static void deallocate_problem_set(ProblemSet* problem_set);

int main(int argc, char *argv[]) {
    // initialise MPI before anything else is done
    MPI_Init(&argc, &argv);
    // retrieve the metadata about this cluster
    init_cluster_book_keeping();
    // get the options on command line. program will exit if these are not valid
    CommandLineOptions options = parse_command_line_options(argc, argv);
    cluster_printf(
        "Start: %" PRIuFAST8 " End: %" PRIuFAST8 " RAM: %zu\n",
        options.start_problem_size,
        options.end_problem_size,
        options.max_ram_per_process
    );
    // this is how many problems we have been requested to solve
    size_t problems_count = count_problems_in_range(
        options.start_problem_size, options.end_problem_size
    );
    // find the largest cacheable problem size for our specified RAM limit
    ProblemSize largest_cacheable = find_largest_cacheable_problem_size(
        options.max_ram_per_process * CLUSTER_METADATA.world_size
    );
    // barrier to synchronise output
    MPI_Barrier(MPI_COMM_WORLD);
    if (CLUSTER_METADATA.rank == 0) {
        cluster_printf(
            "Can cache up to %" PRIuFAST8 "bits (%zu bytes)\n",
            largest_cacheable,
            get_cache_size_of_problem(largest_cacheable - 1U) +
            get_cache_size_of_problem(largest_cacheable)
        );
    }
    // barrier to synchronise output
    MPI_Barrier(MPI_COMM_WORLD);
    // work out what the options passed to the program require us to do
    ProblemSize cache_start_size, cache_end_size;
    // if smallest problem size to solve is not greater than cacheable size
    if (options.start_problem_size <= largest_cacheable) {
        // solve all problems from smallest to largest cacheable
        cache_start_size = options.start_problem_size;
        // use smallest of end problem or largest cacheable
        cache_end_size = largest_cacheable < options.end_problem_size ?
                                                    largest_cacheable :
                                                    options.end_problem_size;
    } else { // otherwise if smallest problem is greater than cacheable size
        // solve all problems from a reasonably small and fast size and cache
        cache_start_size = SMALL_REASONABLY_FAST_CACHEABLE_PROBLEM_SIZE;
        cache_end_size = largest_cacheable;
    }
    // NOTE: work splits in a parallel fashion at this point
    ProblemSize current_size = cache_start_size; 
    // this is the problem cache for this processor
    ProblemSet problem_cache = {0};
    // this is the statistics store for this processor
    ProblemStatistics statistics = {0};
    // these variables are used by master only for collating statistics
    ProblemStatistics all_statistics = {0};
    // generate the initial cache for our share of the problem
    if (
        !generate_initial_cache(
            &problem_cache,
            current_size,
            // only include pointer to statistics if we want stats from problem
            current_size < options.start_problem_size ? NULL : &statistics
        )
    ) {
        abort(); // cheap error-handling
    }
    // send our own part of the statistics to master if required
    if (current_size >= options.start_problem_size) {
        collect_statistics(&statistics, &all_statistics);
    }
    // TODO: master collates statistics and writes to file if required
    current_size++;
    // FIXME: until statistics-collation is implemented, MPI_Barrier() is used
    MPI_Barrier(MPI_COMM_WORLD);
    // then until max problems are cached
    while (current_size <= cache_end_size) {
        // reset statistics
        statistics = (ProblemStatistics){0};
        all_statistics = (ProblemStatistics){0};
        // generate extension problems from our own problems and solve
        if (
            !generate_next_problem_solutions_from_current(
                &problem_cache,
                // only include pointer to statistics if we want stats
                current_size < options.start_problem_size ? NULL : &statistics
            )
        ) {
            abort(); // cheap error-handling
        }
        // send our own part of the statistics to master if required
        if (current_size >= options.start_problem_size) {
            collect_statistics(&statistics, &all_statistics);
        }
        // TODO: master collates statistics and writes to file if required
        // TODO: rebalance cache among processes
        current_size++;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // then until max problem size is searched:
    while (current_size <= options.end_problem_size) {
        // TODO: search our share of the problem space and collect statistics
        // TODO: send statistics to master
        // TODO: master collates statistics and writes to file
        current_size++;
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // free dynamically-allocated memory
    deallocate_problem_set(&problem_cache);
    // close down MPI
    MPI_Finalize();
    return 0;
}

/*
 * private functions which are used only by other private functions which are
 * used directly by main()
 */

/*
 * almost too simple to put in a function, but added for readability.
 * quickly calculate powers of two
 * NOTE: this overflows without warning if a power greater than 1 less of the
 * maximum bit width of integer supported by the system is given. (so a 64-bit
 * system will overflow if 64 is passed).
 */
static uintmax_t two_to_the_power_of(uint_fast8_t power);

/*
 * returns the expected number of mean valid solutions per problem for the given
 * problem size in bits
 */
static size_t predict_number_of_valid_solutions(ProblemSize problem_size);

// retrieves the range of problems this processor has to solve
static void get_problem_share(
    ProblemSize problem_size, Problem* start, Problem* end
);

/*
 * allocates memory for the dynamic members of the given problem set
 * returns false if memory allocation failed
 * NOTE: this does not allocate memory for the nested solution set members
 * this functionality is provided separately
 * NOTE: this function also sets the members `count` and `bits`
 */
static bool allocate_problem_set(
    ProblemSet* problem_set, ProblemSize problem_size, size_t problems_count
);

/*
 * returns collision result of given solution for the given problem, both of
 * given size in bits
 */
static sxbp_CollisionResult solution_is_valid_for_problem(
    ProblemSize size, Problem problem, Solution solution, bool detect_terminals
);

/*
 * tries to append the given solution to the given solution set
 * returns false if unsuccessful (this will be because of memory allocation fail)
 */
static bool add_solution_to_solution_set(SolutionSet* solution_set, Solution s);

/*
 * reallocates the memory of a solution set to be exactly the size it needs
 * this typically implies a shrinking from over-allocated size to exact size,
 * so is unlikely to fail, but if it does this function returns false
 */
static bool shrink_solution_set(SolutionSet* solution_set);

/*
 * updates the given statistics object (if not NULL) with the given solutions
 * count of a problem that has finished testing
 * this will update lowest and highest validity if needed, and accumulate the
 * mean validity
 */
static void update_statistics(
    ProblemStatistics* statistics, size_t solutions_count
);

/*
 * uses A-B-exponential using magic constants derived from regression of
 * existing exhaustive test data to get the validity percentage for a problem of
 * a given size in bits
 * float percentage is returned where 0.0 is 0% and 1.0% is 100%
 */
static long double mean_validity(ProblemSize problem_size);

// unpacks all the bits up to `size` from the given `source` integer into `dest`
static void integer_to_bit_string(
    ProblemSize size, RepresentationBase source, bool dest[size]
);

/*
 * Using cached data in the given problem set, generate the problems and valid
 * solutions to them for the next problem size.
 * Modifies problem_set in-place to place the problems and solutions of the next
 * problem size directly into that structure.
 * If statistics is not NULL, it will be updated with the statistics about the
 * next problem size.
 * Returns false if it fails (due to memory allocation issues)
 */
static bool generate_next_problem_solutions_from_current(
    ProblemSet* problem_set, ProblemStatistics* statistics
);

/*
 * finds all the solutions for the given problem size, taking advantage of the
 * given problem cache to speed up the process if possible
 * statistics about the solutions that were found are written to the statistics
 * object given.
 * returns true/false for if this process succeeded or failed
 */
static bool find_solutions_for_problem(
    ProblemSize size,
    const ProblemSet* problem_cache,
    ProblemStatistics* statistics
);

// implementations of all private functions

static void init_cluster_book_keeping(void) {
    // collect all metadata about the cluster and store in private global
    int world_size = 0, rank = -1, name_length = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    CLUSTER_METADATA.world_size = (size_t)world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    CLUSTER_METADATA.rank = (size_t)rank;
    MPI_Get_processor_name(CLUSTER_METADATA.name, &name_length);
    CLUSTER_METADATA.name_length = (size_t)name_length;
}

static int cluster_printf(const char*restrict format, ...) {
    // begin handling variadic arguments
    va_list args;
    va_start(args, format);
    // print out the processor name and rank preceding the rest of the output
    printf("[%s:%02zu] ", CLUSTER_METADATA.name, CLUSTER_METADATA.rank);
    // now print out what was actually requested of the function
    int result = vprintf(format, args);
    // finally, finish handling variadic arguments
    va_end(args);
    return result;
}

static int cluster_fprintf(
    FILE*restrict stream, const char*restrict format, ...
) {
    // begin handling variadic arguments
    va_list args;
    va_start(args, format);
    // print out the processor name and rank preceding the rest of the output
    fprintf(stream, "[%s:%02zu] ", CLUSTER_METADATA.name, CLUSTER_METADATA.rank);
    // now print out what was actually requested of the function
    int result = vfprintf(stream, format, args);
    // finally, finish handling variadic arguments
    va_end(args);
    return result;
}

static CommandLineOptions parse_command_line_options(
    int argc,
    char *argv[]
) {
    if (argc < 4) exit(-1); // we need 3 additional arguments besides file name
    CommandLineOptions options = {0};
    options.start_problem_size = strtoul(argv[1], NULL, 10);
    options.end_problem_size = strtoul(argv[2], NULL, 10);
    options.max_ram_per_process = strtoul(argv[3], NULL, 10);
    if (
        options.start_problem_size == 0
        || options.end_problem_size == 0
        || options.max_ram_per_process == 0
    ) {
        exit(-1); // none of them can be zero
    }
    return options;
}

static size_t count_problems_in_range(ProblemSize start, ProblemSize end) {
    return (ProblemSize)(end - start) + 1U;
}

static ProblemSize find_largest_cacheable_problem_size(size_t ram_limit) {
    ProblemSize problem_size;
    // this typically won't actually get to 32, 22 bits gives ~1TiB size!
    for (problem_size = 1; problem_size < 32; problem_size++) {
        /*
         * we need to add the cache size of two adjacent problems
         * --the reason we need to calculate this is so that we can gaurantee
         * that we have enough space to store a previous run and the next one,
         * this greatly simplifies the process of generating future results from
         * previous ones because it means we can prove that we have enough RAM
         * to hold them both in memory at once so we can copy between them
         */
        size_t cache_size = (
            get_cache_size_of_problem(problem_size - 1) +
            get_cache_size_of_problem(problem_size)
        );
        // stop when we find a problem size that exceeds our ram limit
        if (cache_size > ram_limit) return problem_size - 1;
    }
    // probably will never be reached, but in case it is, return something
    return problem_size - 1;
}

static size_t get_cache_size_of_problem(ProblemSize problem_size) {
    if (problem_size == 0) return 0;
    if (problem_size < 6) { // problem sizes below this do not follow the trend
        return (
            sizeof(ProblemSet) + (
                two_to_the_power_of(problem_size) * // number of problems
                (
                    sizeof(SolutionSet) +
                    (
                        two_to_the_power_of(problem_size) * // solutions
                        sizeof(Solution)
                    )
                )
            )
        );
    }
    // all problems of size 6 or above do follow the trend line we have plotted
    return (
        sizeof(ProblemSet) + (
            two_to_the_power_of(problem_size) * // number of problems
            (
                sizeof(SolutionSet) +
                (
                    predict_number_of_valid_solutions(problem_size) * // solutions
                    sizeof(Solution)
                )
            )
        )
    );
}

static bool generate_initial_cache(
    ProblemSet* problem_cache,
    ProblemSize problem_size,
    ProblemStatistics* statistics
) {
    // work out this process' share of the problems
    Problem start, end;
    get_problem_share(problem_size, &start, &end);
    size_t problems_count = end - start;
    // for the first problem size, allocate a problem set for that size
    if (!allocate_problem_set(problem_cache, problem_size, problems_count)) {
        return false;
    }
    // estimated mean number of solutions per problem
    size_t estimated_solutions = predict_number_of_valid_solutions(problem_size);
    // init statistics
    init_statistics(statistics, problem_cache->bits);
    // populate with all the problems in our share
    for (Problem p = 0; p < problems_count; p++) {
        problem_cache->problem_solutions[p].problem = p + start;
        // try and allocate memory for the solutions --allocate estimated number
        problem_cache->problem_solutions[p].solutions = calloc(
            estimated_solutions, sizeof(Solution)
        );
        if (problem_cache->problem_solutions[p].solutions == NULL) {
            // memory allocation failure
            return false;
        }
        // set number allocated for memory book-keeping purposes
        problem_cache->problem_solutions[p].allocated_size = estimated_solutions;
        // find valid solutions and add to the list of problem solutions
        for (Solution s = 0; s < two_to_the_power_of(problem_size); s++) {
            switch (
                solution_is_valid_for_problem(
                    problem_cache->bits,
                    problem_cache->problem_solutions[p].problem,
                    s,
                    true
                )
            ) {
            case SXBP_COLLISION_RESULT_COLLIDES:
                break;
            case SXBP_COLLISION_RESULT_CONTINUES:
                if (
                    !add_solution_to_solution_set(
                        &problem_cache->problem_solutions[p], s
                    )
                ) {
                    // memory allocation failure
                    return false;
                }
            case SXBP_COLLISION_RESULT_TERMINATES:
                problem_cache->problem_solutions[p].all_count++;
                break;
            }
        }
        // shrink solution set down to waste less memory
        if (!shrink_solution_set(&problem_cache->problem_solutions[p])) {
            // memory allocation failure
            return false;
        }
        // update statistics
        update_statistics(
            statistics, problem_cache->problem_solutions[p].all_count
        );
    }
    return true; // success
}

static void collect_statistics(
    const ProblemStatistics* statistics,
    ProblemStatistics* all_statistics
) {
    size_t* all_lowest_validities;
    size_t* all_highest_validities;
    long double* all_mean_validities;
    if (CLUSTER_METADATA.rank == 0) {
        // on the master only, allocate memory for the validity arrays
        all_lowest_validities = calloc(
            CLUSTER_METADATA.world_size, sizeof(size_t)
        );
        all_highest_validities = calloc(
            CLUSTER_METADATA.world_size, sizeof(size_t)
        );
        all_mean_validities = calloc(
            CLUSTER_METADATA.world_size, sizeof(long double)
        );
        if (
            all_lowest_validities == NULL ||
            all_highest_validities == NULL ||
            all_mean_validities == NULL
        ) {
            abort(); // cheap error-handling
        }
    }
    // gather lowest validity, highest validity and mean validity
    MPI_Gather(
        &statistics->lowest_validity, 1, MPI_UINT64_T,
        all_lowest_validities, 1, MPI_UINT64_T,
        0, MPI_COMM_WORLD
    );
    MPI_Gather(
        &statistics->highest_validity, 1, MPI_UINT64_T,
        all_highest_validities, 1, MPI_UINT64_T,
        0, MPI_COMM_WORLD
    );
    MPI_Gather(
        &statistics->mean_validity, 1, MPI_LONG_DOUBLE,
        all_mean_validities, 1, MPI_LONG_DOUBLE,
        0, MPI_COMM_WORLD
    );
    // master collates all statistics and writes to file if required
    if (CLUSTER_METADATA.rank == 0) {
        init_statistics(all_statistics, statistics->bits);
        for (size_t i = 0; i < CLUSTER_METADATA.world_size; i++) {
            if (all_lowest_validities[i] < all_statistics->lowest_validity) {
                all_statistics->lowest_validity = all_lowest_validities[i];
            }
            if (all_highest_validities[i] > all_statistics->highest_validity) {
                all_statistics->highest_validity = all_highest_validities[i];
            }
            all_statistics->mean_validity += all_mean_validities[i];
        }
        finalise_statistics(all_statistics);
        cluster_printf(
            "Bits: %2" PRIuFAST8 "\tLowest: %4zu\tHighest: %4zu\tMean: %10.6LF\n",
            all_statistics->bits,
            all_statistics->lowest_validity,
            all_statistics->highest_validity,
            all_statistics->mean_validity
        );
        // free allocated memory
        free(all_lowest_validities);
        free(all_highest_validities);
        free(all_mean_validities);
    }
}

static void deallocate_problem_set(ProblemSet* problem_set) {
    if (problem_set->problem_solutions != NULL) {
        // deallocate all solutions
        for (size_t i = 0; i < problem_set->count; i++) {
            if (problem_set->problem_solutions[i].solutions != NULL) {
                free(problem_set->problem_solutions[i].solutions);
                problem_set->problem_solutions[i].solutions = NULL;
            }
        }
        // finally, deallocate the main pointer
        free(problem_set->problem_solutions);
        problem_set->problem_solutions = NULL;
    }
}

// XXX: old stuff that might not be resued ahead

// static bool generate_problems_and_cache_solutions(
//     ProblemSet* problem_set,
//     ProblemSize smallest_problem,
//     ProblemSize largest_problem,
//     ProblemStatistics* statistics
// ) {
//     // generate the first problem cache
//     if (
//         !generate_new_problem_solutions_cache(
//             problem_set,
//             smallest_problem,
//             statistics != NULL ? &statistics[0] : NULL
//         )
//     ) {
//         // deallocate any memory and return failure
//         deallocate_problem_set(problem_set);
//         return false;
//     }
//     // for each successive problem size after first (if any)
//     size_t problems_to_solve = count_problems_in_range(
//         smallest_problem, largest_problem
//     );
//     for (size_t i = 1U; i < problems_to_solve; i++) {
//         // generate subsequent cache levels from the previous ones, iteratively
//         if (
//             !generate_next_problem_solutions_from_current(
//                 problem_set,
//                 statistics != NULL ? &statistics[i] : NULL
//             )
//         ) {
//             // deallocate any memory and return failure
//             deallocate_problem_set(problem_set);
//             return false;
//         }
//     }
//     return true;
// }

// static bool search_remaining_problem_solutions(
//     ProblemSize smallest_problem,
//     ProblemSize largest_problem,
//     const ProblemSet* problem_cache,
//     ProblemStatistics* problem_statistics
// ) {
//     // we need to work out how many problems we have left to solve w/r to cache
//     size_t problems_count = count_problems_in_range(
//         smallest_problem, largest_problem
//     );
//     size_t problems_left = problem_cache->bits < smallest_problem ?
//         problems_count :
//         count_problems_in_range(problem_cache->bits + 1, largest_problem);
//     for (size_t i = problems_count - problems_left; i < problems_count; i++) {
//         // find solutions for the particular problem
//         find_solutions_for_problem(
//             smallest_problem + i, problem_cache, &problem_statistics[i]
//         );
//     }
//     return true; // hmmm, if it can't error, why did I make it return bool?
// }

// XXX: end of old stuff that might not be reused

static size_t predict_number_of_valid_solutions(ProblemSize problem_size) {
    // problem sizes below 6 bits do not follow the trend
    if (problem_size < 6) return two_to_the_power_of(problem_size * 2U);
    return (size_t)ceill( // round up for a conservative estimate
        two_to_the_power_of(problem_size) * mean_validity(problem_size)
    );
}

static uintmax_t two_to_the_power_of(uint_fast8_t power) {
    return 1U << power;
}

static long double mean_validity(ProblemSize problem_size) {
    // A-B-exponential regression means that value is `A * B^problem_size`
    return (
        MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_A *
        powl(MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_B, problem_size)
    );
}

static void init_statistics(ProblemStatistics* statistics, ProblemSize bits) {
    if (statistics != NULL) {
        // set the problem size bits
        statistics->bits = bits;
        // for our tracking of min validity to work, init it to max size_t
        statistics->lowest_validity = SIZE_MAX;
    }
}

static void update_statistics(
    ProblemStatistics* statistics, size_t solutions_count
) {
    // update statistics if not NULL
    if (statistics != NULL) {
        if (solutions_count < statistics->lowest_validity) {
            statistics->lowest_validity = solutions_count;
        }
        if (solutions_count > statistics->highest_validity) {
            statistics->highest_validity = solutions_count;
        }
        // mean validity is cumulative until we finish at which point divide
        statistics->mean_validity += solutions_count;
    }
}

static void finalise_statistics(ProblemStatistics* statistics) {
    if (statistics != NULL) {
        // divide cumulative mean validity by problems count
        statistics->mean_validity /= two_to_the_power_of(statistics->bits);
    }
}

static bool allocate_problem_set(
    ProblemSet* problem_set, ProblemSize problem_size, size_t problems_count
) {
    // first off, try to allocate memory for all the problems
    problem_set->problem_solutions = calloc(problems_count, sizeof(SolutionSet));
    // catch calloc() failure
    if (problem_set->problem_solutions == NULL) return false;
    // now that we have allocated, set the number of problems and bit size
    problem_set->count = problems_count;
    problem_set->bits = problem_size;
    return true;
}

static void get_problem_share(
    ProblemSize problem_size, Problem* start, Problem* end
) {
    size_t problem_count = two_to_the_power_of(problem_size);
    *start = (Problem)floorl(
        (long double)problem_count /
        CLUSTER_METADATA.world_size *
        CLUSTER_METADATA.rank
    );
    *end = (Problem)floorl(
        (long double)problem_count /
        CLUSTER_METADATA.world_size *
        (CLUSTER_METADATA.rank + 1U)
    );
}

static void integer_to_bit_string(
    ProblemSize size, RepresentationBase source, bool dest[size]
) {
    // NOTE: we handle integers big-endian, but only handle the x lowest bits
    for (ProblemSize i = 0; i < size; i++) {
        RepresentationBase mask = 1U << (size - i - 1);
        if ((source & mask) != 0) {
            dest[i] = true;
        } else {
            dest[i] = false;
        }
    }
}

static sxbp_CollisionResult solution_is_valid_for_problem(
    ProblemSize size, Problem problem, Solution solution, bool detect_terminals
) {
    // yes, these are C99 variable-length arrays
    bool problem_bits[size];
    bool solution_bits[size];
    // get bits of problem and solution
    integer_to_bit_string(size, problem, problem_bits);
    integer_to_bit_string(size, solution, solution_bits);
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
    for (ProblemSize i = 0; i < size; i++) {
        // if bit is 1, turn right, otherwise, turn left
        if (solution_bits[i]) {
            current_direction = (current_direction - 1) % 4;
        } else {
            current_direction = (current_direction + 1) % 4;
        }
        figure.lines[i + 1].length = problem_bits[i] ? 2 : 1;
        figure.lines[i + 1].direction = current_direction;
    }
    // check if figure collides and store result
    sxbp_CollisionResult collision_result = SXBP_COLLISION_RESULT_CONTINUES;
    if (
        !sxbp_success(
            sxbp_figure_collides(&figure, &collision_result, detect_terminals)
        )
    ) {
        abort(); // XXX: Cheap allocation failure exit!
    }

    // XXX: DEBUGGING CODE FOR PRINTING EVERY SINGLE TESTED CANDIDATE
    // if (collision_result == SXBP_COLLISION_RESULT_TERMINATES){
    //     sxbp_Bitmap bitmap = sxbp_blank_bitmap();
    //     sxbp_render_figure_to_bitmap(&figure, &bitmap);
    //     sxbp_print_bitmap(&bitmap, stdout);
    //     sxbp_free_bitmap(&bitmap);
    //     getchar();
    // }

    // free memory for figure
    sxbp_free_figure(&figure);
    // return result
    return collision_result;
}

static bool add_solution_to_solution_set(
    SolutionSet* solution_set, Solution s
) {
    solution_set->solutions[solution_set->count] = s;
    solution_set->count++;
    // make sure there's always memory for at least one more item
    if (solution_set->count == solution_set->allocated_size) {
        // allocate extra memory
        size_t new_size = solution_set->count + SOLUTION_SET_OVER_ALLOCATE_AMOUNT;
        void* set = realloc(
            solution_set->solutions, new_size * sizeof(Solution)
        );
        if (set == NULL) {
            // memory allocation failure
            return false;
        } else {
            // store the new pointer
            solution_set->solutions = set;
            // update allocated size
            solution_set->allocated_size = new_size;
        }
    }
    // success!
    return true;
}

static bool shrink_solution_set(SolutionSet* solution_set) {
    /*
     * because realloc()'ing zero bytes will free the memory, always allocate at
     * least 1 item, even if there are none for some reason
     * NOTE: this should never actually be the case, as that would mean a
     * problem size has been reached for which it can't be gauranteed that there
     * will always be at least one solution.
     */
    size_t shrink_size = solution_set->count > 0U ? solution_set->count : 1U;
    // resize (should be shrinking) down to the exact size needed to store items
    void* set = realloc(
        solution_set->solutions, shrink_size * sizeof(Solution)
    );
    if (set == NULL) {
        // memory allocation failure
        return false;
    } else {
        // store the new pointer
        solution_set->solutions = set;
        // update allocated size
        solution_set->allocated_size = shrink_size;
        return true;
    }
}

static bool generate_next_problem_solutions_from_current(
    ProblemSet* problem_set, ProblemStatistics* statistics
) {
    ProblemSize current_problem = problem_set->bits + 1U;
    /*
     * copy the problem set passed to us --this doesn't copy pointers but that's
     * ok because we don't want to copy the dynamically allocated sections, but
     * we do need to retain the pointers so we can free them after as we shall
     * replace the original problem set object with the new one
     */
    ProblemSet old_set = *problem_set;
    // reset the members of the original
    *problem_set = (ProblemSet){0};
    // create new data structure to contain problem sets
    if (
        !allocate_problem_set(
            problem_set, current_problem, old_set.count * 2U
        )
    ) {
        // allocation failure --deallocate old set before returning
        deallocate_problem_set(&old_set);
        return false;
    }
    // init statistics
    init_statistics(statistics, problem_set->bits);
    // generate and test new problems and new valid solutions from the old ones
    for (size_t i = 0; i < old_set.count; i++) {
        // copy the problem but shift it one bit left --we are extending it
        Problem old_problem = old_set.problem_solutions[i].problem << 1;
        /*
         * deposit two copies of old problem number into array, one with a zero
         * appended and another with a one appended
         */
        for (uint_fast8_t j = 0; j < 2; j++) {
            /*
             * we deposit problems appended with zero at the front and deposit
             * problems appended with one starting halfway through the new set
             */
            size_t k = i + (old_set.count * j);
            problem_set->problem_solutions[k].problem = old_problem | j;
            // allocate memory for the solutions to this problem
            size_t estimated_solutions = predict_number_of_valid_solutions(
                problem_set->bits
            );
            problem_set->problem_solutions[k].solutions = calloc(
                estimated_solutions, sizeof(Solution)
            );
            if (problem_set->problem_solutions[k].solutions == NULL) {
                // allocation failure --deallocate old set before returning
                deallocate_problem_set(&old_set);
                return false;
            }
            // set number allocated for memory book-keeping purposes
            problem_set->problem_solutions[k].allocated_size = estimated_solutions;
            /*
             * we now need to iterate over all the valid solutions to the
             * original problem, create extended versions with zero and one
             * appended and test these against the new problem
             */
            for (size_t l = 0; l < old_set.problem_solutions[i].count; l++) {
                // as with the problem, shift one bit left to extend
                Solution old_solution = old_set.problem_solutions[i]
                                                .solutions[l] << 1;
                for (uint_fast8_t m = 0; m < 2; m++) {
                    Problem p = problem_set->problem_solutions[k].problem;
                    // append the zero or one and validate the new solution
                    Solution s = old_solution | m;
                    switch (
                        solution_is_valid_for_problem(
                            problem_set->bits, p, s, true
                        )
                    ) {
                    case SXBP_COLLISION_RESULT_COLLIDES:
                        break;
                    case SXBP_COLLISION_RESULT_CONTINUES:
                        if (
                            !add_solution_to_solution_set(
                                &problem_set->problem_solutions[k], s
                            )
                        ) {
                            // allocation failure --deallocate before returning
                            deallocate_problem_set(&old_set);
                            return false;
                        }
                    case SXBP_COLLISION_RESULT_TERMINATES:
                        problem_set->problem_solutions[k].all_count++;
                        break;
                    }
                }
            }
            // shrink solution set down to waste less memory
            if (!shrink_solution_set(&problem_set->problem_solutions[k])) {
                // allocation failure --deallocate old set before returning
                deallocate_problem_set(&old_set);
                return false;
            }
            // update statistics
            update_statistics(
                statistics, problem_set->problem_solutions[k].all_count
            );
        }
    }
    // free the original version before exiting (otherwise memory will leak)
    deallocate_problem_set(&old_set);
    return true;
}

static bool find_solutions_for_problem(
    ProblemSize size,
    const ProblemSet* problem_cache,
    ProblemStatistics* statistics
) {
    cluster_printf("Searching %2" PRIuFAST8 "...", size);
    fflush(stdout);
    // initialise the statistics first
    init_statistics(statistics, size);
    // work out how many bits we have to shift/mask
    ProblemSize shift = size - problem_cache->bits;
    // for each problem in the cache (this is our bitmask)
    for (size_t i = 0; i < problem_cache->count; i++) {
        // retrieve the problem and shift it up to turn it into a mask
        Problem p_mask = problem_cache->problem_solutions[i].problem << shift;
        // now, we iterate the non-mask bits to generate individual problems
        for (size_t j = 0; j < two_to_the_power_of(shift); j++) {
            Problem p = p_mask | j; // combine mask and tail
            size_t solutions_found = 0; // how many solutions for this problem
            // iterate all the solutions for this problem
            for (
                size_t k = 0; k < problem_cache->problem_solutions[i].count; k++
            ) {
                // get the solution mask
                Solution s_mask = problem_cache->problem_solutions[i]
                                                .solutions[k] << shift;
                // now, we iterate the non-mask bits of the solution
                for (size_t l = 0; l < two_to_the_power_of(shift); l++) {
                    Solution s = s_mask | l; // combine mask and tail
                    // now finally, test this solution against the problem
                    if (
                        solution_is_valid_for_problem(size, p, s, false)
                        != SXBP_COLLISION_RESULT_COLLIDES
                    ) {
                        solutions_found++;
                    }
                }
            }
            // now update the statistics
            update_statistics(statistics, solutions_found);
        }
    }
    finalise_statistics(statistics);
    cluster_printf("SEARCHED\n");
    return true; // hmmm, if it can't error, why did I make it return bool?
}
