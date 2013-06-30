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

/* Common macros and utility functions
 */

#ifndef TOPIARY_UTIL_H
#define TOPIARY_UTIL_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <stddef.h>

    float topiary_timeval_secs(struct timeval *tv);
    long topiary_timeval_diff(struct timeval *start, struct timeval *end);
    char *topiary_timeval_print(struct timeval *tv, char *buf, size_t len);
    struct timeval topiary_timeval(void);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_UTIL_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
