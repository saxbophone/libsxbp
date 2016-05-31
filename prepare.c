#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral/saxbospiral.h"
#include "saxbospiral/initialise.h"
#include "saxbospiral/serialise.h"


// prints out usage information to a given FILE handle
static void
show_usage(FILE * stream) {
    fprintf(
        stream, "%s\n%s\n",
        "Usage:",
        "./prepare <data_file> <spiral_file.saxbospiral>"
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
    if(argc < 3) {
        // not enough arguments, print usage information
        show_usage(stderr);
        return 1; 
    } else {
        // collect command line arguments
        char * read_file_path = argv[1];
        char * write_file_path = argv[2];
        // open file handles
        FILE * read_file_handle = fopen(read_file_path, "rb");
        FILE * write_file_handle = fopen(write_file_path, "wb");
        // check that read file can be read, and write file can be written
        if(read_file_handle == NULL) {
            file_open_error(read_file_path);
        }
        if(write_file_handle == NULL) {
            file_open_error(write_file_path);
        }
        if((read_file_handle == NULL) || (write_file_handle == NULL)) {
            // couldn't read or write, fail
            return 1;
        } else {
            // can read and write, all good
            // get input file size
            size_t read_file_size = file_size(read_file_handle);
            // print message to let user know how many bytes and lines these
            // will create
            printf(
                "File size is %zi bytes - This generates a spiral with %zi lines\n",
                read_file_size, read_file_size*8
            );
            // allocate memory buffer
            buffer_t input = {
                .size = read_file_size, .bytes = calloc(1, read_file_size),
            };
            // read in file data to buffer
            size_t bytes_read = fread(input.bytes, 1, read_file_size, read_file_handle);
            // close input file handle, because we're polite :)
            fclose(read_file_handle);
            // check we read in the correct number of bytes (whole file)
            if(bytes_read != read_file_size) {
                fprintf(stderr, "%s\n", "ERROR - Didn't read whole file.");
                return 1;
            }
            printf("Converting file data into spiral directions... ");
            fflush(stdout);
            // prepare spiral struct from file data
            spiral_t spiral = init_spiral(input);
            // free memory
            free(input.bytes);
            printf("[DONE]\n");
            printf("Serialising spiral data to storage format... ");
            fflush(stdout);
            // dump spiral to buffer data
            buffer_t output = dump_spiral(spiral);
            printf("[DONE]\n");
            printf("Saving spiral file... ");
            fflush(stdout);
            // save buffer data
            size_t bytes_written = fwrite(
                output.bytes, 1, output.size, write_file_handle
            );
            if(bytes_written != output.size) {
                fprintf(stderr, "%s\n", "ERROR - Didn't write whole file.");
                return 1;
            }
            printf("[DONE]\n");
            // close output file handle
            fclose(write_file_handle);
            return 0;
        }
    }
}
