/*
 * This file is part of liblnxproc.
 *
 *  liblnxproc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  liblnxproc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

/* Common macros and utility functions
 */

#ifndef LIBLNXPROC_UTIL_H
#define LIBLNXPROC_UTIL_H 1

#include <sys/time.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    float lnxproc_timeval_secs(struct timeval *tv);
    long lnxproc_timeval_diff(struct timeval *start, struct timeval *end);
    char *lnxproc_timeval_print(struct timeval *tv, char *buf, size_t len);
    struct timeval lnxproc_timeval(void);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_UTIL_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
