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

#ifndef LIBLNXPROC_LIMITS_H
#define LIBLNXPROC_LIMITS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"

    struct lnxproc_limits_t {
        size_t expected;
        char *chars;
        int len;
    };
    typedef struct lnxproc_limits_t LNXPROC_LIMITS_T;

    char *lnxproc_limit_chr(LNXPROC_LIMITS_T * limit, char c);

    char *lnxproc_chars_print(char *chars, size_t nchars, char *buf,
                              size_t buflen);

    char *lnxproc_limit_print(LNXPROC_LIMITS_T * limit, char *buf,
                              size_t buflen);
    int lnxproc_limits_print(LNXPROC_LIMITS_T limits[], int dim);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_LIMITS_T *lnxproc_limits_dup(LNXPROC_ERROR_CALLBACK
                                         callback,
                                         LNXPROC_LIMITS_T
                                         limits[], size_t dim) WARN_UNUSED;
    LNXPROC_LIMITS_T *lnxproc_limits_free(LNXPROC_LIMITS_T
                                          limits[], size_t dim) WARN_UNUSED;

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_LIMITS_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
