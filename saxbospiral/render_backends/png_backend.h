#ifndef SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H
#define SAXBOPHONE_SAXBOSPIRAL_PNG_BACKEND_H

#include "../render.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * given a file handle and a bitmap_t struct, write the bitmap data as a PNG
 * image to the file, using libpng
 *
 * TODO: Change this to write to a buffer rather than straight to file.
 */
void
write_png_image(FILE * file_handle, bitmap_t bitmap);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
