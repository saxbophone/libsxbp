#ifndef SAXBOPHONE_SAXBOSPIRAL_SXP_H
#define SAXBOPHONE_SAXBOSPIRAL_SXP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "saxbospiral/saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// returns size of file associated with given file handle
size_t get_file_size(FILE * file_handle);

/*
 * given an open file handle and a buffer, read the file contents into buffer
 * returns true on success and false on failure.
 */
bool file_to_buffer(FILE * file_handle, buffer_t * buffer);

/*
 * given a buffer struct and an open file handle, writes the buffer contents
 * to the file.
 * returns true on success and false on failure.
 */
bool buffer_to_file(buffer_t * buffer, FILE * file_handle);

/*
 * function responsible for actually doing the main work, called by main with
 * options configured via command-line.
 * returns true on success, false on failure.
 */
bool run(
    bool prepare, bool generate, bool render, bool perfect,
    int perfect_threshold, int line_limit, int total_lines,
    const char * input_file_path, const char * output_file_path
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
