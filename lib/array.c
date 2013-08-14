/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* This file is a re-implementation of the 'list' type from Python
 */
#include "config.h"

#include <string.h>             //memset

#include "allocate.h"
#include "reference.h"
#include "val2str.h"
#include "error_private.h"
#include "vector_private.h"
#include "limits_private.h"
#include "array_private.h"

static _TOPIARY_VECTOR_T *
array_create(_TOPIARY_LIMITS_T *limits, int depth)
{
    _TOPIARY_DEBUG("limits %p\n", limits);
    _TOPIARY_DEBUG("dim %zd\n", limits->dim);
    _TOPIARY_DEBUG("depth %d\n", depth);

    _TOPIARY_LIMITS_ROW_T *limit = limits->row + depth;

    _TOPIARY_DEBUG("limit %p\n", limit);
#ifdef DEBUG
    char buf[64];

    _topiary_limit_print(limit, buf, sizeof buf);
    _TOPIARY_DEBUG("limit %s\n", buf);
#endif

    int recursive = depth < limits->dim - 1 ? 1 : 0;

    _TOPIARY_DEBUG("recursive %d\n", recursive);

    _TOPIARY_VECTOR_T *wvec = NULL;

    int ret = _topiary_vector_new(&wvec, limit->expected, recursive);

    if (ret) {
        return NULL;
    }

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < limit->expected; i++) {
            _TOPIARY_VECTOR_T *f = array_create(limits, depth);

            if (!f) {
                _TOPIARY_DEBUG("Create array failed\n");
                return NULL;
            }
            ret = _topiary_vector_set_last_child(wvec, i, f);
            if (ret) {
                return NULL;
            }
        }
    }
    return wvec;
}

