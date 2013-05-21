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

#ifndef LIBLNXPROC_LIMITS_PRIVATE_H
#define LIBLNXPROC_LIMITS_PRIVATE_H 1

#include "util_private.h"
#include "error_private.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct _lnxproc_limits_row_t {
        size_t expected;
        char *chars;
        int len;
    };
    typedef struct _lnxproc_limits_row_t _LNXPROC_LIMITS_ROW_T;

    struct _lnxproc_limits_t {
        size_t dim;
        _LNXPROC_LIMITS_ROW_T row[];
    };
    typedef struct _lnxproc_limits_t _LNXPROC_LIMITS_T;

    LNXPROC_ERROR_T _lnxproc_limits_new(_LNXPROC_LIMITS_T ** newlimits,
                                        size_t dim);
    LNXPROC_ERROR_T _lnxproc_limits_set(_LNXPROC_LIMITS_T * limits, int pos,
                                        size_t expected, char *chars, int len);

    LNXPROC_ERROR_T _lnxproc_limits_dup(_LNXPROC_LIMITS_T ** newlimits,
                                        _LNXPROC_LIMITS_T * limits);

    LNXPROC_ERROR_T _lnxproc_limits_free(_LNXPROC_LIMITS_T ** limits);

#define _LNXPROC_LIMITS_FREE(ll) {\
    _lnxproc_limits_free(&ll);\
}

    LNXPROC_ERROR_T _lnxproc_chars_print(char *chars, size_t nchars, char *buf,
                                         size_t buflen);

    LNXPROC_ERROR_T _lnxproc_limit_print(_LNXPROC_LIMITS_ROW_T * limit,
                                         char *buf, size_t buflen);
    LNXPROC_ERROR_T _lnxproc_limits_print(_LNXPROC_LIMITS_T * limits);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_LIMITS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
