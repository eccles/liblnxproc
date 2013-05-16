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

#ifndef LIBLNXPROC_RESULTS_PRIVATE_H
#define LIBLNXPROC_RESULTS_PRIVATE_H 1

#include "error_private.h"
#include "util_private.h"

#ifdef DEBUG
#define HASH_DEBUG 1
#endif
#include "uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct _lnxproc_results_table_t {
        char key[32];
        char value[32];
        UT_hash_handle hh;
    };
    typedef struct _lnxproc_results_table_t _LNXPROC_RESULTS_TABLE_T;

    struct _lnxproc_results_t {
        long jiffies_per_sec;
        float secs_per_jiffy;
        long page_size;
        size_t size;
        size_t length;
        _LNXPROC_RESULTS_TABLE_T *table;
        _LNXPROC_RESULTS_TABLE_T *hash;
    };
    typedef struct _lnxproc_results_t _LNXPROC_RESULTS_T;

    LNXPROC_ERROR_T _lnxproc_results_new(_LNXPROC_RESULTS_T ** newresults);

    LNXPROC_ERROR_T _lnxproc_results_free(_LNXPROC_RESULTS_T ** resultsptr);
#define _LNXPROC_RESULTS_FREE(r) {\
    _lnxproc_results_free(&r);\
}

    LNXPROC_ERROR_T _lnxproc_results_print(_LNXPROC_RESULTS_T * results);
    LNXPROC_ERROR_T _lnxproc_results_hash(_LNXPROC_RESULTS_T * results);

    LNXPROC_ERROR_T _lnxproc_results_init(_LNXPROC_RESULTS_T * results,
                                          size_t nentries);

    LNXPROC_ERROR_T _lnxproc_results_fetch(_LNXPROC_RESULTS_T * results,
                                           char **value, char *fmt, ...);

    LNXPROC_ERROR_T _lnxproc_results_add(_LNXPROC_RESULTS_T * results,
                                         const char *value, char *fmt, ...);

    typedef int (*_LNXPROC_RESULTS_ITERATE_FUNC) (char *key, char *value,
                                                  void *data);
    LNXPROC_ERROR_T _lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                                             _LNXPROC_RESULTS_ITERATE_FUNC func,
                                             void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_RESULTS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