int
_topiary_array_new(_TOPIARY_ARRAY_T **array, _TOPIARY_LIMITS_T *limits)
{

    _TOPIARY_DEBUG("Array * %p Limits %p\n", array, limits);
    _TOPIARY_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _TOPIARY_DEBUG("sizeof _TOPIARY_ARRAY_T %lu\n", sizeof(_TOPIARY_ARRAY_T));

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (*array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!limits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    _TOPIARY_ARRAY_T *p = Allocate(NULL, sizeof(_TOPIARY_ARRAY_T));

    if (!p) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Array");
        return TOPIARY_ERROR_MALLOC;
    }

    p->limits = Acquire(limits, 0);
    //int ret = _topiary_limits_dup(&p->limits, limits);

    //if (ret) {
    //    _TOPIARY_ARRAY_FREE(p);
    //    return ret;
    //}

    if (limits->dim > 0) {
        p->vector = array_create(limits, 0);
        if (!p->vector) {
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Array");
            _TOPIARY_ARRAY_FREE(p);
            return TOPIARY_ERROR_MALLOC;
        }
    }
    else {
        _TOPIARY_DEBUG("Scalar\n");

        p->vector = NULL;
        int ret = _topiary_vector_new(&p->vector, 1, 0);

        if (ret) {
            _TOPIARY_ARRAY_FREE(p);
            return ret;
        }
    }

    *array = p;
    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_array_size(_TOPIARY_ARRAY_T *array, size_t * size)
{
    if (!size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Size");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size += sizeof(*array);
    size_t s;

    if (array->limits) {
        _topiary_limits_size(array->limits, &s);
        *size += s;
    }
    if (array->vector) {
        _topiary_vector_size(array->vector, &s);
        *size += s;
    }
    return TOPIARY_OK;
}

int
_topiary_array_free(_TOPIARY_ARRAY_T **arrayptr)
{
    _TOPIARY_DEBUG("Array %p\n", arrayptr);

    if (arrayptr && *arrayptr) {
        _TOPIARY_ARRAY_T *array = *arrayptr;

        _TOPIARY_LIMITS_FREE(array->limits);
        _TOPIARY_VECTOR_FREE(array->vector);

        _TOPIARY_DEBUG("Free array %p\n", array);
        DESTROY(array);
        *arrayptr = NULL;
    }

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

#ifdef DEBUG
static  inline void
debug_idx(size_t idx[], size_t dim) {
    char buf[32] = "";
    int i;
    int s = sizeof buf;
    char *c = buf;
    for( i=0; i < dim; i++ ) {
        int n = sizet2str(idx[i],c, s);
        c += n;
        s -= n;
    }
    _TOPIARY_DEBUG("Idx %s Dim %zd\n", buf, dim);
}
#else
#define debug_idx(iii,ddd)
#endif

int
_topiary_array_set(_TOPIARY_ARRAY_T *array, size_t idx[], size_t dim, char *val)
{
#ifdef DEBUG
    _TOPIARY_DEBUG("Array %p\n", array);
    debug_idx(idx, dim);
    _TOPIARY_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!idx || dim == 0 || dim != array->limits->dim) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Array: Incorrect index");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    _TOPIARY_VECTOR_T *saved[dim];

    saved[0] = array->vector;

    int ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        _TOPIARY_DEBUG("Saved[%d] = %p\n", i, saved[i]);
        _TOPIARY_VECTOR_T *f = NULL;

        if (idx[i] < saved[i]->size) {
            ret = _topiary_vector_child(saved[i], idx[i], &f);
            if (ret) {
                return ret;
            }
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret =
                _topiary_vector_new(&f, array->limits->row[j].expected,
                                    recursive);
            if (ret) {
                return ret;
            }
            ret = _topiary_vector_set_child(saved[i], idx[i], f);
            if (ret) {
                return ret;
            }
        }
        saved[j] = f;
        _TOPIARY_DEBUG("Saved[%d] = %p\n", j, saved[j]);
    }
    ret = _topiary_vector_set_value(saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        return ret;
    }

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_array_set_last(_TOPIARY_ARRAY_T *array, size_t idx[], size_t dim,
                        char *val)
{
#ifdef DEBUG
    _TOPIARY_DEBUG("Array %p\n", array);
    debug_idx(idx, dim);
    _TOPIARY_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (dim != array->limits->dim) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Array Arg dim = %zd, dim = %zd", dim,
                             array->limits->dim);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (dim == 0) {
        int ret = _topiary_vector_set_last_value(array->vector, 0, val);

        if (ret) {
            return ret;
        }

        _TOPIARY_DEBUG("Success\n");
        return TOPIARY_OK;
    }

    _TOPIARY_VECTOR_T *saved[dim];

    saved[0] = array->vector;
    _TOPIARY_DEBUG("Saved[%d] = %p\n", 0, saved[0]);

    int ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        _TOPIARY_VECTOR_T *f = NULL;

        if (idx[i] < saved[i]->size) {
            ret = _topiary_vector_child(saved[i], idx[i], &f);
            if (ret) {
                return ret;
            }
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret =
                _topiary_vector_new(&f, array->limits->row[j].expected,
                                    recursive);
            if (ret) {
                return ret;
            }
            ret = _topiary_vector_set_last_child(saved[i], idx[i], f);
            if (ret) {
                return ret;
            }
        }
        else {
            _topiary_vector_set_length(saved[i], idx[i]);
        }
        saved[j] = f;
        _TOPIARY_DEBUG("Saved[%d] = %p\n", j, saved[j]);
    }
    ret = _topiary_vector_set_last_value(saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        return ret;
    }

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_array_get(_TOPIARY_ARRAY_T *array, size_t idx[], size_t dim,
                   char **value)
{
#ifdef DEBUG
    _TOPIARY_DEBUG("Array %p\n", array);
    debug_idx(idx, dim);
#endif

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (dim != array->limits->dim) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Array Arg dim = %zd, dim = %zd", dim,
                             array->limits->dim);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int ret;

    if (dim == 0) {
        ret = _topiary_vector_value(array->vector, 0, value);
        if (ret) {
            return ret;
        }
        _TOPIARY_DEBUG("Success\n");
        return TOPIARY_OK;
    }

    _TOPIARY_VECTOR_T *saved[dim];

    saved[0] = array->vector;

    int i;
    int j = 0;

    for (i = 1; i < array->limits->dim; i++) {
        j = i - 1;
        _TOPIARY_VECTOR_T *f = NULL;

        ret = _topiary_vector_child(saved[j], idx[j], &f);

        if (ret) {
            return ret;
        }
        saved[i] = f;
    }
    ret = _topiary_vector_value(saved[dim - 1], idx[dim - 1], value);
    if (ret) {
        return ret;
    }

    _TOPIARY_DEBUG("Success (val = %1$p '%1$s')\n", *value);
    return TOPIARY_OK;
}

