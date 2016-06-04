#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "saxbospiral.h"
#include "plot.h"
#include "render.h"


#ifdef __cplusplus
extern "C"{
#endif

// given a spiral struct with co-ords in it's cache, find and return the co-ords
// for the corners of the square needed to contain the points.
static co_ord_array_t
get_bounds(spiral_t spiral) {
    int64_t min_x = 0;
    int64_t min_y = 0;
    int64_t max_x = 0;
    int64_t max_y = 0;
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
bitmap_t
render_spiral(spiral_t spiral) {
    // plot co-ords of spiral into it's cache
    cache_spiral_points(&spiral, spiral.size);
    // get the min and max bounds of the spiral's co-ords
    co_ord_array_t bounds = get_bounds(spiral);
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
    // initialise output bitmap - image dimensions are twice the size + 1
    bitmap_t output = {
        .width = ((bottom_right.x + 1) * 2) + 1,
        .height = ((bottom_right.y + 1) * 2) + 1,
    };
    // allocate dynamic memory - 2D array of bools
    output.pixels = malloc(output.width * sizeof(bool*));
    for(size_t i = 0; i < output.width; i++) {
        output.pixels[i] = calloc(output.height, sizeof(bool));
    }
    // set 'current point' co-ordinate
    co_ord_t current = {
        .x = 0,
        .y = 0,
    };
    // plot the lines of the spiral as points
    for(size_t i = 0; i < spiral.size; i++) {
        // get current direction
        vector_t direction = VECTOR_DIRECTIONS[spiral.lines[i].direction];
        // make as many jumps in this direction as this lines length
        for(uint64_t j = 0; j < (spiral.lines[i].length * 2) + 1; j++) {
            // get output co-ords
            int64_t x_pos = current.x + (normalisation_vector.x * 2) + 1;
            int64_t y_pos = current.y + (normalisation_vector.y * 2) + 1;
            // skip the second pixel of the first line
            if(!((i == 0) && (j == 1))) {
                // flip the y-axis otherwise they appear vertically mirrored
                output.pixels[x_pos][output.height - 1 - y_pos] = true;
            }
            if(j != (spiral.lines[i].length * 2)) {
                // if we're not on the last line, advance the marker along
                current.x += direction.x;
                current.y += direction.y;
            }
        }
    }
    return output;
}

#ifdef __cplusplus
} // extern "C"
#endif
