#include "bspirals.h"


#ifdef __cplusplus
extern "C"{
#endif

// when facing the direction specified by current, return the direction that
// will be faced when turning in the rotational direction specified by turn.
direction_t
change_direction(direction_t current, rotation_t turn) {
    return (current + turn) % 4;
}

#ifdef __cplusplus
} // extern "C"
#endif
