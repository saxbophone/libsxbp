#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "saxbospiral.h"
#include "serialise.h"


#ifdef __cplusplus
extern "C"{
#endif

// constants related to how spiral data is packed in files - measured in bytes
const size_t FILE_HEADER_SIZE = 25;
const size_t LINE_T_PACK_SIZE = 4;

/*
 * given a buffer, return a spiral represented by the data in the struct
 * returns a spiral of length 0 if the data could not be interpreted correctly
 */
spiral_t
load_spiral(buffer_t buffer) {
    // create initial output spiral, a spiral of length 0 (shows failure)
    spiral_t output = { .size = 0, };
    /*
     * Check for buffer size (must be at least the size of the header + amount
     * of space needed for one line). Also, check for magic number
     */
    if(
        (strncmp((char *)buffer.bytes, "SAXBOSPIRAL", 11) == 0)
        && (buffer.size >= FILE_HEADER_SIZE + 4)
    ) {
        // good to go
        // TODO: Add checks for buffer data version compatibility
        /*
        version_t data_version = {
            buffer.bytes[12], buffer.bytes[13], buffer.bytes[13],
        };
        */
        // get size of spiral object contained in buffer
        size_t spiral_size = 0;
        for(size_t i = 0; i < 8; i++) {
            spiral_size |= (buffer.bytes[16 + i]) << (8 * (7 - i));
        }
        // Check that the file data section is large enough for the spiral size
        if((buffer.size - FILE_HEADER_SIZE) != (LINE_T_PACK_SIZE * spiral_size)) {
            // this check failed, so return it as it is
            return output;
        }
        // init spiral struct
        output.size = spiral_size;
        // allocate memory
        output.lines = calloc(sizeof(line_t), output.size);
        // convert each serialised line segment in buffer into a line_t struct
        for(size_t i = 0; i < spiral_size; i++) {
            // direction is stored in 2 most significant bits of each 32-bit sequence
            output.lines[i].direction = (
                buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)] >> 6
            );
            /*
             * length is stored as 30 least significant bits, so we have to unpack it
             * handle first byte on it's own as we only need least 6 bits of it
             * bit mask and shift 3 bytes to left
             */
            output.lines[i].length = (
                buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)] & 0b00111111
            ) << 24;
            // handle remaining 3 bytes in loop
            for(uint8_t j = 0; j < 3; j++) {
                output.lines[i].length |= (
                    buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE) + 1 + j]
                ) << (8 * (2 - j));
            }
        }
    }
    /*
     * return the output struct, this will be a struct of size 0 if the magic
     * number check failed, else it will be a valid spiral
     */
    return output;
}

// given a spiral, return a buffer of the raw bytes used to represent and store it
buffer_t
dump_spiral(spiral_t spiral) {
    // build output buffer struct, base size on header + spiral size
    buffer_t output = {
        .size = (FILE_HEADER_SIZE + (LINE_T_PACK_SIZE * spiral.size)),
    };
    // allocate memory
    output.bytes = calloc(1, output.size);
    // write first part of data header (magic number and version info)
    sprintf(
        (char *)output.bytes, "SAXBOSPIRAL\n%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch
    );
    // write second part of data header (spiral size as 64 bit uint)
    for(uint8_t i = 0; i < 8; i++) {
        uint8_t shift = (8 * (7 - i));
        output.bytes[16 + i] = (uint8_t)(
            ((uint64_t)spiral.size & (0xff << shift)) >> shift
        );
    }
    // write final newline at end of header
    output.bytes[24] = '\n';
    // now write the data section
    for(size_t i = 0; i < spiral.size; i++) {
        /*
         * serialise each line in the spiral to 4 bytes, handle first byte first
         * map direction to 2 most significant bits
         */
        output.bytes[
            FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)
        ] = (spiral.lines[i].direction << 6);
        // handle first 6 bits of the length
        output.bytes[
            FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)
        ] |= (spiral.lines[i].length >> 24);
        // handle remaining 3 bytes in a loop
        for(uint8_t j = 0; j < 3; j++) {
            output.bytes[
                FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE) + 1 + j
            ] = (uint8_t)(spiral.lines[i].length >> (8 * (2 - j)));
        }
    }
    return output;
}

#ifdef __cplusplus
} // extern "C"
#endif
