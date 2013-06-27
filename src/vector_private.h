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

#ifndef TOPIARY_VECTOR_PRIVATE_H
#define TOPIARY_VECTOR_PRIVATE_H 1

typedef struct _topiary_vector_t _TOPIARY_VECTOR_T;
struct _topiary_vector_t {
    size_t length;
    size_t size;
    int recursive;
    _TOPIARY_VECTOR_T *parent;
    int idx;
    _TOPIARY_VECTOR_T **children;
    char **values;
};

int _topiary_vector_new(_TOPIARY_VECTOR_T **vec, size_t size, int recursive);

int _topiary_vector_free(_TOPIARY_VECTOR_T **vectorptr);

#define _TOPIARY_VECTOR_FREE(v) _topiary_vector_free(&v)

int _topiary_vector_size(_TOPIARY_VECTOR_T *vector, size_t * size);
int _topiary_vector_resize(_TOPIARY_VECTOR_T *vector, size_t size);

int _topiary_vector_set_child(_TOPIARY_VECTOR_T *vector,
                              size_t idx, _TOPIARY_VECTOR_T *child);
int _topiary_vector_set_value(_TOPIARY_VECTOR_T *vector, size_t idx, char *val);

int _topiary_vector_set_last_child(_TOPIARY_VECTOR_T *vector,
                                   size_t idx, _TOPIARY_VECTOR_T *child);
int _topiary_vector_set_last_value(_TOPIARY_VECTOR_T *vector,
                                   size_t idx, char *val);

int _topiary_vector_set_length(_TOPIARY_VECTOR_T *vector, size_t idx);

int _topiary_vector_child(_TOPIARY_VECTOR_T *vector,
                          size_t idx, _TOPIARY_VECTOR_T **child);
int _topiary_vector_value(_TOPIARY_VECTOR_T *vector, size_t idx, char **value);

typedef
int (*_TOPIARY_VECTOR_ITERATE_FUNC) (_TOPIARY_VECTOR_T *vector, int idx,
                                     int depth, void *data);

int _topiary_vector_iterate(_TOPIARY_VECTOR_T *vector,
                            int idx,
                            int depth,
                            int allocated,
                            void *data, _TOPIARY_VECTOR_ITERATE_FUNC func);

int _topiary_vector_print(_TOPIARY_VECTOR_T *vector, int allocated, void *data);

#endif                          // TOPIARY_VECTOR_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
