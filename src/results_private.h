/*
 * This file is part of topiary.
 *
 *  topiary is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  topiary is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with topiary.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#ifndef TOPIARY_RESULTS_PRIVATE_H
#define TOPIARY_RESULTS_PRIVATE_H 1

#include <sys/time.h>           // struct timeval

#include "print_private.h"

#ifdef DEBUG
#define HASH_DEBUG 1
#endif
#include "uthash.h"

#define _TOPIARY_RESULTS_TABLE_KEYLEN 48
#define _TOPIARY_RESULTS_TABLE_VALLEN 32

union _topiary_results_table_value_t {
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    float f;
    char s[_TOPIARY_RESULTS_TABLE_VALLEN];
    char *sptr;
    void *ptr;
};

typedef union _topiary_results_table_value_t _TOPIARY_RESULTS_TABLE_VALUE_T;

enum _topiary_results_table_valuetype_t {
    _TOPIARY_RESULTS_TABLE_VALUETYPE_NONE = 0,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_INT,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_INT,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_LONG,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_FLOAT,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_STR,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREFS,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_PTR,
    _TOPIARY_RESULTS_TABLE_VALUETYPE_FIXED,
};
typedef enum _topiary_results_table_valuetype_t
    _TOPIARY_RESULTS_TABLE_VALUETYPE_T;

struct _topiary_results_table_t {
    size_t keylen;
    int width;
    int precision;
    char key[_TOPIARY_RESULTS_TABLE_KEYLEN];
    _TOPIARY_RESULTS_TABLE_VALUETYPE_T valuetype;
    _TOPIARY_RESULTS_TABLE_VALUE_T value;
    UT_hash_handle hh;
};
typedef struct _topiary_results_table_t _TOPIARY_RESULTS_TABLE_T;

int _topiary_results_table_valuestr(_TOPIARY_RESULTS_TABLE_T *entry,
                                    char *buf, size_t len, char **res);

struct _topiary_results_t {
    int error;
    struct timeval tv;
    char *tag;
    long jiffies_per_sec;
    float secs_per_jiffy;
    long page_size;
    size_t size;
    size_t length;
    _TOPIARY_RESULTS_TABLE_T *table;
    _TOPIARY_RESULTS_TABLE_T *hash;
};
typedef struct _topiary_results_t _TOPIARY_RESULTS_T;

int _topiary_results_new(_TOPIARY_RESULTS_T **resultsptr, char *tag);

int _topiary_results_free(_TOPIARY_RESULTS_T **resultsptr);
void _topiary_results_release(void *arg);

#define _TOPIARY_RESULTS_FREE(r) _topiary_results_free(&r)

int _topiary_results_size(_TOPIARY_RESULTS_T *results, size_t * size);
int _topiary_results_print(_TOPIARY_RESULTS_T *results, int fd,
                           TOPIARY_PRINT_T print);
int _topiary_results_hash(_TOPIARY_RESULTS_T *results);

int _topiary_results_init(_TOPIARY_RESULTS_T *results, size_t nentries);

int _topiary_results_fetch(_TOPIARY_RESULTS_T *results, char *key,
                           _TOPIARY_RESULTS_TABLE_T **entry);
int _topiary_results_last(_TOPIARY_RESULTS_T *results,
                          _TOPIARY_RESULTS_TABLE_T **entry);

int _topiary_results_add_int(_TOPIARY_RESULTS_T *results,
                             const char *key, const int value);
int _topiary_results_add_unsigned_int(_TOPIARY_RESULTS_T *results,
                                      const char *key,
                                      const unsigned int value);
int _topiary_results_add_long(_TOPIARY_RESULTS_T *results, const char *key,
                              const long value);
int _topiary_results_add_unsigned_long(_TOPIARY_RESULTS_T *results,
                                       const char *key,
                                       const unsigned long value);
int _topiary_results_add_float(_TOPIARY_RESULTS_T *results, const char *key,
                               const float value);
int _topiary_results_add_fixed(_TOPIARY_RESULTS_T *results, const char *key,
                               const float value, const int width,
                               const int precision);
int _topiary_results_add_string(_TOPIARY_RESULTS_T *results, const char *key,
                                const char *value, size_t valuelen);

int _topiary_results_add_stringref(_TOPIARY_RESULTS_T *results,
                                   const char *key, const char *value);

int _topiary_results_add_ptr(_TOPIARY_RESULTS_T *results,
                             const char *key, const void *value);

typedef int (*_TOPIARY_RESULTS_ITERATE_FUNC) (_TOPIARY_RESULTS_T *results,
                                              _TOPIARY_RESULTS_TABLE_T *entry,
                                              void *data);
int _topiary_results_iterate(_TOPIARY_RESULTS_T *results,
                             _TOPIARY_RESULTS_ITERATE_FUNC func, void *data);

#endif                          // TOPIARY_RESULTS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
