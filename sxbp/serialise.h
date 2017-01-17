/*
 * This source file forms part of libsxbp, a library which generates
 * experimental 2D spiral-like shapes based on input binary data.
 */

/**
 * @file
 *
 * @brief This compilation unit provides functions for serialising spirals to and from
 * byte sequences (which may be stored in files or any other form).
 *
 * @author Joshua Saxby <joshua.a.saxby+TNOPLuc8vM==@gmail.com
 * @date 2016, 2017
 *
 * @copyright Copyright (C) Joshua Saxby 2016, 2017
 *
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H
#define SAXBOPHONE_SAXBOSPIRAL_SERIALISE_H

#include <stddef.h>

#include "saxbospiral.h"


#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Provides further error information specific to spiral de-serialisation
 */
typedef enum sxbp_deserialise_diagnostic_t {
    /** @brief no problem */
    SXBP_DESERIALISE_OK,
    /** @brief header section too small to be valid */
    SXBP_DESERIALISE_BAD_HEADER_SIZE,
    /** @brief wrong magic number in header section */
    SXBP_DESERIALISE_BAD_MAGIC_NUMBER,
    /** @brief unsupported data version (according to header) */
    SXBP_DESERIALISE_BAD_VERSION,
    /** @brief data section too small to be valid */
    SXBP_DESERIALISE_BAD_DATA_SIZE,
} sxbp_deserialise_diagnostic_t;

/**
 * @brief Stores both generic and serialisation-specific error information.
 */
typedef struct sxbp_serialise_result_t {
    /** @brief generic error information applicable to all functions */
    sxbp_status_t status;
    /** @brief additional specific error information */
    sxbp_deserialise_diagnostic_t diagnostic;
} sxbp_serialise_result_t;

/** @brief The size of the file header in bytes */
extern const size_t SXBP_FILE_HEADER_SIZE;
/** @brief The size in bytes of one line when stored in the file */
extern const size_t SXBP_LINE_T_PACK_SIZE;

/**
 * @brief De-serialises a spiral from a buffer.
 * @details Reads in a binary representation of a spiral and populates a given
 * spiral with the data which represents this spiral (if input data is valid).
 *
 * @param buffer The data buffer to load the spiral from.
 * @param spiral [out] The spiral to write the spiral data to.
 * @return For information on return values, see the documentation of the return
 * types.
 *
 * @note Asserts:
 * - That buffer.bytes is not NULL
 * - That spiral->lines is NULL
 *
 * @see sxbp_status_t for generic error return codes and
 * sxbp_deserialise_diagnostic_t for file-specific error return codes.
 */
sxbp_serialise_result_t sxbp_load_spiral(
    sxbp_buffer_t buffer, sxbp_spiral_t* spiral
);

/**
 * @brief Serialises a spiral to a buffer.
 * @details Writes out a binary representation of a given spiral to a buffer,
 * which can then be stored in a file or sent across the network as required.
 *
 * @param spiral The spiral which should be serialised to buffer.
 * @param buffer [out] The data buffer to write out the spiral data to.
 * @return For information on return values, see the documentation of the return
 * types.
 *
 * @note Asserts:
 * - That spiral.lines is not NULL
 * - That buffer->bytes is NULL
 *
 * @see sxbp_status_t for generic error return codes and
 * sxbp_deserialise_diagnostic_t for file-specific error return codes.
 *
 * @todo The return type of this function could be changed to sxbp_status_t, as
 * none of the serialisation-specific error codes can ever be returned by it.
 */
sxbp_serialise_result_t sxbp_dump_spiral(
    sxbp_spiral_t spiral, sxbp_buffer_t* buffer
);

#ifdef __cplusplus
} // extern "C"
#endif

// end of header file
#endif
