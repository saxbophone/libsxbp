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
size_t get_file_size(FILE * file_handle) {
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
bool file_to_buffer(FILE * file_handle, buffer_t * buffer) {
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
bool buffer_to_file(buffer_t * buffer, FILE * file_handle) {
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
 * private function, given a diagnostic_t error, returns the string name of the
 * error code
 */
static const char * error_code_string(diagnostic_t error) {
    switch(error) {
        case OPERATION_FAIL:
            return "OPERATION_FAIL";
        case MALLOC_REFUSED:
            return "MALLOC_REFUSED";
        case IMPOSSIBLE_CONDITION:
            return "IMPOSSIBLE_CONDITION";
        case OPERATION_OK:
            return "OPERATION_OK (NO ERROR)";
        case STATE_UNKNOWN:
        default:
            return "UNKNOWN ERROR";
    }
}

/*
 * private function, given a deserialise_diagnostic_t error, returns the string
 * name of the error code
 */
static const char * file_error_code_string(deserialise_diagnostic_t error) {
    switch(error) {
        case DESERIALISE_OK:
            return "DESERIALISE_OK (NO ERROR)";
        case DESERIALISE_BAD_HEADER_SIZE:
            return "DESERIALISE_BAD_HEADER_SIZE";
        case DESERIALISE_BAD_MAGIC_NUMBER:
            return "DESERIALISE_BAD_MAGIC_NUMBER";
        case DESERIALISE_BAD_VERSION:
            return "DESERIALISE_BAD_VERSION";
        case DESERIALISE_BAD_DATA_SIZE:
            return "DESERIALISE_BAD_DATA_SIZE";
        default:
            return "UNKNOWN ERROR";
    }
}

/*
 * private function to handle all generic errors, by printing to stderr
 * returns true if there was an error, false if not
 */
static bool handle_error(status_t result) {
    // if we had problems, print to stderr and return true
    if(result.diagnostic != OPERATION_OK) {
        fprintf(
            stderr,
            "Error Code: %s\n", error_code_string(result.diagnostic)
        );
        return true;
    } else {
        // otherwise, return false to say 'no error'
        return false;
    }
}

/*
 * function responsible for actually doing the main work, called by main with
 * options configured via command-line.
 * returns true on success, false on failure.
 */
bool run(
    bool prepare, bool generate, bool render, bool perfect,
    int perfect_threshold, int line_limit, int total_lines,
    const char * input_file_path, const char * output_file_path
) {
    // get input file handle
    FILE * input_file = fopen(input_file_path, "rb");
    if(input_file == NULL) {
        fprintf(stderr, "%s\n", "Couldn't open input file");
        return false;
    }
    // make input buffer
    buffer_t input_buffer = {0, 0};
    // make output buffer
    buffer_t output_buffer = {0, 0};
    // read input file into buffer
    bool read_ok = file_to_buffer(input_file, &input_buffer);
    // used later for telling if write of output file was success
    bool write_ok = false;
    // close input file
    fclose(input_file);
    // if read was unsuccessful, don't continue
    if(read_ok == false) {
        fprintf(stderr, "%s\n", "Couldn't read input file");
        return false;
    }
    // create initial blank spiral struct
    spiral_t spiral = {0, NULL, {{NULL, 0}, 0}, false, 0, 0, 0};
    // resolve perfection threshold - set to -1 if disabled completely
    int perfection = (perfect == false) ? -1 : perfect_threshold;
    // check error condition (where no actions were specified)
    if((prepare || generate || render) == false) {
        // none of the above. this is an error condition - nothing to be done
        fprintf(stderr, "%s\n", "Nothing to be done!");
        return false;
    }
    // otherwise, good to go
    if(prepare) {
        // we must build spiral from raw file first
        if(handle_error(init_spiral(input_buffer, &spiral))) {
            // handle errors
            return false;
        }
    } else {
        // otherwise, we must load spiral from file
        serialise_result_t result = load_spiral(input_buffer, &spiral);
        // if we had problems, print to stderr and quit
        if(result.status.diagnostic != OPERATION_OK) {
            fprintf(
                stderr,
                "Error Code:\t\t%s\nFile Error Code:\t%s\n",
                error_code_string(result.status.diagnostic),
                file_error_code_string(result.diagnostic)
            );
            return false;
        }
    }
    if(generate) {
        /*
         * find out how many lines we are to plot
         * this is based on two options: the line_limit and the total_lines
         * arguments.
         */
        // first, check the line_limit argument
        uint64_t lines_to_plot;
        // set to spiral size + line limit if set, else spiral size
        lines_to_plot = (
            (line_limit != -1) ? (spiral.size + line_limit) : spiral.size
        );
        // set to total_lines if set and less than current amount
        lines_to_plot = (
            (total_lines != -1 && total_lines < lines_to_plot) ?
            (uint64_t)total_lines : lines_to_plot
        );
        // we must plot all lines from spiral file
        if(handle_error(plot_spiral(&spiral, perfection, lines_to_plot))) {
            // handle errors
            return false;
        }
    }
    if(render) {
        // we must render an image from spiral
        bitmap_t image = {0, 0, 0};
        if(handle_error(render_spiral(spiral, &image))) {
            // handle errors
            return false;
        }
        // now write PNG image data to buffer with libpng
        if(handle_error(write_png_image(image, &output_buffer))) {
            // handle errors
            return false;
        }
    } else {
        // otherwise, we must simply dump the spiral as-is
        serialise_result_t result = dump_spiral(spiral, &output_buffer);
        // if we had problems, print to stderr and quit
        if(result.status.diagnostic != OPERATION_OK) {
            fprintf(
                stderr,
                "Error Code:\t\t%s\nFile Error Code:\t%s\n",
                error_code_string(result.status.diagnostic),
                file_error_code_string(result.diagnostic)
            );
            return false;
        }
    }
    // get output file handle
    FILE * output_file = fopen(output_file_path, "wb");
    if(output_file == NULL) {
        fprintf(stderr, "%s\n", "Couldn't open output file");
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
int main(int argc, char * argv[]) {
    // status code initially set to -1
    int status_code = -1;
    // build argtable struct for parsing command-line arguments
    // show help
    struct arg_lit * help = arg_lit0("h","help", "show this help and exit");
    // show version
    struct arg_lit * version = arg_lit0("v", "version", "show version and exit");
    // flag for if we want to prepare a spiral
    struct arg_lit * prepare = arg_lit0(
        "p", "prepare",
        "prepare a spiral from raw binary data"
    );
    // flag for if we want to generate the solution for a spiral's line lengths
    struct arg_lit * generate = arg_lit0(
        "g", "generate",
        "generate the lengths of a spiral's lines"
    );
    // flag for if we want to render a spiral to imagee
    struct arg_lit * render = arg_lit0(
        "r", "render", "render a spiral to an image"
    );
    struct arg_lit * perfect = arg_lit0(
        "D", "disable-perfection", "allow unlimited optimisations"
    );
    struct arg_int * perfect_threshold = arg_int0(
        "d", "perfection-threshold", NULL, "set optimisation threshold"
    );
    struct arg_int * total_lines = arg_int0(
        "t", "total-lines", NULL, "total number of lines to plot to"
    );
    struct arg_int * line_limit = arg_int0(
        "l", "line-limit", NULL, "plot this many more lines than currently solved"
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
        perfect, perfect_threshold, line_limit, total_lines,
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
    // set default value of line_limit and total_lines arguments
    line_limit->ival[0] = -1;
    total_lines->ival[0] = -1;
    // parse arguments
    int count_errors = arg_parse(argc, argv, argtable);
    // if we asked for the version, show it
    if(version->count > 0) {
        printf("%s %s\n", program_name, SAXBOSPIRAL_VERSION_STRING);
        status_code = 0;
    }
    // if parser returned any errors, display them and set return code to 1
    if(count_errors > 0) {
        arg_print_errors(stderr, end, program_name);
        status_code = 1;
    }
    // set return code to 0 if we asked for help
    if(help->count > 0) {
        status_code = 0;
    }
    // display usage information if we asked for help or got arguments wrong
    if((count_errors > 0) || (help->count > 0)) {
        printf("Usage: %s", program_name);
        arg_print_syntax(stdout, argtable, "\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
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
        line_limit->ival[0],
        total_lines->ival[0],
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
