#ifndef TOPIARY_UTIL_H
#define TOPIARY_UTIL_H 1

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
 * @defgroup util The topiary utility API.
 *
 * Some utility functions.
 *
 * @section topiary_util Utility functions.
 *
 * @{
 */

#include <sys/time.h>
#include <stddef.h>

/**
 * @brief Calculate float representation of time from timeval struct.
 *
 * @param[in]  tv   Address of struct timeval.
 *
 * @return          float representation of time in seconds.
 *
 */
    float topiary_timeval_secs(struct timeval *tv);

/**
 * @brief Calculate long representation of time difference from timeval structs.
 *
 * @param[in]  start   Address of struct timeval start time.
 *
 * @param[in]  end   Address of struct timeval end time.
 *
 * @return          long representation of time in microseconds.
 *
 */
    long topiary_timeval_diff(struct timeval *start, struct timeval *end);

/**
 * @brief Calculate string representation of time from timeval struct.
 *
 * @param[in]  tv   Address of struct timeval.
 *
 * @param[out]  buf   Address of buffer into which to store result.
 *
 * @param[in]  len   Length of buf.
 *
 * @return          character representation of time in SSSSSS.UUUUUU seconds.
 *
 */
    char *topiary_timeval_print(struct timeval *tv, char *buf, size_t len);

/**
 * @brief Return current time.
 *
 * @return          current time in struct timeval.
 *
 */
    struct timeval topiary_timeval(void);

/** @} ******************************************************************/

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_UTIL_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
