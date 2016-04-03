#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <png.h>

#include "saxbospiral.h"


typedef struct bitmap_t {
    uint64_t width;
    uint64_t height;
    bool ** pixels;
} bitmap_t;

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

// given a spiral struct with co-ords in it's cache, find and return the co-ords
// for the corners of the square needed to contain the points.
static co_ord_array_t
get_bounds(spiral_t spiral) {
    int64_t min_x, min_y, max_x, max_y;
    for(size_t i = 0; i < spiral.co_ord_cache.co_ords.size; i++) {
        if(spiral.co_ord_cache.co_ords.items[i].x < min_x) {
            min_x = spiral.co_ord_cache.co_ords.items[i].x;
        }
        if(spiral.co_ord_cache.co_ords.items[i].y < min_y) {
            min_y = spiral.co_ord_cache.co_ords.items[i].y;
        }
        if(spiral.co_ord_cache.co_ords.items[i].x > min_x) {
            max_x = spiral.co_ord_cache.co_ords.items[i].x;
        }
        if(spiral.co_ord_cache.co_ords.items[i].y > min_y) {
            max_y = spiral.co_ord_cache.co_ords.items[i].y;
        }
    }
    // initialise output struct
    co_ord_array_t bounds = {
        .size = 2,
        .items = calloc(sizeof(co_ord_t), 2),
    };
    // write bounds to struct
    bounds.items[0].x = min_x;
    bounds.items[0].y = min_y;
    bounds.items[1].x = max_x;
    bounds.items[1].y = max_y;
    return bounds;
}

// given a spiral struct, returns a bitmap_t representing a monochromatic image
// of the rendered spiral
static bitmap_t
render_spiral(spiral_t spiral) {
    // plot co-ords of spiral into it's cache
    cache_spiral_points(&spiral, spiral.size);
    // get the min and max bounds of the spiral's co-ords
    co_ord_array_t bounds = get_bounds(spiral);
    printf(
        "(%li, %li), (%li, %li)\n",
        bounds.items[0].x, bounds.items[0].y,
        bounds.items[1].x, bounds.items[1].y
    );
    // get the normalisation vector needed to make all values unsigned
    tuple_t normalisation_vector = {
        .x = -bounds.items[0].x,
        .y = -bounds.items[0].y,
    };
    // get co-ords of top left and bottom right corners, as unsigned
    co_ord_t top_left = {
        .x = 0,
        .y = 0,
    };
    co_ord_t bottom_right = {
        .x = bounds.items[1].x + normalisation_vector.x,
        .y = bounds.items[1].y + normalisation_vector.y,
    };
    printf(
        "(%li, %li), (%li, %li)\n",
        top_left.x, top_left.y,
        bottom_right.x, bottom_right.y
    );
    // initialise output bitmap
    bitmap_t output = {
        .width = (bottom_right.x * 2) + 2,
        .height = (bottom_right.y * 2) + 2,
    };
    // allocate dynamic memory - 2D array of bools
    output.pixels = malloc(output.width * sizeof(bool*));
    for(size_t i = 0; i < output.width; i++) {
        output.pixels[i] = malloc(output.height * sizeof(bool));
    }
    return output;
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
        // try and open output file for writing
        FILE * output_file_handle = fopen(output_file_path, "wb");
        if(output_file_handle == NULL) {
            // handle file open errors
            file_open_error(output_file_path);
            return 1;
        }
        printf("Saving rendered image to output file... ");
        // now write PNG image data to file with libpng
        // TODO: Implement this step!
        // #error Image rendering not implemented
        printf("[DONE]\n");
        // close output file handle
        fclose(output_file_handle);
        return 0;
    }
}
