/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This program provides a very basic command-line program for testing this new
 * version of sxbp until a more complete version is made.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "sxbp/sxbp.h"


#ifdef __cplusplus
extern "C"{
#endif

int main(int argc, char const *argv[]) {
    if (argc - 1 < 2) {
        fprintf(stderr, "Too few arguments given, usage:\nsxbp <string> <output file>\n");
        return 1;
    }
    // extract command-line arguments
    const char* string = argv[1];
    const char* output_file_name = argv[2];
    // try and open output file and handle error if unsuccessful
    FILE* output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "Cannot open output file %s for writing\n", output_file_name);
        return 2;
    }
    // construct and allocate buffer to read string into
    sxbp_buffer_t buffer = sxbp_blank_buffer();
    buffer.size = strlen(string);
    sxbp_init_buffer(&buffer);
    // copy string into buffer
    memcpy(buffer.bytes, string, buffer.size);
    // create the figure and begin constructing it from buffer data
    sxbp_figure_t figure = sxbp_blank_figure();
    sxbp_begin_figure(&buffer, &figure);
    // we can now free the buffer as it's no longer needed
    sxbp_free_buffer(&buffer);
    // now refine the figure
    sxbp_refine_figure(&figure);
    // render the figure to bitmap
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();
    sxbp_render_figure(&figure, &bitmap);
    // TODO: render the bitmap as PBM to buffer
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif
