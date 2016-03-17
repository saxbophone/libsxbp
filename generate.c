#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"


// prints out usage information to a given FILE handle
static void
show_usage(FILE * stream) {
    fprintf(
        stream, "%s\n%s\n",
        "Usage:",
        "./generate <input_file.saxbospiral> <optional_output_file.saxbospiral>"
    );
}

// prints out error message when unable to read given file
static void
file_open_error(char * filename) {
    fprintf(stderr, "Cannot open file '%s'\n", filename);
}

// returns size of file associated with given file handle
static size_t
file_size(FILE * file_handle) {
    // seek to end
    fseek(file_handle, 0L, SEEK_END);
    // get size
    size_t file_size = ftell(file_handle);
    // seek to start again
    fseek(file_handle, 0L, SEEK_SET);
    return file_size;
}

int
main(int argc, char * argv[]) {
    if(argc < 2) {
        // not enough arguments, print usage information
        show_usage(stderr);
        return 1;
    } else {
        // no program implementation, so return 1 anyway!
        return 1;
    }
}
