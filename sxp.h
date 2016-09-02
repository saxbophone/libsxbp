#ifndef SAXBOPHONE_SAXBOSPIRAL_SXP_H
#define SAXBOPHONE_SAXBOSPIRAL_SXP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C"{
#endif

// returns size of file associated with given file handle
size_t
file_size(FILE * file_handle);

/*
 * returns file handle of input file, given a file path string
 * path can be NULL and if it is then it returns stdin
 */
FILE *
get_input_file(char * path);

/*
 * returns file handle of output file, given a file path string
 * path can be NULL and if it is then it returns stdout
 */
FILE *
get_output_file(char * path);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
