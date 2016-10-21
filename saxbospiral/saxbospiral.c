/*
 * This source file forms part of libsaxbospiral, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides most of the data types used by the library, as
 * well as defining some library constants and versioning functionality.
 *
 *
 *
 * Copyright (C) 2016, Joshua Saxby joshua.a.saxby+TNOPLuc8vM==@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

// Version numbers are passed as preprocessor definitions by CMake
const sxbp_version_t LIB_SXBP_VERSION = {
    .major = SAXBOSPIRAL_VERSION_MAJOR,
    .minor = SAXBOSPIRAL_VERSION_MINOR,
    .patch = SAXBOSPIRAL_VERSION_PATCH,
    .string = SAXBOSPIRAL_VERSION_STRING,
};

/*
 * computes a version_hash_t for a given version_t,
 * to be used for indexing and ordering
 */
sxbp_version_hash_t sxbp_version_hash(sxbp_version_t version) {
    return (version.major * 65536) + (version.minor * 256) + version.patch;
}

// vector direction constants
const sxbp_vector_t SXBP_VECTOR_DIRECTIONS[4] = {
    // UP       RIGHT       DOWN        LEFT
    { 0, 1, }, { 1, 0, }, { 0, -1, }, { -1, 0, },
};

#ifdef __cplusplus
} // extern "C"
#endif
