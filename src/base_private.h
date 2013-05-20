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

#ifndef LIBLNXPROC_BASE_PRIVATE_H
#define LIBLNXPROC_BASE_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/time.h>

#ifdef __cplusplus
}                               // extern "C"
#endif
#include "util_private.h"
#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "array_private.h"
#ifdef __cplusplus
extern "C" {
#endif

    struct _lnxproc_base_data_t {
        int id;
        long rawread_time;
        long map_time;
        long hash_time;
        long normalize_time;
        struct timeval tv;
        char *lines;
        size_t buflen;
        int nbytes;
        _LNXPROC_ARRAY_T *array;
        _LNXPROC_RESULTS_T *results;
    };
    typedef struct _lnxproc_base_data_t _LNXPROC_BASE_DATA_T;

    enum _lnxproc_base_type_t {
        _LNXPROC_BASE_TYPE_VANILLA = 0,
        _LNXPROC_BASE_TYPE_PREVIOUS,
        _LNXPROC_BASE_TYPE_MEMOIZE,
        _LNXPROC_BASE_TYPE_SIZE,        // must be last
    };

    typedef enum _lnxproc_base_type_t _LNXPROC_BASE_TYPE_T;

    char *_lnxproc_base_typestr(_LNXPROC_BASE_TYPE_T type, char *buf,
                                size_t len);

    typedef struct _lnxproc_base_t _LNXPROC_BASE_T;
    typedef LNXPROC_ERROR_T (*_LNXPROC_BASE_METHOD) (_LNXPROC_BASE_T * base);

    struct _lnxproc_base_t {
        _LNXPROC_BASE_METHOD rawread;
        _LNXPROC_BASE_METHOD normalize;
        _LNXPROC_BASE_METHOD read;
        _LNXPROC_BASE_METHOD memoize_rawread;
        _LNXPROC_BASE_METHOD memoize_normalize;
        _LNXPROC_BASE_METHOD memoize_read;
        char **filenames;
        size_t nfiles;
        char *fileprefix;
        char *fileglob;
        char *filesuffix;
        unsigned long count;
        _LNXPROC_BASE_TYPE_T type;
        _LNXPROC_BASE_DATA_T *current;
        _LNXPROC_BASE_DATA_T *previous;
        _LNXPROC_BASE_DATA_T data[2];
    };

    LNXPROC_ERROR_T _lnxproc_base_new(_LNXPROC_BASE_T ** base,
                                      _LNXPROC_BASE_TYPE_T type,
                                      char **filenames,
                                      size_t nfiles,
                                      char *fileprefix,
                                      char *fileglob,
                                      char *filesuffix,
                                      _LNXPROC_BASE_METHOD rawread,
                                      _LNXPROC_BASE_METHOD normalize,
                                      _LNXPROC_BASE_METHOD read,
                                      size_t buflen,
                                      _LNXPROC_LIMITS_T limits[], size_t dim);

    LNXPROC_ERROR_T _lnxproc_base_free(_LNXPROC_BASE_T ** baseptr);

#define _LNXPROC_BASE_FREE(b) {\
    _lnxproc_base_free(&b);\
}

    LNXPROC_ERROR_T _lnxproc_base_read(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_rawread(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_normalize(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_print(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_store_previous(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_memoize(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_unmemoize(_LNXPROC_BASE_T * base);
    LNXPROC_ERROR_T _lnxproc_base_timeval_diff(_LNXPROC_BASE_T * base,
                                               float *tdiff);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
