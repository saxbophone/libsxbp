#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <argtable2.h>

#include "sxp.h"
#include "saxbospiral/saxbospiral.h"
#include "saxbospiral/initialise.h"
#include "saxbospiral/solve.h"
#include "saxbospiral/serialise.h"
#include "saxbospiral/render.h"
#include "saxbospiral/render_backends/png_backend.h"



#ifdef __cplusplus
extern "C"{
#endif

// returns size of file associated with given file handle
size_t
get_file_size(FILE * file_handle) {
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
get_input_file(const char * path) {
    return (path != NULL) ? fopen(path, "rb") : stdin;
}

/*
 * returns file handle of output file, given a file path string
 * path can be NULL and if it is then it returns stdout
 */
FILE *
get_output_file(const char * path) {
    return (path != NULL) ? fopen(path, "wb") : stdout;
}

/*
 * given an open file handle and a buffer, read the file contents into buffer
 * returns true on success and false on failure.
 */
bool
file_to_buffer(FILE * file_handle, buffer_t * buffer) {
    size_t file_size = get_file_size(file_handle);
    // allocate/re-allocate buffer memory
    if(buffer->bytes == NULL) {
        buffer->bytes = calloc(1, file_size);
    } else {
        buffer->bytes = realloc(buffer->bytes, file_size);
    }
    if(buffer->bytes == NULL) {
        // couldn't allocate enough memory!
        return false;
    }
    buffer->size = file_size;
    // read in file data to buffer
    size_t bytes_read = fread(buffer->bytes, 1, file_size, file_handle);
    // check amount read was the size of file
    if(bytes_read != file_size) {
        // free memory
        free(buffer->bytes);
        return false;
    } else {
        return true;
    }
}

/*
 * given a buffer struct and an open file handle, writes the buffer contents
 * to the file.
 * returns true on success and false on failure.
 */
bool
buffer_to_file(buffer_t * buffer, FILE * file_handle) {
    size_t bytes_written = fwrite(
        buffer->bytes, 1, buffer->size, file_handle
    );
    // check amount written was the size of buffer
    if(bytes_written != buffer->size) {
        return false;
    } else {
        return true;
    }
}

/*
 * function responsible for actually doing the main work, called by main with
 * options configured via command-line.
 * returns true on success, false on failure.
 */
bool
run(
    bool prepare, bool generate, bool render,
    const char * input_file_path, const char * output_file_path
) {
    printf("RUN\n");
    // get input file handle
    FILE * input_file = get_input_file(input_file_path);
    // make input buffer
    buffer_t input_buffer;
    // make output buffer
    buffer_t output_buffer;
    // read input file into buffer
    bool read_ok = file_to_buffer(input_file, &input_buffer);
    // close input file
    fclose(input_file);
    // if read was unsuccessful, don't continue
    if(read_ok == false) {
        fprintf(stderr, "%s\n", "Couldn't read input file");
        return false;
    }
    if(prepare) {
        printf("PREPARE\n");
        // we must build spiral from raw file first
        spiral_t spiral = init_spiral(input_buffer);
        if(generate) {
            printf("GENERATE\n");
            // now we must plot all lines from spiral file
            spiral = plot_spiral(spiral);
        }
        if(render) {
            printf("RENDER\n");
            // we must render an image from spiral
            bitmap_t image = render_spiral(spiral);
            printf("WRITE\n");
            // now write PNG image data to buffer with libpng
            output_buffer = write_png_image(image);
        } else {
            // dump spiral
            output_buffer = dump_spiral(spiral);
        }
    } else if(generate || render) {
        // try and load a spiral struct from input file
        spiral_t spiral = load_spiral(input_buffer);
        // the spiral size will be set to 0 if buffer data was invalid
        if(spiral.size == 0) {
            fprintf(
                stderr, "ERROR - File data was invalid (not a format accepted "
                "by SAXBOSPIRAL " SAXBOSPIRAL_VERSION_STRING ")\n"
            );
            return false;
        }
        if(generate) {
            printf("GENERATE\n");
            // now we must plot all lines from spiral
            spiral = plot_spiral(spiral);
        }
        if(render) {
            printf("RENDER\n");
            // we must render an image from spiral
            // FIXME: This call currently throwing a Segfault!
            bitmap_t image = render_spiral(spiral);
            printf("WRITE\n");
            // now write PNG image data to buffer with libpng
            output_buffer = write_png_image(image);
        } else {
            // let's just dump the spiral again
            output_buffer = dump_spiral(spiral);
        }
    } else {
        // none of the above. this is an error condition - nothing to be done
        fprintf(stderr, "%s\n", "Nothing to be done!");
        return false;
    }
    // get output file handle
    FILE * output_file = get_output_file(output_file_path);
    // now, write output buffer to file
    bool write_ok = buffer_to_file(&output_buffer, output_file);
    // close output file
    fclose(output_file);
    // free buffers
    // free(input_buffer.bytes);
    // free(output_buffer.bytes);
    // return success depends on last write
    return write_ok;
}

// main - mostly just process arguments, the bulk of the work is done by run()
int
main(int argc, char * argv[]) {
    // status code initially set to -1
    int status_code = -1;
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
        status_code = 2;
    }
    // parse arguments
    int count_errors = arg_parse(argc,argv,argtable);
    if(help->count > 0) {
        // first check if we asked for the help option
        printf("Usage: %s", program_name);
        arg_print_syntax(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        status_code = 0;
    } else if(count_errors > 0) {
        // next, if parser returned any errors, display them then exit
        arg_print_errors(stdout, end, program_name);
        status_code = 1;
    }
    // if at this point status_code is not -1, clean up then return early
    if(status_code != -1) {
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return status_code;
    }
    // otherwise, carry on...
    // if we asked for the version, show it
    if(version->count > 0) {
        printf("Saxbospiral " SAXBOSPIRAL_VERSION_STRING "\n");
    }
    // now, call run with options from command-line
    bool result = run(
        (prepare->count > 0) ? true : false,
        (generate->count > 0) ? true : false,
        (render->count > 0) ? true : false,
        * input->filename,
        * output->filename
    );
    // free argtable struct
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    // return appropriate status code based on success/failure
    return (result) ? 0 : 1;
}

#ifdef __cplusplus
} // extern "C"
#endif
