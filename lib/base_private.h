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

#ifndef TOPIARY_BASE_PRIVATE_H
#define TOPIARY_BASE_PRIVATE_H 1

#include "config.h"

#include <stddef.h>             // size_t

#include "reference.h"
#include "util_private.h"
#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "array_private.h"
#include "opt_private.h"

struct _topiary_base_data_t {
    int id;
    long rawread_time;
    long map_time;
    long hash_time;
    long normalize_time;
    char *lines;
    size_t buflen;
    int nbytes;
    _TOPIARY_ARRAY_T *array;
    _TOPIARY_RESULTS_T *results;
};
typedef struct _topiary_base_data_t _TOPIARY_BASE_DATA_T;

enum _topiary_base_type_t {
    _TOPIARY_BASE_TYPE_VANILLA = 0,
    _TOPIARY_BASE_TYPE_PREVIOUS,
    _TOPIARY_BASE_TYPE_MEMOIZE,
    _TOPIARY_BASE_TYPE_SIZE,    // must be last
};

typedef enum _topiary_base_type_t _TOPIARY_BASE_TYPE_T;

char *_topiary_base_typestr(_TOPIARY_BASE_TYPE_T type, char *buf, size_t len);

typedef struct _topiary_base_t _TOPIARY_BASE_T;
typedef int (*_TOPIARY_BASE_METHOD) (_TOPIARY_BASE_T *base);

struct _topiary_base_t {
    _TOPIARY_BASE_METHOD rawread;
    _TOPIARY_BASE_METHOD normalize;
    _TOPIARY_BASE_METHOD read;
    _TOPIARY_BASE_METHOD memoize_rawread;
    _TOPIARY_BASE_METHOD memoize_normalize;
    _TOPIARY_BASE_METHOD memoize_read;
    char **filenames;
    size_t nfiles;
    char *fileprefix;
    char *fileglob;
    char *filesuffix;
    unsigned long count;
    _TOPIARY_BASE_TYPE_T type;
    _TOPIARY_BASE_DATA_T *current;
    _TOPIARY_BASE_DATA_T *previous;
    _TOPIARY_BASE_DATA_T data[2];
    TOPIARY_OPT_T *optional;
};

int _topiary_base_new(_TOPIARY_BASE_T **base,
                      char *tag,
                      _TOPIARY_BASE_TYPE_T type,
                      _TOPIARY_BASE_METHOD rawread,
                      _TOPIARY_BASE_METHOD normalize,
                      _TOPIARY_BASE_METHOD read,
                      size_t buflen, _TOPIARY_LIMITS_T *limits);

int _topiary_base_free(_TOPIARY_BASE_T **baseptr);
int _topiary_base_size(_TOPIARY_BASE_T *baseptr, size_t * size);

#define _TOPIARY_BASE_FREE(b) _topiary_base_free(&b)

int _topiary_base_set_filenames(_TOPIARY_BASE_T *base, char **filenames,
                                size_t nfiles);
int _topiary_base_set_fileprefix(_TOPIARY_BASE_T *base, char *fileprefix);
int _topiary_base_set_fileglob(_TOPIARY_BASE_T *base, char *fileglob);
int _topiary_base_set_filesuffix(_TOPIARY_BASE_T *base, char *filesuffix);
int _topiary_base_set_optional(_TOPIARY_BASE_T *base, TOPIARY_OPT_T *optional);
int _topiary_base_read(_TOPIARY_BASE_T *base);
int _topiary_base_rawread(_TOPIARY_BASE_T *base);
int _topiary_base_normalize(_TOPIARY_BASE_T *base);
int _topiary_base_print(_TOPIARY_BASE_T *base);
int _topiary_base_store_previous(_TOPIARY_BASE_T *base);
int _topiary_base_memoize(_TOPIARY_BASE_T *base);
int _topiary_base_unmemoize(_TOPIARY_BASE_T *base);
int _topiary_base_timeval_diff(_TOPIARY_BASE_T *base, float *tdiff);

#endif                          // TOPIARY_BASE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
