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

/* This file is a re-implementation of the 'list' type from Python
 */

#ifndef TOPIARY_ARRAY_PRIVATE_H
#define TOPIARY_ARRAY_PRIVATE_H 1

#include "vector_private.h"
#include "limits_private.h"

struct _topiary_array_t {
    _TOPIARY_LIMITS_T *limits;
    _TOPIARY_VECTOR_T *vector;
    char *data;
};
typedef struct _topiary_array_t _TOPIARY_ARRAY_T;

int _topiary_array_new(_TOPIARY_ARRAY_T **array, _TOPIARY_LIMITS_T *limits);

int _topiary_array_free(_TOPIARY_ARRAY_T **arrayptr);

#define _TOPIARY_ARRAY_FREE(a) _topiary_array_free(&a)

int _topiary_array_size(_TOPIARY_ARRAY_T *array, size_t * size);
int _topiary_array_set(_TOPIARY_ARRAY_T *array, size_t idx[],
                       size_t dim, char *val);
int _topiary_array_set_last(_TOPIARY_ARRAY_T *array,
                            size_t idx[], size_t dim, char *val);
int _topiary_array_get(_TOPIARY_ARRAY_T *array, size_t idx[],
                       size_t dim, char **value);

int _topiary_array_diff(_TOPIARY_ARRAY_T *previous,
                        _TOPIARY_ARRAY_T *current,
                        size_t idx[], size_t dim, int *diff);

typedef int (*_TOPIARY_ARRAY_ITERATE_FUNC) (char *val,
                                            void *data,
                                            size_t idx[], size_t dim);

int _topiary_array_iterate(_TOPIARY_ARRAY_T *array,
                           void *data,
                           int allocated, _TOPIARY_ARRAY_ITERATE_FUNC func);

int _topiary_array_print(_TOPIARY_ARRAY_T *array, int allocated);

#endif                          // TOPIARY_ARRAY_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
