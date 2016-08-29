#include <stdlib.h>

#include <png.h>

#include "../saxbospiral.h"
#include "../render.h"
#include "png_backend.h"


#ifdef __cplusplus
extern "C"{
#endif

// private custom libPNG buffer write function
static void
buffer_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    // retrieve pointer to buffer
    buffer_t * p = (buffer_t *)png_get_io_ptr(png_ptr);
    size_t new_size = p->size + length;
    // allocate or grow buffer
    if(p->bytes) {
        p->bytes = realloc(p->bytes, new_size);
    }
    else{
        p->bytes = malloc(new_size);
    }
    if(!p->bytes){
        png_error(png_ptr, "Write Error");
    }
    // copy new bytes to end of buffer
    memcpy(p->bytes + p->size, data, length);
    p->size += length;
}

// dummy function for unecessary flush function
void dummy_png_flush(png_structp png_ptr) {}

// private function for generating the PNG structure
static void
build_png(
    bitmap_t bitmap, buffer_t * buffer, png_structp png_ptr,
    png_infop info_ptr, png_bytep row
) {
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      // abort();
      png_error(png_ptr, "Write Error");
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      // abort();
      png_error(png_ptr, "Write Error");
    }
    // set PNG write function - in this case, a function that writes to buffer
    png_set_write_fn(png_ptr, buffer, buffer_write_data, dummy_png_flush);
    // Write header - specify a 1-bit grayscale image with adam7 interlacing
    png_set_IHDR(
        png_ptr, info_ptr, bitmap.width, bitmap.height,
        1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
    );
    // configure bit packing - 1 bit gray channel
    png_color_8 sig_bit;
    sig_bit.gray = 1;
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);
    // Set image metadata
    png_text metadata[5]; // Author, Description, Copyright, Software, Comment
    metadata[0].key = "Author";
    metadata[0].text = "Joshua Saxby (https://github.com/saxbophone)";
    metadata[1].key = "Description";
    metadata[1].text = "Experimental generation of 2D spiralling lines based on input binary data";
    metadata[2].key = "Copyright";
    metadata[2].text = "Copyright Joshua Saxby";
    metadata[3].key = "Software";
    // SAXBOSPIRAL_VERSION_STRING is a macro that expands to a double-quoted string
    metadata[3].text = "SAXBOSPIRAL " SAXBOSPIRAL_VERSION_STRING;
    metadata[4].key = "Comment";
    metadata[4].text = "https://github.com/saxbophone/saxbospiral";
    // set compression of each metadata key
    for(size_t i = 0; i < 5; i++) {
        metadata[i].compression = PNG_TEXT_COMPRESSION_NONE;
    }
    // write metadata
    png_set_text(png_ptr, info_ptr, metadata, 5);
    png_write_info(png_ptr, info_ptr);
    // set bit shift - TODO: Check if this is acutally needed
    png_set_shift(png_ptr, &sig_bit);
    // set bit packing - NOTE: I'm pretty sure this bit is needed but worth checking
    png_set_packing(png_ptr);
    // Allocate memory for one row (1 byte per pixel - RGB)
    row = (png_bytep) malloc(bitmap.width * sizeof(png_byte));
    // Write image data
    for (size_t y = 0 ; y < bitmap.height; y++) {
       for (size_t x = 0; x < bitmap.width; x++) {
            // set to black if there is a point here, white if not
            row[x] = (bitmap.pixels[x][y] == true) ? 0 : 1;
       }
       png_write_row(png_ptr, row);
    }
    // End write
    png_write_end(png_ptr, NULL);
    // cleanup
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);
    return;
}

/*
 * given a bitmap_t struct, create a new buffer and write the bitmap data as a
 * PNG image to the buffer, using libpng. Returns the written buffer.
 */
buffer_t
write_png_image(bitmap_t bitmap) {
    // init libpng stuff
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;
    // init buffer
    buffer_t buffer = { .size = 0, };
    build_png(bitmap, &buffer, png_ptr, info_ptr, row);
    return buffer;
}

#ifdef __cplusplus
} // extern "C"
#endif
