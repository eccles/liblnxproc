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

/* This file is a re-implementation of the 'list' type from Python
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "vector_private.h"
#include "limits_private.h"
#include "array_private.h"

static _LNXPROC_VECTOR_T *
array_create(_LNXPROC_LIMITS_T * limits, int depth)
{
    _LNXPROC_DEBUG("limits %p\n", limits);
    _LNXPROC_DEBUG("dim %zd\n", limits->dim);
    _LNXPROC_DEBUG("depth %d\n", depth);

    _LNXPROC_LIMITS_ROW_T *limit = limits->row + depth;

    _LNXPROC_DEBUG("limit %p\n", limit);
#ifdef DEBUG
    char buf[64];

    _lnxproc_limit_print(limit, buf, sizeof buf);
    _LNXPROC_DEBUG("limit %s\n", buf);
#endif

    int recursive = depth < limits->dim - 1 ? 1 : 0;

    _LNXPROC_DEBUG("recursive %d\n", recursive);

    _LNXPROC_VECTOR_T *wvec = NULL;

    int ret = _lnxproc_vector_new(&wvec, limit->expected, recursive);

    if (ret) {
        return NULL;
    }

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < limit->expected; i++) {
            _LNXPROC_VECTOR_T *f = array_create(limits, depth);

            if (!f) {
                _LNXPROC_DEBUG("Create array failed\n");
                return NULL;
            }
            ret = _lnxproc_vector_set_last_child(wvec, i, f);
            if (ret) {
                return NULL;
            }
        }
    }
    return wvec;
}

int
_lnxproc_array_new(_LNXPROC_ARRAY_T ** array, _LNXPROC_LIMITS_T * limits)
{

    _LNXPROC_DEBUG("Array * %p Limits %p\n", array, limits);
    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_ARRAY_T %lu\n", sizeof(_LNXPROC_ARRAY_T));

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (*array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Limits");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_ARRAY_T *p = calloc(1, sizeof(_LNXPROC_ARRAY_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Array");
        return LNXPROC_ERROR_MALLOC;
    }

    int ret = _lnxproc_limits_dup(&p->limits, limits);

    if (ret) {
        _LNXPROC_ARRAY_FREE(p);
        return ret;
    }

    if (limits->dim > 0) {
        p->vector = array_create(limits, 0);
        if (!p->vector) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Array");
            _LNXPROC_ARRAY_FREE(p);
            return LNXPROC_ERROR_MALLOC;
        }
    }
    else {
        _LNXPROC_DEBUG("Scalar\n");

        p->vector = NULL;
        ret = _lnxproc_vector_new(&p->vector, 1, 0);
        if (ret) {
            _LNXPROC_ARRAY_FREE(p);
            return ret;
        }
    }

    *array = p;
    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
_lnxproc_array_free(_LNXPROC_ARRAY_T ** arrayptr)
{
    _LNXPROC_DEBUG("Array %p\n", arrayptr);

    if (arrayptr && *arrayptr) {
        _LNXPROC_ARRAY_T *array = *arrayptr;

        if (array->limits) {
            _LNXPROC_LIMITS_FREE(array->limits);
        }
        if (array->vector) {
            _LNXPROC_VECTOR_FREE(array->vector);
        }

        _LNXPROC_DEBUG("Free array %p\n", array);
        free(array);
        array = NULL;
        *arrayptr = NULL;
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

#ifdef DEBUG
#define DEBUG_IDX(iii,ddd) {\
    char buf[32] = "";\
    int i;\
    int s = sizeof buf;\
    char *c = buf;\
    for( i=0; i < (ddd); i++ ) {\
        int n = snprintf(c, s,"%zd,",(iii)[i]);\
        c += n;\
        s -= n;\
    }\
    _LNXPROC_DEBUG("Idx %s Dim %zd\n", buf, (ddd));\
}
#else
#define DEBUG_IDX(iii,ddd)
#endif

int
_lnxproc_array_set(_LNXPROC_ARRAY_T * array, size_t idx[], size_t dim,
                   char *val)
{
#ifdef DEBUG
    _LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    _LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array vector");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!idx || dim == 0 || dim != array->limits->dim) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Array: Incorrect index");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_VECTOR_T *saved[dim];

    saved[0] = array->vector;

    int ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        _LNXPROC_DEBUG("Saved[%d] = %p\n", i, saved[i]);
        _LNXPROC_VECTOR_T *f = NULL;

        if (idx[i] < saved[i]->size) {
            ret = _lnxproc_vector_child(saved[i], idx[i], &f);
            if (ret) {
                return ret;
            }
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret =
                _lnxproc_vector_new(&f, array->limits->row[j].expected,
                                    recursive);
            if (ret) {
                return ret;
            }
            ret = _lnxproc_vector_set_child(saved[i], idx[i], f);
            if (ret) {
                return ret;
            }
        }
        saved[j] = f;
        _LNXPROC_DEBUG("Saved[%d] = %p\n", j, saved[j]);
    }
    ret = _lnxproc_vector_set_value(saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        return ret;
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
_lnxproc_array_set_last(_LNXPROC_ARRAY_T * array, size_t idx[], size_t dim,
                        char *val)
{
#ifdef DEBUG
    _LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    _LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array vector");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (dim != array->limits->dim) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Array Arg dim = %zd, dim = %zd", dim,
                             array->limits->dim);
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (dim == 0) {
        int ret = _lnxproc_vector_set_last_value(array->vector, 0, val);

        if (ret) {
            return ret;
        }

        _LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    _LNXPROC_VECTOR_T *saved[dim];

    saved[0] = array->vector;
    _LNXPROC_DEBUG("Saved[%d] = %p\n", 0, saved[0]);

    int ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        _LNXPROC_VECTOR_T *f = NULL;

        if (idx[i] < saved[i]->size) {
            ret = _lnxproc_vector_child(saved[i], idx[i], &f);
            if (ret) {
                return ret;
            }
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret =
                _lnxproc_vector_new(&f, array->limits->row[j].expected,
                                    recursive);
            if (ret) {
                return ret;
            }
            ret = _lnxproc_vector_set_last_child(saved[i], idx[i], f);
            if (ret) {
                return ret;
            }
        }
        else {
            _lnxproc_vector_set_length(saved[i], idx[i]);
        }
        saved[j] = f;
        _LNXPROC_DEBUG("Saved[%d] = %p\n", j, saved[j]);
    }
    ret = _lnxproc_vector_set_last_value(saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        return ret;
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
_lnxproc_array_get(_LNXPROC_ARRAY_T * array, size_t idx[], size_t dim,
                   char **value)
{
#ifdef DEBUG
    _LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
#endif

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array vector");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (dim != array->limits->dim) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Array Arg dim = %zd, dim = %zd", dim,
                             array->limits->dim);
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    int ret;

    if (dim == 0) {
        ret = _lnxproc_vector_value(array->vector, 0, value);
        if (ret) {
            return ret;
        }
        _LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    _LNXPROC_VECTOR_T *saved[dim];

    saved[0] = array->vector;

    int i;
    int j = 0;

    for (i = 1; i < array->limits->dim; i++) {
        j = i - 1;
        _LNXPROC_VECTOR_T *f = NULL;

        ret = _lnxproc_vector_child(saved[j], idx[j], &f);

        if (ret) {
            return ret;
        }
        saved[i] = f;
    }
    ret = _lnxproc_vector_value(saved[dim - 1], idx[dim - 1], value);
    if (ret) {
        return ret;
    }

    _LNXPROC_DEBUG("Success (val = %1$p '%1$s')\n", *value);
    return LNXPROC_OK;
}

int
_lnxproc_array_diff(_LNXPROC_ARRAY_T * previous,
                    _LNXPROC_ARRAY_T * current,
                    size_t idx[], size_t dim, int *diff)
{
    int ret;

    *diff = 0;
    char *prev = NULL;

    ret = _lnxproc_array_get(previous, idx, dim, &prev);
    if (ret) {
        return ret;
    }
    char *curr = NULL;

    ret = _lnxproc_array_get(current, idx, dim, &curr);
    if (ret) {
        return ret;
    }
    *diff = atoi(curr) - atoi(prev);
    return LNXPROC_OK;
}

struct array_iterate_t {
    int allocated;
    char *data;
    size_t *idx;
    int dim;
    _LNXPROC_ARRAY_ITERATE_FUNC func;
};

static int
array_vector_iterate_func(_LNXPROC_VECTOR_T * vector, int idx, int depth,
                          void *data)
{

    _LNXPROC_DEBUG("Vector %p Idx %d Parent %p Depth %d Data %p\n", vector, idx,
                   vector->parent, depth, data);

    struct array_iterate_t *adata = data;
    size_t *aidx = adata->idx;
    size_t adim = adata->dim;

#ifdef DEBUG
    _LNXPROC_DEBUG("Adata: Data %p\n", adata->data);
    DEBUG_IDX(aidx, adim);
#endif

    int ret = LNXPROC_OK;

    if (vector->parent && (depth > 0)) {
        aidx[depth - 1] = idx;
        _LNXPROC_DEBUG("parent aidx[%d] = %zd\n", depth - 1, aidx[depth - 1]);
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
_lnxproc_array_iterate(_LNXPROC_ARRAY_T * array,
                       void *data, int allocated,
                       _LNXPROC_ARRAY_ITERATE_FUNC func)
{
    _LNXPROC_DEBUG("Array %p Data %p Func %p\n", array, data, func);

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!array->vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array vector");
        return LNXPROC_ERROR_ILLEGAL_ARG;
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
            _lnxproc_vector_iterate(array->vector, 0, 0, allocated, &adata,
                                    array_vector_iterate_func);

        if (ret) {
            return ret;
        }
    }
    else {
        size_t idx = 0;
        char *val = NULL;

        ret = _lnxproc_vector_value(array->vector, idx, &val);
        if (ret) {
            return ret;
        }

        ret = func(val, NULL, &idx, 1);
        if (ret) {
            return ret;
        }
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

static int
array_print_internal(char *val, void *data, size_t idx[], size_t dim)
{
#ifdef DEBUG
    _LNXPROC_DEBUG("Val %p '%s' Data %p\n", val, val, data);
    DEBUG_IDX(idx, dim);
#endif
    int i;

    printf("--> ");
    for (i = 0; i < dim; i++) {
        printf("%zd ", idx[i]);
    }
    printf(":%1$p '%1$s'\n", val);

    return LNXPROC_OK;
}

int
_lnxproc_array_print(_LNXPROC_ARRAY_T * array, int allocated)
{

    _LNXPROC_DEBUG("Array %p Alloc %d\n", array, allocated);

    printf("Array at %p\n", array);

    if (!array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    int ret;

    printf("Array limits at  %p\n", array->limits);
    _lnxproc_limits_print(array->limits);
    ret = _lnxproc_array_iterate(array, NULL, allocated, array_print_internal);
    if (ret) {
        return ret;
    }

    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
