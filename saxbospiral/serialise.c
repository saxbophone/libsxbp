// sanity check for support of 64-bit integers
#if __SIZEOF_SIZE_T__ < 8
#warning "Please compile this code for a target with 64-bit words or greater."
#endif

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
 * given a buffer and a pointer to a blank spiral_t, create a spiral represented
 * by the data in the struct and write to the spiral
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 */
serialise_result_t load_spiral(buffer_t buffer, spiral_t * spiral) {
    serialise_result_t result; // build struct for returning success / failure
    // first, if header is too small for header + 1 line, then return early
    if(buffer.size < FILE_HEADER_SIZE + LINE_T_PACK_SIZE) {
        result.status.location = DEBUG; // catch location of error
        result.status.diagnostic = OPERATION_FAIL; // flag failure
        result.diagnostic = DESERIALISE_BAD_HEADER_SIZE; // failure reason
        return result;
    }
    // check for magic number and return early if not right
    if(strncmp((char *)buffer.bytes, "SAXBOSPIRAL", 11) != 0) {
        result.status.location = DEBUG; // catch location of error
        result.status.diagnostic = OPERATION_FAIL; // flag failure
        result.diagnostic = DESERIALISE_BAD_MAGIC_NUMBER; // failure reason
        return result;
    }
    // grab file version from header
    version_t buffer_version = {
        .major = buffer.bytes[12],
        .minor = buffer.bytes[13],
        .patch = buffer.bytes[14],
    };
    // we don't accept anything over v0.12.x, so the max is v0.12.255
    version_t max_version = { .major = 0, .minor = 12, .patch = 255, };
    // check for version compatibility
    if(version_hash(buffer_version) > version_hash(max_version)) {
        // check failed
        result.status.location = DEBUG; // catch location of error
        result.status.diagnostic = OPERATION_FAIL; // flag failure
        result.diagnostic = DESERIALISE_BAD_VERSION; // failure reason
        return result;
    }
    // get size of spiral object contained in buffer
    size_t spiral_size = 0;
    for(size_t i = 0; i < 8; i++) {
        spiral_size |= (buffer.bytes[16 + i]) << (8 * (7 - i));
    }
    // Check that the file data section is large enough for the spiral size
    if((buffer.size - FILE_HEADER_SIZE) != (LINE_T_PACK_SIZE * spiral_size)) {
        // this check failed
        result.status.location = DEBUG; // catch location of error
        result.status.diagnostic = OPERATION_FAIL; // flag failure
        result.diagnostic = DESERIALISE_BAD_DATA_SIZE; // failure reason
        return result;
    }
    // good to go
    // populate spiral struct
    spiral->size = spiral_size;
    // allocate memory
    spiral->lines = calloc(sizeof(line_t), spiral->size);
    // catch allocation error
    if(spiral->lines == NULL) {
        result.status.location = DEBUG; // catch location of error
        result.status.diagnostic = MALLOC_REFUSED; // flag failure
        return result;
    }
    // convert each serialised line segment in buffer into a line_t struct
    for(size_t i = 0; i < spiral_size; i++) {
        // direction is stored in 2 most significant bits of each 32-bit sequence
        spiral->lines[i].direction = (
            buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)] >> 6
        );
        /*
         * length is stored as 30 least significant bits, so we have to unpack it
         * handle first byte on it's own as we only need least 6 bits of it
         * bit mask and shift 3 bytes to left
         */
        spiral->lines[i].length = (
            buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)]
            & 0x3f // <= binary value is 0b00111111
        ) << 24;
        // handle remaining 3 bytes in loop
        for(uint8_t j = 0; j < 3; j++) {
            spiral->lines[i].length |= (
                buffer.bytes[FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE) + 1 + j]
            ) << (8 * (2 - j));
        }
    }
    // return ok status
    result.status.diagnostic = OPERATION_OK;
    return result;
}

/*
 * given a spiral_t struct and a pointer to a blank buffer_t, serialise a binary
 * representation of the spiral and write this to the data buffer pointer
 * returns a serialise_result_t struct, which will contain information about
 * whether the operation was successful or not and information about what went
 * wrong if it was not successful
 */
serialise_result_t dump_spiral(spiral_t spiral, buffer_t * buffer) {
    serialise_result_t result; // build struct for returning success / failure
    // populate buffer struct, base size on header + spiral size
    buffer->size = (FILE_HEADER_SIZE + (LINE_T_PACK_SIZE * spiral.size));
    // allocate memory for buffer
    buffer->bytes = calloc(1, buffer->size);
    // catch memory allocation failure
    if(buffer->bytes == NULL) {
        result.status.location = DEBUG;
        result.status.diagnostic = MALLOC_REFUSED;
        return result;
    }
    // write first part of data header (magic number and version info)
    sprintf(
        (char *)buffer->bytes, "SAXBOSPIRAL\n%c%c%c\n",
        VERSION.major, VERSION.minor, VERSION.patch
    );
    // write second part of data header (spiral size as 64 bit uint)
    for(uint8_t i = 0; i < 8; i++) {
        uint8_t shift = (8 * (7 - i));
        buffer->bytes[16 + i] = (uint8_t)(
            ((uint64_t)spiral.size & (0xffUL << shift)) >> shift
        );
    }
    // write final newline at end of header
    buffer->bytes[24] = '\n';
    // now write the data section
    for(size_t i = 0; i < spiral.size; i++) {
        /*
         * serialise each line in the spiral to 4 bytes, handle first byte first
         * map direction to 2 most significant bits
         */
        buffer->bytes[
            FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)
        ] = (spiral.lines[i].direction << 6);
        // handle first 6 bits of the length
        buffer->bytes[
            FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE)
        ] |= (spiral.lines[i].length >> 24);
        // handle remaining 3 bytes in a loop
        for(uint8_t j = 0; j < 3; j++) {
            buffer->bytes[
                FILE_HEADER_SIZE + (i * LINE_T_PACK_SIZE) + 1 + j
            ] = (uint8_t)(spiral.lines[i].length >> (8 * (2 - j)));
        }
    }
    // return ok status
    result.status.diagnostic = OPERATION_OK;
    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
