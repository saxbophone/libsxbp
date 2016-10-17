#ifndef SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H
#define SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a bitmap_t struct and a pointer to a blank buffer_t, write the bitmap
 * data as a PNG image to the buffer, using libpng.
 * returns a status struct containing error information, if any
 */
sxbp_status_t sxbp_write_png_image(sxbp_bitmap_t bitmap, sxbp_buffer_t* buffer);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
