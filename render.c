#include <stdio.h>
#include <stdlib.h>

#include "saxbospiral/saxbospiral.h"
#include "saxbospiral/render_backends/png_backend.h"
#include "saxbospiral/render.h"
#include "saxbospiral/serialise.h"


// prints out usage information to a given FILE handle
static void
show_usage(FILE * stream) {
    fprintf(
        stream, "%s\n%s\n",
        "Usage:",
        "./render <input_file.saxbospiral> <output_file.png>"
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
        // otherwise, read in arguments
        char * input_file_path = argv[1];
        char * output_file_path = argv[2];
        printf(
            "Attempting to load spiral from input file: '%s'... ", input_file_path
        );
        fflush(stdout);
        // try and open input file
        FILE * input_file_handle = fopen(input_file_path, "rb");
        if(input_file_handle == NULL) {
            // handle file open errors
            file_open_error(input_file_path);
            return 1;
        }
        // prepare to read input file
        size_t input_file_size = file_size(input_file_handle);
        // allocate memory buffer
        buffer_t input_buffer = {
            .size = input_file_size, .bytes = calloc(1, input_file_size),
        };
        // read in file data to buffer
        size_t bytes_read = fread(
            input_buffer.bytes, 1, input_file_size, input_file_handle
        );
        // close input file handle
        fclose(input_file_handle);
        // check we read in the correct number of bytes (whole file)
        if(bytes_read != input_file_size) {
            fprintf(stderr, "%s\n", "ERROR - Didn't read whole file.");
            return 1;
        }
        // try and load a spiral struct from input file
        spiral_t spiral = load_spiral(input_buffer);
        // free dynamically allocated memory for input buffer now
        free(input_buffer.bytes);
        // the spiral size will be set to 0 if buffer data was invalid
        if(spiral.size == 0) {
            fprintf(
                stderr,
                "ERROR - File data was invalid "
                "(not a format accepted by SAXBOSPIRAL v%i.%i.%i)\n",
                VERSION.major, VERSION.minor, VERSION.patch
            );
            return 1;
        }
        printf("[DONE]\n");
        printf("Rendering image... ");
        fflush(stdout);
        // if file was ok, then render the spiral to a monochrome bitmap
        bitmap_t image = render_spiral(spiral);
        printf("[DONE]\n");
        printf("Building PNG File... ");
        // now write PNG image data to buffer with libpng
        buffer_t output_buffer = write_png_image(image);
        printf("[DONE]\n");
        printf("Saving rendered image to output file... ");
        // try and open output file for writing
        FILE * output_file_handle = fopen(output_file_path, "wb");
        if(output_file_handle == NULL) {
            // handle file open errors
            file_open_error(output_file_path);
            return 1;
        }
        // save buffer data
        size_t bytes_written = fwrite(
            output_buffer.bytes, 1, output_buffer.size, output_file_handle
        );
        if(bytes_written != output_buffer.size) {
            fprintf(stderr, "%s\n", "ERROR - Didn't write whole file.");
            return 1;
        }
        printf("[DONE]\n");
        // free data in buffer
        free(output_buffer.bytes);
        output_buffer.size = 0;
        // close output file handle
        fclose(output_file_handle);
        return 0;
    }
}
