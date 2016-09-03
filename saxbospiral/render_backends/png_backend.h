#ifndef SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H
#define SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H

#include "../saxbospiral.h"
#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a bitmap_t struct, create a new buffer and write the bitmap data as a
 * PNG image to the buffer, using libpng. Returns the written buffer.
 */
buffer_t
write_png_image(bitmap_t bitmap);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
