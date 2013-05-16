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
#include <lnxproc/base.h>
#ifdef __cplusplus
extern "C" {
#endif

    typedef LNXPROC_ERROR_T (*LNXPROC_BASE_METHOD) (LNXPROC_BASE_T *base);

    LNXPROC_ERROR_T _lnxproc_base_free(LNXPROC_BASE_T **baseptr);

#define _LNXPROC_BASE_FREE(b) {\
    _lnxproc_base_free(&b);\
}

    LNXPROC_ERROR_T _lnxproc_base_read(LNXPROC_BASE_T *base);

    struct lnxproc_base_data_t {
        int id;
        long rawread_time;
        long map_time;
        long normalize_time;
        struct timeval tv;
        char *lines;
        size_t buflen;
        int nbytes;
        _LNXPROC_ARRAY_T *array;
        _LNXPROC_RESULTS_T *results;
    };

    struct lnxproc_base_t {
        LNXPROC_BASE_METHOD rawread;
        LNXPROC_BASE_METHOD normalize;
        LNXPROC_BASE_METHOD read;
        LNXPROC_BASE_METHOD memoize_rawread;
        LNXPROC_BASE_METHOD memoize_normalize;
        LNXPROC_BASE_METHOD memoize_read;
        char **filenames;
        size_t nfiles;
        char *fileprefix;
        char *fileglob;
        char *filesuffix;
        LNXPROC_BASE_DATA_T *current;
        LNXPROC_BASE_DATA_T *previous;
        LNXPROC_BASE_DATA_T data[2];
    };

    LNXPROC_ERROR_T _lnxproc_base_new(LNXPROC_BASE_T **base,
                                      char **filenames,
                                      size_t nfiles,
                                      char *fileprefix,
                                      char *fileglob,
                                      char *filesuffix,
                                      LNXPROC_BASE_METHOD rawread,
                                      LNXPROC_BASE_METHOD normalize,
                                      LNXPROC_BASE_METHOD read,
                                      size_t buflen,
                                      _LNXPROC_LIMITS_T limits[], size_t dim);

    LNXPROC_ERROR_T _lnxproc_base_rawread(LNXPROC_BASE_T *base);
    LNXPROC_ERROR_T _lnxproc_base_normalize(LNXPROC_BASE_T *base);

    LNXPROC_ERROR_T _lnxproc_base_print(LNXPROC_BASE_T *base);

#ifdef LNXPROC_UNUSED
    LNXPROC_ERROR_T _lnxproc_base_variable_usage(LNXPROC_BASE_T *base,
                                                 size_t idx[], size_t dim,
                                                 float scale, char *buf,
                                                 size_t len);

    LNXPROC_ERROR_T _lnxproc_base_variable_rate(LNXPROC_BASE_T *base,
                                                size_t idx[], size_t dim,
                                                long tdiff, float scale,
                                                char *buf, size_t len);

    LNXPROC_ERROR_T _lnxproc_base_variable_sample_rate(LNXPROC_BASE_T *base,
                                                       size_t idx[], size_t dim,
                                                       float scale, char *buf,
                                                       size_t len);
#endif

    LNXPROC_ERROR_T _lnxproc_base_store_previous(LNXPROC_BASE_T *base);

    LNXPROC_ERROR_T _lnxproc_base_memoize(LNXPROC_BASE_T *base);
    LNXPROC_ERROR_T _lnxproc_base_unmemoize(LNXPROC_BASE_T *base);
    LNXPROC_ERROR_T _lnxproc_base_timeval_diff(LNXPROC_BASE_T *base,
                                               float *tdiff);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
