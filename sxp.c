#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    return 1;
}

#ifdef __cplusplus
} // extern "C"
#endif