int
_topiary_array_diff(_TOPIARY_ARRAY_T *previous,
                    _TOPIARY_ARRAY_T *current,
                    size_t idx[], size_t dim, int *diff)
{
    int ret;

    *diff = 0;
    char *prev = NULL;

    ret = _topiary_array_get(previous, idx, dim, &prev);
    if (ret) {
        return ret;
    }
    char *curr = NULL;

    ret = _topiary_array_get(current, idx, dim, &curr);
    if (ret) {
        return ret;
    }
    *diff = atoi(curr) - atoi(prev);
    return TOPIARY_OK;
}

struct array_iterate_t {
    int allocated;
    char *data;
    size_t *idx;
    int dim;
    _TOPIARY_ARRAY_ITERATE_FUNC func;
};

static int
array_vector_iterate_func(_TOPIARY_VECTOR_T *vector, int idx, int depth,
                          void *data)
{

    _TOPIARY_DEBUG("Vector %p Idx %d Parent %p Depth %d Data %p\n", vector, idx,
                   vector->parent, depth, data);

    struct array_iterate_t *adata = data;
    size_t *aidx = adata->idx;
    size_t adim = adata->dim;

#ifdef DEBUG
    _TOPIARY_DEBUG("Adata: Data %p\n", adata->data);
    debug_idx(aidx, adim);
#endif

    int ret = TOPIARY_OK;

    if (vector->parent && (depth > 0)) {
        aidx[depth - 1] = idx;
        _TOPIARY_DEBUG("parent aidx[%d] = %zd\n", depth - 1, aidx[depth - 1]);
    }
    if (!vector->recursive && vector->values && vector->length > 0) {
        int i;

        for (i = 0; i < vector->length; i++) {
            aidx[depth] = i;
            ret = adata->func(vector->values[i], adata->data, aidx, adim);
        }
    }
    return ret;
}

int
_topiary_array_iterate(_TOPIARY_ARRAY_T *array,
                       void *data, int allocated,
                       _TOPIARY_ARRAY_ITERATE_FUNC func)
{
    _TOPIARY_DEBUG("Array %p Data %p Func %p\n", array, data, func);

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int ret;

    if (array->limits->dim > 0) {
        size_t idx[array->limits->dim];

        memset(idx, 0, array->limits->dim * sizeof(size_t));

        struct array_iterate_t adata = {
            .allocated = allocated,
            .data = data,
            .idx = idx,
            .dim = array->limits->dim,
            .func = func,
        };

        ret =
            _topiary_vector_iterate(array->vector, 0, 0, allocated, &adata,
                                    array_vector_iterate_func);

        if (ret) {
            return ret;
        }
    }
    else {
        size_t idx = 0;
        char *val = NULL;

        ret = _topiary_vector_value(array->vector, idx, &val);
        if (ret) {
            return ret;
        }

        ret = func(val, NULL, &idx, 1);
        if (ret) {
            return ret;
        }
    }

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

static int
array_print_internal(char *val, void *data, size_t idx[], size_t dim)
{
#ifdef DEBUG
    _TOPIARY_DEBUG("Val %p '%s' Data %p\n", val, val, data);
    debug_idx(idx, dim);
#endif
    int i;

    printf("--> ");
    for (i = 0; i < dim; i++) {
        printf("%zd ", idx[i]);
    }
    printf(":%1$p '%1$s'\n", val);

    return TOPIARY_OK;
}

int
_topiary_array_print(_TOPIARY_ARRAY_T *array, int allocated)
{

    _TOPIARY_DEBUG("Array %p Alloc %d\n", array, allocated);

    printf("Array at %p\n", array);

    if (!array) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Array");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int ret;

    printf("Array limits at  %p\n", array->limits);
    _topiary_limits_print(array->limits);
    ret = _topiary_array_iterate(array, NULL, allocated, array_print_internal);
    if (ret) {
        return ret;
    }

    printf("\n");
    return TOPIARY_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
