#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// Version numbers are passed as preprocessor definitions by CMake
const version_t VERSION = {
    .major = SAXBOSPIRAL_VERSION_MAJOR,
    .minor = SAXBOSPIRAL_VERSION_MINOR,
    .patch = SAXBOSPIRAL_VERSION_PATCH,
};

/*
 * computes a version_hash_t for a given version_t,
 * to be used for indexing and ordering
 */
version_hash_t version_hash(version_t version) {
    return (version.major * 65536) + (version.minor * 256) + version.patch;
}

// vector direction constants
const vector_t VECTOR_DIRECTIONS[4] = {
    // UP       RIGHT       DOWN        LEFT
    { 0, 1, }, { 1, 0, }, { 0, -1, }, { -1, 0, },
};

#ifdef __cplusplus
} // extern "C"
#endif
