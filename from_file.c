#include <stdio.h>
#include <stdlib.h>

#include "bspirals.h"


int
main(int argc, char const * argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Too few arguments given!\n");
        return 1;
    } else if(argc > 2) {
        fprintf(stderr, "Too many arguments given!\n");
        return 1;
    } else {
        // try and read filename given as command-line argument
        FILE * file_handle = fopen(argv[1], "rb");
        // catch open error
        if(file_handle == NULL) {
            fprintf(stderr, "Unable to read file '%s'\n", argv[1]);
            return 1;
        } else {
            printf("Reading file '%s'\n", argv[1]);
            // get file size by doing these steps:
            // seek to end
            fseek(file_handle, 0L, SEEK_END);
            // get size
            size_t file_size = ftell(file_handle);
            // seek to start again
            fseek(file_handle, 0L, SEEK_SET);
            // print message to let user know how many bytes and lines these will create
            printf("File size is %zi bytes - This generates a spiral with %zi lines\n", file_size, file_size*8);
            // TODO: Read in file data to a buffer
            // close file handle, because we're polite :)
            fclose(file_handle);
            // TODO: Generate the spiral from the file data
            return 0;
        }
    }
}
