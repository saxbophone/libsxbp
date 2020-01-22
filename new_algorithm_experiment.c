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
#include <time.h>

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

// private data structure for tracking program self-timing state
typedef struct TimingData {
    long double last_estimate; // the last time estimate we made
    long double seconds_elapsed; // the amount of time elapsed in the current timing period
    time_t start_time;
} TimingData;

typedef uint_fast8_t ProblemSize;

typedef struct CommandLineOptions {
    char* csv_file_name; // name of file to write out CSV statistics to
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

// timing output constants
static const long double MINUTE_SECONDS = 60.0L;
static const long double HOUR_SECONDS = 60.0L * 60.0L;
static const long double DAY_SECONDS = 60.0L * 60.0L * 24.0L;
static const long double WEEK_SECONDS = 60.0L * 60.0L * 24.0L * 7.0L;
static const long double MONTH_SECONDS = 60.0L * 60.0L * 24.0L * 30.44L;
static const long double YEAR_SECONDS = 60.0L * 60.0L * 24.0L * 365.2425L;

// these constants are calculated from A-B-exponential regression on search data
static const double MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_A = 1.579354269601670;
static const double MEAN_VALIDITY_A_B_EXPONENTIAL_REGRESSION_CURVE_B = 0.832119117570787;
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
 * writes out the CSV header for the statistics of this search
 * uses the given options object to defermine which file to write this to
 * files are truncated on open
 * returns whether this succeeded or not
 */
static bool write_statistics_header(const CommandLineOptions* options);

// starts the stopwatch used for self-timing and completion estimates
static void stopwatch_start(TimingData* timing_data);

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

/*
 * writes out the given statistics object to CSV file, using options object to
 * determine which file this should be
 * returns whether this succeeded or not
 */
static bool write_out_statistics(
    const ProblemStatistics* statistics,
    const CommandLineOptions* options
);

// stops the stopwatch
static void stopwatch_stop(TimingData* timing_data);

// updates completion time estimate based on stopwatch and displays the estimate
static void update_and_print_completion_estimate(
    TimingData* timing_data,
    ProblemSize last_solved,
    ProblemSize last_to_solve
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

/*
 * given this process' problem cache, compares the amount of contents in it with
 * those of all other processes and redistributes among them if necessary
 */
static void rebalance_cache(ProblemSet* problem_set);

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
    // write CSV header out to file
    if (!write_statistics_header(&options)) abort(); // cheap error-handling
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
    // this variable is used only by master for time-completion estimates
    TimingData stopwatch = {0};
    if (CLUSTER_METADATA.rank == 0) stopwatch_start(&stopwatch);
    // generate the initial cache for our share of the problem
    if (
        !generate_initial_cache(
            &problem_cache,
            current_size,
            // only include pointer to statistics if we want stats
            current_size < options.start_problem_size ? NULL : &statistics
        )
    ) {
        abort(); // cheap error-handling
    }
    // XXX: Debug
    cluster_printf("Solutions = %.0Lf\n", statistics.mean_validity);
    // send our own part of the statistics to master if required
    if (current_size >= options.start_problem_size) {
        collect_statistics(&statistics, &all_statistics);
        // master writes statistics to file
        if (!write_out_statistics(&all_statistics, &options)) abort();
    }
    if (CLUSTER_METADATA.rank == 0) {
        stopwatch_stop(&stopwatch);
        // estimate completion of next and log this to console
        update_and_print_completion_estimate(
            &stopwatch, current_size, options.end_problem_size
        );
    }
    current_size++;
    // then until max problems are cached
    while (current_size <= cache_end_size) {
        // reset statistics
        statistics = (ProblemStatistics){0};
        all_statistics = (ProblemStatistics){0};
        if (CLUSTER_METADATA.rank == 0) stopwatch_start(&stopwatch);
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
        // XXX: Debug
        cluster_printf("Solutions = %.0Lf\n", statistics.mean_validity);
        // send our own part of the statistics to master if required
        if (current_size >= options.start_problem_size) {
            collect_statistics(&statistics, &all_statistics);
            // master writes statistics to file
            if (!write_out_statistics(&all_statistics, &options)) abort();
        }
        // rebalance cache among processes to keep work shared roughly evenly
        rebalance_cache(&problem_cache);
        if (CLUSTER_METADATA.rank == 0) {
            stopwatch_stop(&stopwatch);
            // estimate completion of next and log this to console
            update_and_print_completion_estimate(
                &stopwatch, current_size, options.end_problem_size
            );
        }
        current_size++;
    }
    // then until max problem size is searched:
    while (current_size <= options.end_problem_size) {
        // reset statistics
        statistics = (ProblemStatistics){0};
        all_statistics = (ProblemStatistics){0};
        if (CLUSTER_METADATA.rank == 0) stopwatch_start(&stopwatch);
        // search our share of the problem space and collect statistics
        find_solutions_for_problem(current_size, &problem_cache, &statistics);
        // XXX: Debug
        cluster_printf("Solutions = %.0Lf\n", statistics.mean_validity);
        // send our own part of the statistics to master if required
        if (current_size >= options.start_problem_size) {
            collect_statistics(&statistics, &all_statistics);
            // master writes statistics to file
            if (!write_out_statistics(&all_statistics, &options)) abort();
        }
        if (CLUSTER_METADATA.rank == 0) {
            stopwatch_stop(&stopwatch);
            // estimate completion of next and log this to console
            update_and_print_completion_estimate(
                &stopwatch, current_size, options.end_problem_size
            );
        }
        current_size++;
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

// returns the time in seconds, converted if needed to most convenient unit
static long double convenient_time_value(long double seconds);

// returns the most convenient way of describing a given time unit
static const char* convenient_time_unit(long double seconds);

/*
 * returns estimated completion time of search based on run time of last
 * completed factor and the number of factors left to complete
 */
static long double estimated_completion_time(
    long double latest_run_time,
    ProblemSize completed_factor,
    ProblemSize factors_left
);

// returns estimated completion time of the next problem size based on current
static long double estimated_completion_time_of_next(
    long double latest_run_time, ProblemSize completed_factor
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
 * transfers count number of problems from the problem cache of process with
 * rank *from* to rank *to*
 */
static void transfer_problems_between_processes(
    ProblemSet* problem_set, size_t from, size_t to, size_t count
);

/*
 * shrinks the given solution set by trim amount, deallocating and discarding
 * the latter items in the set
 */
static void trim_solution_set(ProblemSet* set, size_t trim_amount);

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
    if (argc < 5) exit(-1); // we need 4 additional arguments besides file name
    CommandLineOptions options = {0};
    options.csv_file_name = argv[1];
    options.start_problem_size = strtoul(argv[2], NULL, 10);
    options.end_problem_size = strtoul(argv[3], NULL, 10);
    options.max_ram_per_process = strtoul(argv[4], NULL, 10);
    if (
        options.start_problem_size == 0
        || options.end_problem_size == 0
        || options.max_ram_per_process == 0
    ) {
        exit(-1); // none of them can be zero
    }
    return options;
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

static bool write_statistics_header(const CommandLineOptions* options) {
    // only the master process does this
    if (CLUSTER_METADATA.rank == 0) {
        FILE* csv_file = fopen(options->csv_file_name, "w");
        if (csv_file == NULL) return false;
        fprintf(
            csv_file,
            "Timestamp,Bits,Problem Size,Lowest Validity,Highest Validity,Mean Validity\n"
        );
        fclose(csv_file);
    }
    return true;
}

static void stopwatch_start(TimingData* timing_data) {
    timing_data->start_time = time(NULL);
    timing_data->seconds_elapsed = 1.0L / 0.0L; // NAN
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
        // free allocated memory
        free(all_lowest_validities);
        free(all_highest_validities);
        free(all_mean_validities);
    }
}

static bool write_out_statistics(
    const ProblemStatistics* statistics,
    const CommandLineOptions* options
) {
    // only do this on the master process
    if (CLUSTER_METADATA.rank == 0) {
        // get ISO time string
        time_t now = time(NULL);
        char time_string[21];
        strftime(time_string, sizeof(time_string), "%FT%TZ", gmtime(&now));
        // open file for appending
        FILE* csv_file = fopen(options->csv_file_name, "a");
        if (csv_file == NULL) return false;
        // write out statistics row to CSV file
        fprintf(
            csv_file,
            "%s,%" PRIuFAST8 ",%" PRIuMAX ",%" PRIu64 ",%" PRIu64 ",%Lf\n",
            time_string,
            statistics->bits,
            two_to_the_power_of(statistics->bits),
            statistics->lowest_validity,
            statistics->highest_validity,
            statistics->mean_validity
        );
        fclose(csv_file);
    }
    return true;
}

static void stopwatch_stop(TimingData* timing_data) {
    time_t now = time(NULL);
    long double seconds_elapsed = difftime(now, timing_data->start_time);
    timing_data->seconds_elapsed = seconds_elapsed;
}

static void update_and_print_completion_estimate(
    TimingData* timing_data,
    ProblemSize last_solved,
    ProblemSize last_to_solve
) {
    time_t now = time(NULL);
    char time_string[21];
    strftime(time_string, sizeof(time_string), "%FT%TZ", gmtime(&now));
    // print error of estimate
    cluster_printf("============================= %s =============================\n", time_string);
    cluster_printf(
        "Solved problem size: %" PRIuFAST8
        " - Time taken:\t%Lf%s (%.2Lf%% of estimate)\n",
        last_solved,
        convenient_time_value(timing_data->seconds_elapsed),
        convenient_time_unit(timing_data->seconds_elapsed),
        ((timing_data->seconds_elapsed / timing_data->last_estimate) - 0.0L) * 100.0L
    );
    long double completion_estimate = estimated_completion_time(timing_data->seconds_elapsed, last_solved, last_to_solve - last_solved);
    cluster_printf(
        "Estimated time til completion:\t\t%Lf%s\n",
        convenient_time_value(completion_estimate),
        convenient_time_unit(completion_estimate)
    );
    if (last_solved < last_to_solve) {
        timing_data->last_estimate = estimated_completion_time(timing_data->seconds_elapsed, last_solved, 1);
        cluster_printf(
            "Estimated time til next solved:\t\t%Lf%s\n",
            convenient_time_value(timing_data->last_estimate),
            convenient_time_unit(timing_data->last_estimate)
        );
    }
    cluster_printf("================================================================================\n\n");
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

static long double convenient_time_value(long double seconds) {
    if (seconds < MINUTE_SECONDS) {
        return seconds;
    } else if (seconds < HOUR_SECONDS) {
        return seconds / MINUTE_SECONDS;
    } else if (seconds < DAY_SECONDS) {
        return seconds / HOUR_SECONDS;
    } else if (seconds < WEEK_SECONDS) {
        return seconds / DAY_SECONDS;
    } else if (seconds < MONTH_SECONDS) {
        return seconds / WEEK_SECONDS;
    } else if (seconds < YEAR_SECONDS) {
        return seconds / MONTH_SECONDS;
    } else {
        return seconds / YEAR_SECONDS;
    }
}

static const char* convenient_time_unit(long double seconds) {
    if (seconds < MINUTE_SECONDS) {
        return "s";
    } else if (seconds < HOUR_SECONDS) {
        return " mins";
    } else if (seconds < DAY_SECONDS) {
        return " hours";
    } else if (seconds < WEEK_SECONDS) {
        return " days";
    } else if (seconds < MONTH_SECONDS) {
        return " weeks";
    } else if (seconds < YEAR_SECONDS) {
        return " months";
    } else {
        return " years";
    }
}

static long double estimated_completion_time(
    long double latest_run_time,
    ProblemSize completed_factor,
    ProblemSize factors_left
) {
    long double estimate = 0.0L;
    long double last_estimated = latest_run_time;
    for (ProblemSize f = 0; f < factors_left; f++) {
        last_estimated = estimated_completion_time_of_next(
            last_estimated,
            completed_factor + f
        );
        estimate += last_estimated;
    }
    return estimate;
}

static long double estimated_completion_time_of_next(
    long double latest_run_time, ProblemSize completed_factor
) {
    ProblemSize next_factor = completed_factor + 1;
    return latest_run_time / (powl(4.0L, completed_factor) * completed_factor)
                           * (powl(4.0L, next_factor) * next_factor);
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

static void rebalance_cache(ProblemSet* problem_set) {
    // use MPI All-gather to allow all processes to know eachother's cache size
    uint64_t* cache_sizes = calloc(CLUSTER_METADATA.world_size, sizeof(uint64_t));
    if (cache_sizes == NULL) abort(); // cheap error-handling
    uint64_t* expected_cache_sizes = calloc(
        CLUSTER_METADATA.world_size, sizeof(uint64_t)
    );
    if (expected_cache_sizes == NULL) abort(); // cheap error-handling
    MPI_Allgather(
        &problem_set->count, 1, MPI_UINT64_T,
        cache_sizes, 1, MPI_UINT64_T,
        MPI_COMM_WORLD
    );
    // calculate how many problems each processor *should* have
    uint64_t our_expected;
    {
        Problem start, end;
        get_problem_share(problem_set->bits, &start, &end);
        our_expected = end - start;
    }
    MPI_Allgather(
        &our_expected, 1, MPI_UINT64_T,
        expected_cache_sizes, 1, MPI_UINT64_T,
        MPI_COMM_WORLD
    );
    // check each process in turn to see if it has too many elements in cache
    for (size_t i = 0; i < CLUSTER_METADATA.world_size; i++) {
        if (cache_sizes[i] > expected_cache_sizes[i]) {
            // it has too many elements, so now find a process to give them to
            for (size_t j = 0; j < CLUSTER_METADATA.world_size; j++) {
                if (i == j) continue; // never transfer items to ourself
                // we need to check both conditions again as it may change
                if (
                    cache_sizes[i] > expected_cache_sizes[i] &&
                    cache_sizes[j] < expected_cache_sizes[j]
                ) {
                    // this is how much extra there is
                    size_t overflow = cache_sizes[i] - expected_cache_sizes[i];
                    // this process has at least *some* space to accept extra
                    size_t underflow = expected_cache_sizes[j] - cache_sizes[j];
                    // transfer the smallest of overflow and underflow
                    size_t transfer = overflow < underflow ? overflow : underflow;
                    transfer_problems_between_processes(
                        problem_set, i, j, transfer
                    );
                    cache_sizes[i] -= transfer;
                    cache_sizes[j] += transfer;
                    /*
                     * not actually required, but mark a synchronisation point
                     * for programmer's benefit --this is useful because we can
                     * then guarantee that all processes are on the same page,
                     * although this might induce a small performance penalty
                     * and this should be reviewed
                     */
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
        }
    }
    // free memory
    free(cache_sizes);
    free(expected_cache_sizes);
    return;
}

static void transfer_problems_between_processes(
    ProblemSet* problem_set, size_t from, size_t to, size_t count
) {
    // check if we are either of *from* or *to*
    if (CLUSTER_METADATA.rank == from) {
        // we are the sender
        for (size_t c = 0; c < count; c++) {
            size_t buffer_size = 3U + problem_set->problem_solutions[problem_set->count - count + c].count;
            // cluster_printf("Send size %zu\n", buffer_size);
            // allocate memory for a buffer of 64-bit uints
            uint64_t* buffer = calloc(buffer_size, sizeof(uint64_t));
            if (buffer == NULL) abort(); // cheap error-handling
            // fill the buffer with the problem and solutions
            buffer[0] = problem_set->problem_solutions[problem_set->count - count + c].count;
            buffer[1] = problem_set->problem_solutions[problem_set->count - count + c].all_count;
            buffer[2] = problem_set->problem_solutions[problem_set->count - count + c].problem;
            for (size_t s = 0; s < problem_set->problem_solutions[problem_set->count - count + c].count; s++) {
                buffer[3 + s] = problem_set->problem_solutions[problem_set->count - count + c].solutions[s];
            }
            // send the buffer to the other process
            MPI_Send(buffer, buffer_size, MPI_UINT64_T, to, 0, MPI_COMM_WORLD);
            // free memory
            free(buffer);
        }
        // remove the *count* last items from this problem set
        trim_solution_set(problem_set, count);
    } else if (CLUSTER_METADATA.rank == to) {
        // we are the receiver
        // grow our problem set by *count* items
        problem_set->problem_solutions = realloc(
            problem_set->problem_solutions,
            (problem_set->count + count) * sizeof(SolutionSet)
        );
        if (problem_set->problem_solutions == NULL) abort();
        for (size_t c = 0; c < count; c++) {
            // Use MPI_Probe() to count how large the incoming buffer is
            MPI_Status status;
            MPI_Probe(from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int buffer_size = 0;
            MPI_Get_count(&status, MPI_UINT64_T, &buffer_size);
            // allocate memory for the incoming buffer and the problem set
            uint64_t* buffer = calloc((size_t)buffer_size, sizeof(uint64_t));
            if (buffer == NULL) abort(); // cheap error-handling
            // number of solutions is 3 less the total size due to other vars
            size_t solutions_count = (size_t)(buffer_size - 3);
            problem_set->problem_solutions[problem_set->count + c]
                        .solutions = calloc(solutions_count, sizeof(Solution));
            if (
                problem_set->problem_solutions[problem_set->count + c].solutions
                == NULL
            ) {
                abort(); // cheap error-handling
            }
            // Use MPI_Recv() to read into our buffer
            MPI_Recv(
                buffer, buffer_size, MPI_UINT64_T, from, MPI_ANY_TAG,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE
            );
            // cluster_printf("Receive size %i\n", buffer_size);
            // validation
            if (buffer[0] != solutions_count) abort();
            // Extract the data out of the buffer into our problem set
            problem_set->problem_solutions[problem_set->count + c]
                        .allocated_size = solutions_count;
            problem_set->problem_solutions[problem_set->count + c]
                        .count = solutions_count;
            problem_set->problem_solutions[problem_set->count + c]
                        .all_count = buffer[1];
            problem_set->problem_solutions[problem_set->count + c]
                        .problem = buffer[2];
            // extract out all the solutions
            for (size_t s = 0; s < solutions_count; s++) {
                problem_set->problem_solutions[problem_set->count + c]
                            .solutions[s] = buffer[3 + s];
            }
            // free memory
            free(buffer);
        }
        problem_set->count += count;
    }
    // otherwise, we are neither, so there's nothing to do
}

static void trim_solution_set(ProblemSet* set, size_t trim_amount) {
    for (size_t i = set->count - trim_amount; i < set->count; i++) {
        // deallocate memory
        free(set->problem_solutions[i].solutions);
    }
    set->count -= trim_amount;
    // now shrink the problem solutions
    set->problem_solutions = realloc(
        set->problem_solutions, set->count * sizeof(SolutionSet)
    );
}

static bool find_solutions_for_problem(
    ProblemSize size,
    const ProblemSet* problem_cache,
    ProblemStatistics* statistics
) {
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
    return true; // hmmm, if it can't error, why did I make it return bool?
}
