#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

const version_t VERSION = {
    .major = 0, .minor = 8, .patch = 0,
};

// vector direction constants
const vector_t VECTOR_DIRECTIONS[4] = {
    // UP       RIGHT       DOWN        LEFT
    { 0, 1, }, { 1, 0, }, { 0, -1, }, { -1, 0, },
};

#ifdef __cplusplus
} // extern "C"
#endif
