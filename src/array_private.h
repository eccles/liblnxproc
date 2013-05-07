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

/* This file is a re-implementation of the 'list' type from Python
 */

#ifndef LIBLNXPROC_ARRAY_PRIVATE_H
#define LIBLNXPROC_ARRAY_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "util_private.h"
#include "vector_private.h"
#include "limits_private.h"

    struct _lnxproc_array_t {
        _LNXPROC_LIMITS_T *limits;
        size_t dim;
        _LNXPROC_VECTOR_T **saved;
        _LNXPROC_VECTOR_T *vector;
    };
    typedef struct _lnxproc_array_t _LNXPROC_ARRAY_T;

    LNXPROC_ERROR_T _lnxproc_array_new(_LNXPROC_ARRAY_T ** array,
                                       _LNXPROC_LIMITS_T limits[], size_t dim);

    _LNXPROC_ARRAY_T *_lnxproc_array_free(_LNXPROC_ARRAY_T * array) WARN_UNUSED;

#define _LNXPROC_ARRAY_FREE(a) {\
    a = _lnxproc_array_free(a);\
}

    LNXPROC_ERROR_T _lnxproc_array_set(_LNXPROC_ARRAY_T * array, size_t idx[],
                                       size_t dim, char *val);
    LNXPROC_ERROR_T _lnxproc_array_set_last(_LNXPROC_ARRAY_T * array,
                                            size_t idx[], size_t dim,
                                            char *val);
    LNXPROC_ERROR_T _lnxproc_array_get(_LNXPROC_ARRAY_T * array, size_t idx[],
                                       size_t dim, char **value);

    LNXPROC_ERROR_T _lnxproc_array_diff(_LNXPROC_ARRAY_T * previous,
                                        _LNXPROC_ARRAY_T * current,
                                        size_t idx[], size_t dim, int *diff);

    typedef LNXPROC_ERROR_T (*_LNXPROC_ARRAY_ITERATE_FUNC) (char *val,
                                                            void *data,
                                                            size_t idx[],
                                                            size_t dim);

    LNXPROC_ERROR_T _lnxproc_array_iterate(_LNXPROC_ARRAY_T * array,
                                           void *data,
                                           _LNXPROC_ARRAY_ITERATE_FUNC func);

    LNXPROC_ERROR_T _lnxproc_array_print(_LNXPROC_ARRAY_T * array,
                                         int allocated);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ARRAY_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
