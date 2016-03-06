#include <stdio.h>
#include <stdint.h>
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
            printf(
                "File size is %zi bytes - This generates a spiral with %zi lines\n",
                file_size, file_size*8
            );
            // allocate memory buffer to store file data
            uint8_t * buffer = malloc(file_size);
            // read in file data to buffer
            size_t bytes_read = fread(buffer, 1, file_size, file_handle);
            // close file handle, because we're polite :)
            fclose(file_handle);
            // check we read in the correct number of bytes (whole file)
            if(bytes_read == file_size) {
                printf("Preparing Spiral structure from file data...\n");
                // prepare spiral struct from file data
                spiral_t spiral = init_spiral(buffer, file_size);
                printf("Plotting Spiral (THIS MAY TAKE A VERY LONG TIME)...\n");
                // generate the spiral from the file data
                spiral = plot_spiral(spiral);
                // print out a representation of the final spiral
                printf("Directions:\n[");
                for(size_t i = 0; i < spiral.size; i++) {
                    printf("%i, ", spiral.lines[i].direction);
                }
                printf("]\nLengths:\n[");
                for(size_t i = 0; i < spiral.size; i++) {
                    printf("%i, ", spiral.lines[i].length);
                }
                printf("]\n");
                return 0;
            } else {
                fprintf(
                    stderr, "Expected to read %zi bytes, actually read %zi\n",
                    file_size, bytes_read
                );
                return 1;
            }
        }
    }
}
