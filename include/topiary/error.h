#ifndef TOPIARY_ERROR_H
#define TOPIARY_ERROR_H 1

/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup error The topiary error API.
 *
 * All topiary functions return an error code. Negative error codes are system
 * errors. Positive error codes are peculiar to topiary and are described here.
 *
 * @section topiary_error Error codes.
 *
 * @{
 */

/**
 * @brief Indicates error status.
 *
 * TOPIARY_OK is 0 and indicates success.
 *
 * TOPIARY_ERROR_MISMATCHED_STRINGS Internal error used to detect if error 
 *                                  functions are correctly set up. User should
 *                                  never see this error.
 *
 * TOPIARY_ERROR_MALLOC Memory allocation failure.
 *
 * TOPIARY_ERROR_ILLEGAL_ARG Function argument is illegal.
 *
 * TOPIARY_ERROR_BASE_READ_OVERFLOW Internal error that causes the library to
 *                                  grow internal buffers. User should never
 *                                  see this error.
 *
 * TOPIARY_ERROR_BASE_REGEX_FAILURE No files found that match regex.
 *
 * TOPIARY_ERROR_BASE_GLOB_FAILURE No files found that match glob.
 *
 * TOPIARY_ERROR_NOT_FOUND Value not found.
 *
 * TOPIARY_ERROR_BASE_READ_SSIZE_MAX Attempted to read more than SSIZE_MAX 
 *                                   bytes.
 *
 */
    enum topiary_error_t {
        TOPIARY_OK = 0,
        TOPIARY_ERROR_MISMATCHED_STRINGS,
        TOPIARY_ERROR_MALLOC,
        TOPIARY_ERROR_ILLEGAL_ARG,
        TOPIARY_ERROR_BASE_READ_OVERFLOW,
        TOPIARY_ERROR_BASE_REGEX_FAILURE,
        TOPIARY_ERROR_BASE_GLOB_FAILURE,
        TOPIARY_ERROR_NOT_FOUND,
        TOPIARY_ERROR_BASE_READ_SSIZE_MAX,
        TOPIARY_ERROR_SIZE   // must be last
    };

/**
 * @brief converts error code into string.
 *
 * @param[in]  err   Error code (negative is a system error).
 *
 * @param[out]  buf Buffer that will contain the string representation.
 *
 * @param[in]  buflen  Length of buffer.
 *
 * @return              Usually buf. However if the error is unknown returns
 *                      a pointer to a static character string internal to
 *                      the topiary library.
 */
    const char *topiary_strerror(int err, char *buf, size_t buflen);

/** @} ******************************************************************/

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_ERROR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
