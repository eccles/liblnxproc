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

#ifndef LIBLNXPROC_VECTOR_PRIVATE_H
#define LIBLNXPROC_VECTOR_PRIVATE_H 1

#include "util_private.h"
#include "error_private.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

    typedef struct _lnxproc_vector_t _LNXPROC_VECTOR_T;

    struct _lnxproc_vector_t {
        size_t length;
        size_t size;
        int recursive;
        _LNXPROC_VECTOR_T *parent;
        _LNXPROC_VECTOR_T **children;
        char **values;
    };
    typedef struct _lnxproc_vector_t _LNXPROC_VECTOR_T;

    LNXPROC_ERROR_T _lnxproc_vector_new(_LNXPROC_VECTOR_T ** vec,
                                        size_t size, int recursive);

    _LNXPROC_VECTOR_T *_lnxproc_vector_free(_LNXPROC_VECTOR_T *
                                            vector) WARN_UNUSED;

#define _LNXPROC_VECTOR_FREE(v) {\
    v =  _lnxproc_vector_free(v);\
}

    LNXPROC_ERROR_T _lnxproc_vector_resize(_LNXPROC_VECTOR_T * vector,
                                           size_t size);

    LNXPROC_ERROR_T _lnxproc_vector_set_child(_LNXPROC_VECTOR_T * vector,
                                              size_t idx,
                                              _LNXPROC_VECTOR_T * child);
    LNXPROC_ERROR_T _lnxproc_vector_set_value(_LNXPROC_VECTOR_T * vector,
                                              size_t idx, char *val);

    LNXPROC_ERROR_T _lnxproc_vector_set_last_child(_LNXPROC_VECTOR_T * vector,
                                                   size_t idx,
                                                   _LNXPROC_VECTOR_T * child);
    LNXPROC_ERROR_T _lnxproc_vector_set_last_value(_LNXPROC_VECTOR_T * vector,
                                                   size_t idx, char *val);

    LNXPROC_ERROR_T _lnxproc_vector_set_length(_LNXPROC_VECTOR_T * vector,
                                               size_t idx);

    LNXPROC_ERROR_T _lnxproc_vector_child(_LNXPROC_VECTOR_T * vector,
                                          size_t idx,
                                          _LNXPROC_VECTOR_T ** child);
    LNXPROC_ERROR_T _lnxproc_vector_value(_LNXPROC_VECTOR_T * vector,
                                          size_t idx, char **value);

    typedef
    LNXPROC_ERROR_T (*_LNXPROC_VECTOR_ITERATE_FUNC) (_LNXPROC_VECTOR_T *
                                                     child, char *value,
                                                     int recursive, void *data,
                                                     size_t idx);

    LNXPROC_ERROR_T _lnxproc_vector_iterate(_LNXPROC_VECTOR_T * vector,
                                            void *data,
                                            int start,
                                            int end,
                                            _LNXPROC_VECTOR_ITERATE_FUNC func);

    LNXPROC_ERROR_T _lnxproc_vector_print(_LNXPROC_VECTOR_T * vector,
                                          int allocated, void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_VECTOR_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
