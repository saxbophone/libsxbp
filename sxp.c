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
    bool prepare, bool generate, bool render, bool perfect, int perfect_threshold,
    const char * input_file_path, const char * output_file_path
) {
    // get input file handle
    FILE * input_file = fopen(input_file_path, "rb");
    if(input_file == NULL) {
        fprintf(stderr, "%s\n", "Couldn't open input file");
        return false;
    }
    // make input buffer
    buffer_t input_buffer = {};
    // make output buffer
    buffer_t output_buffer = {};
    // read input file into buffer
    bool read_ok = file_to_buffer(input_file, &input_buffer);
    // used later for telling if write of output file was success
    bool write_ok = false;
    // close input file
    fclose(input_file);
    // get output file handle
    FILE * output_file = fopen(output_file_path, "wb");
    if(output_file == NULL) {
        fprintf(stderr, "%s\n", "Couldn't open output file");
        return false;
    }
    // if read was unsuccessful, don't continue
    if(read_ok == false) {
        fprintf(stderr, "%s\n", "Couldn't read input file");
        return false;
    }
    // create initial blank spiral struct
    spiral_t spiral = {};
    // resolve perfection threshold - set to -1 if disabled completely
    int perfection = (perfect == false) ? -1 : perfect_threshold;

    //// TODO: Get this version of the program working, as this is ideal

    // if(prepare) {
    //     // we must build spiral from raw file first
    //     init_spiral(input_buffer, &spiral);
    // } else {
    //     // otherwise, we must load spiral from file
    //     load_spiral(input_buffer, &spiral);
    // }
    // if(generate) {
    //     // we must plot all lines from spiral file
    //     spiral = plot_spiral(spiral, perfection);
    // }
    // if(render) {
    //     // we must render an image from spiral
    //     bitmap_t image = {};
    //     render_spiral(spiral, &image);
    //     // now write PNG image data to buffer with libpng
    //     write_png_image(image, &output_buffer);
    // } else {
    //     // otherwise, we must simply dump the spiral as-is
    //     dump_spiral(spiral, &output_buffer);
    // }

    //// NOTE: End Ideal version of the program

    if(prepare) {
        // we must build spiral from raw file first
        init_spiral(input_buffer, &spiral);
        if(generate) {
            // now we must plot all lines from spiral file
            spiral = plot_spiral(spiral, perfection);
        }
        // dump spiral
        dump_spiral(spiral, &output_buffer);
    } else if(generate) {
        // try and load a spiral struct from input file
        load_spiral(input_buffer, &spiral);
        // we must plot all lines from spiral file
        spiral = plot_spiral(spiral, perfection);
        // dump spiral
        dump_spiral(spiral, &output_buffer);
    } else if(render) {
        // try and load a spiral struct from input file
        load_spiral(input_buffer, &spiral);
        // we must render an image from spiral
        bitmap_t image;
        render_spiral(spiral, &image);
        // now write PNG image data to buffer with libpng
        write_png_image(image, &output_buffer);
    } else {
        // none of the above. this is an error condition - nothing to be done
        fprintf(stderr, "%s\n", "Nothing to be done!");
        return false;
    }
    // now, write output buffer to file
    write_ok = buffer_to_file(&output_buffer, output_file);
    // close output file
    fclose(output_file);
    // free buffers
    free(input_buffer.bytes);
    free(output_buffer.bytes);
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
        "generate the correct lengths of all the lines in the spiral"
    );
    // flag for if we want to render a spiral to imagee
    struct arg_lit * render = arg_lit0(
        "r", "render", "render the input spiral to an image (cannot be used "
        "with -p or -g options)"
    );
    struct arg_lit * perfect = arg_lit0(
        "D", "disable-perfection", "allow aggressive optimisations to take "
        "place for a massive speed boost, at the cost of producing spirals that"
        " are imperfect and waste some space with oversized lines"
    );
    struct arg_int * perfect_threshold = arg_int0(
        "d", "perfection-threshold", NULL, "set a threshold above which length "
        "lines are not optimised (default value is 1, which still yields "
        "results)"
    );
    // input file path option
    struct arg_file * input = arg_file0(
        "i", "input", NULL, "input file path"
    );
    // output file path option
    struct arg_file * output = arg_file0(
        "o", "output", NULL, "output file path"
    );
    // argtable boilerplate
    struct arg_end * end = arg_end(20);
    void * argtable[] = {
        help, version,
        prepare, generate, render,
        perfect, perfect_threshold,
        input, output, end,
    };
    const char * program_name = "sxp";
    // check argtable members were allocated successfully
    if(arg_nullcheck(argtable) != 0) {
        // NULL entries were detected, so some allocations failed
        fprintf(
            stderr, "%s\n", "FATAL: Could not allocate all entries for argtable"
        );
        status_code = 2;
    }
    // set default value of perfect_threshold argument
    perfect_threshold->ival[0] = 1;
    // parse arguments
    int count_errors = arg_parse(argc, argv, argtable);
    // if we asked for the version, show it
    if(version->count > 0) {
        printf("%s %s\n", program_name, SAXBOSPIRAL_VERSION_STRING);
        status_code = 0;
    }
    if(help->count > 0) {
        // check if we asked for the help option
        printf("Usage: %s", program_name);
        arg_print_syntax(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        status_code = 0;
    } else if(count_errors > 0) {
        // if parser returned any errors, display them then exit
        arg_print_errors(stderr, end, program_name);
        status_code = 1;
    }
    // if at this point status_code is not -1, clean up then return early
    if(status_code != -1) {
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return status_code;
    }
    // otherwise, carry on...
    // now, call run with options from command-line
    bool result = run(
        (prepare->count > 0) ? true : false,
        (generate->count > 0) ? true : false,
        (render->count > 0) ? true : false,
        (perfect->count > 0) ? false : true,
        perfect_threshold->ival[0],
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