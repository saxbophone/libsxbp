/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides functions for rendering a spiral to a bitmap.
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License (version 3),
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "plot.h"
#include "render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a spiral struct with co-ords in it's cache and a pointer to a
 * 2-item-long array of type co_ord_t, find and store the co-ords for the
 * corners of the square needed to contain the points.
 * NOTE: This should NEVER be called with a pointer to anything other than a
 * 2-item array of type co_ord_t
 *
 * Asserts:
 * - That spiral.co_ord_cache.co_ords.items is not NULL
 * - That bounds is not NULL
 */
static void get_bounds(sxbp_spiral_t spiral, sxbp_co_ord_t* bounds) {
    // preconditional assertions
    assert(spiral.co_ord_cache.co_ords.items != NULL);
    assert(bounds != NULL);
    sxbp_tuple_item_t min_x = 0;
    sxbp_tuple_item_t min_y = 0;
    sxbp_tuple_item_t max_x = 0;
    sxbp_tuple_item_t max_y = 0;
    for(size_t i = 0; i < spiral.co_ord_cache.co_ords.size; i++) {
        if(spiral.co_ord_cache.co_ords.items[i].x < min_x) {
            min_x = spiral.co_ord_cache.co_ords.items[i].x;
        }
        if(spiral.co_ord_cache.co_ords.items[i].y < min_y) {
            min_y = spiral.co_ord_cache.co_ords.items[i].y;
        }
        if(spiral.co_ord_cache.co_ords.items[i].x > max_x) {
            max_x = spiral.co_ord_cache.co_ords.items[i].x;
        }
        if(spiral.co_ord_cache.co_ords.items[i].y > max_y) {
            max_y = spiral.co_ord_cache.co_ords.items[i].y;
        }
    }
    // write bounds to struct
    bounds[0].x = min_x;
    bounds[0].y = min_y;
    bounds[1].x = max_x;
    bounds[1].y = max_y;
}

/*
 * given a spiral struct and a pointer to a blank bitmap_t struct, writes data
 * representing a monochromatic image of the rendered spiral to the bitmap
 * returns a status struct with error information (if any)
 *
 * Asserts:
 * - That image->pixels is NULL
 * - That spiral.lines is not NULL
 */
sxbp_status_t sxbp_render_spiral_raw(
    sxbp_spiral_t spiral, sxbp_bitmap_t* image
) {
    // preconditional assertions
    assert(image->pixels == NULL);
    assert(spiral.lines != NULL);
    // create result status struct
    sxbp_status_t result;
    // plot co-ords of spiral into it's cache
    sxbp_cache_spiral_points(&spiral, spiral.size);
    // get the min and max bounds of the spiral's co-ords
    sxbp_co_ord_t bounds[2] = {{0, 0}};
    get_bounds(spiral, bounds);
    // get the normalisation vector needed to make all values unsigned
    sxbp_tuple_t normalisation_vector = {
        .x = -bounds[0].x,
        .y = -bounds[0].y,
    };
    // get co-ords of bottom right corner, as unsigned
    sxbp_co_ord_t bottom_right = {
        .x = bounds[1].x + normalisation_vector.x,
        .y = bounds[1].y + normalisation_vector.y,
    };
    // initialise image struct - image dimensions are twice the size + 1
    image->width = ((bottom_right.x + 1) * 2) + 1;
    image->height = ((bottom_right.y + 1) * 2) + 1;
    // allocate dynamic memory to image struct - 2D array of bools
    image->pixels = malloc(image->width * sizeof(bool*));
    // check for malloc fail
    if(image->pixels == NULL) {
        result = SXBP_MALLOC_REFUSED;
        return result;
    }
    for(size_t i = 0; i < image->width; i++) {
        image->pixels[i] = calloc(image->height, sizeof(bool));
        // check for malloc fail
        if(image->pixels[i] == NULL) {
            // we need to free() all previous rows
            for(size_t j = i; j > 0; j--) {
                free(image->pixels[j]);
            }
            // now we need to free() the top-level array
            free(image->pixels);
            result = SXBP_MALLOC_REFUSED;
            return result;
        }
    }
    // set 'current point' co-ordinate
    sxbp_co_ord_t current = {
        .x = 0,
        .y = 0,
    };
    // plot the lines of the spiral as points
    for(size_t i = 0; i < spiral.size; i++) {
        // get current direction
        sxbp_vector_t direction = SXBP_VECTOR_DIRECTIONS[spiral.lines[i].direction];
        // make as many jumps in this direction as this lines length
        for(uint64_t j = 0; j < (spiral.lines[i].length * 2U) + 1U; j++) {
            // get output co-ords
            sxbp_tuple_item_t x_pos = current.x + (normalisation_vector.x * 2) + 1;
            sxbp_tuple_item_t y_pos = current.y + (normalisation_vector.y * 2) + 1;
            // skip the second pixel of the first line
            if(!((i == 0) && (j == 1))) {
                // flip the y-axis otherwise they appear vertically mirrored
                image->pixels[x_pos][image->height - 1 - y_pos] = true;
            }
            if(j != (spiral.lines[i].length * 2U)) {
                // if we're not on the last line, advance the marker along
                current.x += direction.x;
                current.y += direction.y;
            }
        }
    }
    // status ok
    result = SXBP_OPERATION_OK;
    return result;
}

/*
 * given a spiral struct, a pointer to a blank buffer and a pointer to a
 * function with the appropriate signature, render the spiral as a bitmap and
 * then call the pointed-to function to render the image to the buffer (in
 * whatever format the function pointer is written for)
 * returns a status struct with error information (if any)
 *
 * Asserts:
 * - That spiral.lines is not NULL
 * - That buffer->bytes is NULL
 * - That the function pointer is not NULL
 */
sxbp_status_t sxbp_render_spiral_image(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer,
    sxbp_status_t(* image_writer_callback)(
        sxbp_bitmap_t image, sxbp_buffer_t* buffer
    )
) {
    // preconditional assertions
    assert(spiral.lines != NULL);
    assert(buffer->bytes == NULL);
    assert(image_writer_callback != NULL);
    // create bitmap to render raw image to
    sxbp_bitmap_t raw_image = {0, 0, NULL};
    // render spiral to raw image (and store success/failure)
    sxbp_status_t result = sxbp_render_spiral_raw(spiral, &raw_image);
    // check return status
    if(result != SXBP_OPERATION_OK) {
        return result;
    }
    // render to buffer using callback (and return its status code)
    return image_writer_callback(raw_image, buffer);
}

#ifdef __cplusplus
} // extern "C"
#endif
