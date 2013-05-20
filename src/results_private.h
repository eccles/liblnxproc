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

#include "util_private.h"
#include "error_private.h"

#ifdef DEBUG
#define HASH_DEBUG 1
#endif
#include "uthash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _LNXPROC_RESULTS_TABLE_KEYLEN 32
#define _LNXPROC_RESULTS_TABLE_VALLEN 32

    union _lnxproc_results_table_value_t {
        int i;
        unsigned int ui;
        long l;
        unsigned long ul;
        float f;
        char s[_LNXPROC_RESULTS_TABLE_VALLEN];
        void *p;
    };

    typedef union _lnxproc_results_table_value_t _LNXPROC_RESULTS_TABLE_VALUE_T;

    enum _lnxproc_results_table_valuetype_t {
        _LNXPROC_RESULTS_TABLE_VALUETYPE_INT = 0,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_STR,
        _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR,
    };
    typedef enum _lnxproc_results_table_valuetype_t
     _LNXPROC_RESULTS_TABLE_VALUETYPE_T;

    struct _lnxproc_results_table_t {
        char key[_LNXPROC_RESULTS_TABLE_KEYLEN];
        _LNXPROC_RESULTS_TABLE_VALUETYPE_T valuetype;
        _LNXPROC_RESULTS_TABLE_VALUE_T value;
        UT_hash_handle hh;
    };
    typedef struct _lnxproc_results_table_t _LNXPROC_RESULTS_TABLE_T;

    char *_lnxproc_results_table_valuestr(_LNXPROC_RESULTS_TABLE_T * entry,
                                          char *buf, size_t len);

    LNXPROC_ERROR_T _lnxproc_results_table_copy(_LNXPROC_RESULTS_TABLE_T * dest,
                                                _LNXPROC_RESULTS_TABLE_T * src);

    struct _lnxproc_results_t {
        char *tag;
        long jiffies_per_sec;
        float secs_per_jiffy;
        long page_size;
        size_t size;
        size_t length;
        _LNXPROC_RESULTS_TABLE_T *table;
        _LNXPROC_RESULTS_TABLE_T *hash;
    };
    typedef struct _lnxproc_results_t _LNXPROC_RESULTS_T;

    LNXPROC_ERROR_T _lnxproc_results_new(_LNXPROC_RESULTS_T ** newresults,
                                         char *tag);

    LNXPROC_ERROR_T _lnxproc_results_free(_LNXPROC_RESULTS_T ** resultsptr);
#define _LNXPROC_RESULTS_FREE(r) {\
    _lnxproc_results_free(&r);\
}

    LNXPROC_ERROR_T _lnxproc_results_print(_LNXPROC_RESULTS_T * results);
    LNXPROC_ERROR_T _lnxproc_results_hash(_LNXPROC_RESULTS_T * results);

    LNXPROC_ERROR_T _lnxproc_results_init(_LNXPROC_RESULTS_T * results,
                                          size_t nentries);

    LNXPROC_ERROR_T _lnxproc_results_fetch(_LNXPROC_RESULTS_T * results,
                                           _LNXPROC_RESULTS_TABLE_T * entry);

    LNXPROC_ERROR_T _lnxproc_results_add_int(_LNXPROC_RESULTS_T * results,
                                             const char *key, const int value);
    LNXPROC_ERROR_T _lnxproc_results_add_long(_LNXPROC_RESULTS_T * results,
                                              const char *key,
                                              const long value);
    LNXPROC_ERROR_T _lnxproc_results_add_float(_LNXPROC_RESULTS_T * results,
                                               const char *key,
                                               const float value);
    LNXPROC_ERROR_T _lnxproc_results_add_string(_LNXPROC_RESULTS_T * results,
                                                const char *key,
                                                const char *value);

    typedef int (*_LNXPROC_RESULTS_ITERATE_FUNC) (_LNXPROC_RESULTS_TABLE_T *
                                                  entry, void *data);
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
