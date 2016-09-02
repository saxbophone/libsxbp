#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <argtable2.h>

#include "sxp.h"


#ifdef __cplusplus
extern "C"{
#endif

// returns size of file associated with given file handle
size_t
file_size(FILE * file_handle) {
    // seek to end
    fseek(file_handle, 0L, SEEK_END);
    // get size
    size_t file_size = ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0L, SEEK_SET);
    return file_size;
}

/*
 * returns file handle of input file, given a file path string
 * path can be NULL and if it is then it returns stdin
 */
FILE *
get_input_file(char * path) {
    return (path != NULL) ? fopen(path, "rb") : stdin;
}

/*
 * returns file handle of output file, given a file path string
 * path can be NULL and if it is then it returns stdout
 */
FILE *
get_output_file(char * path) {
    return (path != NULL) ? fopen(path, "wb") : stdout;
}

// main
int
main(int argc, char * argv[]) {
    // build argtable struct for parsing command-line arguments
    // show help
    struct arg_lit * help = arg_lit0("h","help", "show this help and exit");
    // show version
    struct arg_lit * version = arg_lit0("v", "version", "show version");
    // flag for if we want to prepare a spiral
    struct arg_lit * prepare = arg_lit0(
        "p", "prepare",
        "prepare a spiral with directions attained from processing the binary "
        "data from the input file"
    );
    // flag for if we want to generate the solution for a spiral's line lengths
    struct arg_lit * generate = arg_lit0(
        "g", "generate",
        "generate the correct lengths of all the lines in the input spiral"
    );
    // flag for if we want to render a spiral to imagee
    struct arg_lit * render = arg_lit0(
        "r", "render", "render the input spiral to an image"
    );
    // input file path option
    struct arg_file * input = arg_file0(
        "i", "input", "<path>", "input file path (uses stdin if not given)"
    );
    // output file path option
    struct arg_file * output = arg_file0(
        "o", "output", "<path>", "output file path (uses stdout if not given)"
    );
    // argtable boilerplate
    struct arg_end * end = arg_end(20);
    void * argtable[] = {
        help, version, prepare, generate, render, input, output, end,
    };
    const char * program_name = "sxp";
    // check argtable members were allocated successfully
    if(arg_nullcheck(argtable) != 0) {
        // NULL entries were detected, so some allocations failed
        fprintf(stderr, "%s\n", "Bad Doo-doo!");
        return 2;
    }
    // parse arguments
    int count_errors = arg_parse(argc,argv,argtable);
    if(help->count > 0) {
        // first check if we asked for the help option
        printf("Usage: %s", program_name);
        arg_print_syntax(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        return 0;
    } else if(count_errors > 0) {
        // next, if parser returned any errors, display them then exit
        arg_print_errors(stdout, end, program_name);
        return 1;
    }
    // if we asked for the version, show it
    if(version->count > 0) {
        printf("Saxbospiral " SAXBOSPIRAL_VERSION_STRING "\n");
    }
}

#ifdef __cplusplus
} // extern "C"
#endif
