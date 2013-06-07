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

#ifdef DEBUG
#define HASH_DEBUG 1
#endif
#include "uthash.h"

#define _LNXPROC_RESULTS_TABLE_KEYLEN 48
#define _LNXPROC_RESULTS_TABLE_VALLEN 32

union _lnxproc_results_table_value_t {
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    float f;
    char s[_LNXPROC_RESULTS_TABLE_VALLEN];
    char *sptr;
    void *ptr;
};

typedef union _lnxproc_results_table_value_t _LNXPROC_RESULTS_TABLE_VALUE_T;

enum _lnxproc_results_table_valuetype_t {
    _LNXPROC_RESULTS_TABLE_VALUETYPE_NONE = 0,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_INT,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_STR,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREFS,
    _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR,
};
typedef enum _lnxproc_results_table_valuetype_t
 _LNXPROC_RESULTS_TABLE_VALUETYPE_T;

struct _lnxproc_results_table_t {
    size_t keylen;
    char key[_LNXPROC_RESULTS_TABLE_KEYLEN];
    _LNXPROC_RESULTS_TABLE_VALUETYPE_T valuetype;
    _LNXPROC_RESULTS_TABLE_VALUE_T value;
    UT_hash_handle hh;
};
typedef struct _lnxproc_results_table_t _LNXPROC_RESULTS_TABLE_T;

char *_lnxproc_results_table_valuestr(_LNXPROC_RESULTS_TABLE_T * entry,
                                      char *buf, size_t len, int copy);

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

int _lnxproc_results_new(_LNXPROC_RESULTS_T ** resultsptr, char *tag);

int _lnxproc_results_free(_LNXPROC_RESULTS_T ** resultsptr);
void _lnxproc_results_release(void *arg);

#define _LNXPROC_RESULTS_FREE(r) _lnxproc_results_free(&r)

int _lnxproc_results_size(_LNXPROC_RESULTS_T * results, size_t * size);
int _lnxproc_results_print(_LNXPROC_RESULTS_T * results);
int _lnxproc_results_hash(_LNXPROC_RESULTS_T * results);

int _lnxproc_results_init(_LNXPROC_RESULTS_T * results, size_t nentries);

int _lnxproc_results_fetch(_LNXPROC_RESULTS_T * results, char *key,
                           _LNXPROC_RESULTS_TABLE_T ** entry);
int _lnxproc_results_last(_LNXPROC_RESULTS_T * results,
                          _LNXPROC_RESULTS_TABLE_T ** entry);

int _lnxproc_results_add_int(_LNXPROC_RESULTS_T * results,
                             const char *key, const int value);
int _lnxproc_results_add_long(_LNXPROC_RESULTS_T * results,
                              const char *key, const long value);
int _lnxproc_results_add_unsigned_long(_LNXPROC_RESULTS_T * results,
                                       const char *key,
                                       const unsigned long value);
int _lnxproc_results_add_float(_LNXPROC_RESULTS_T * results, const char *key,
                               const float value);
int _lnxproc_results_add_string(_LNXPROC_RESULTS_T * results, const char *key,
                                const char *value, size_t valuelen);

int _lnxproc_results_add_stringref(_LNXPROC_RESULTS_T * results,
                                   const char *key, const char *value);

int _lnxproc_results_add_ptr(_LNXPROC_RESULTS_T * results,
                             const char *key, const void *value);

typedef int (*_LNXPROC_RESULTS_ITERATE_FUNC) (_LNXPROC_RESULTS_T * results,
                                              _LNXPROC_RESULTS_TABLE_T *
                                              entry, void *data);
int _lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                             _LNXPROC_RESULTS_ITERATE_FUNC func, void *data);

#endif                          // LIBLNXPROC_RESULTS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
