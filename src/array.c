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

#include "error.h"
#include "vector_private.h"
#include "limits.h"
#include "array_private.h"

static LNXPROC_VECTOR_T *
array_create(LNXPROC_LIMITS_T limits[], size_t dim, int depth)
{
    LNXPROC_DEBUG("limits %p\n", limits);
    LNXPROC_DEBUG("dim %d\n", dim);
    LNXPROC_DEBUG("depth %d\n", depth);

    LNXPROC_LIMITS_T *limit = limits + depth;

    LNXPROC_DEBUG("limit %p\n", limit);
#ifdef DEBUG
    char buf[64];

    lnxproc_limit_print(limit, buf, sizeof buf);
    LNXPROC_DEBUG("limit %s\n", buf);
#endif

    int recursive = depth < dim - 1 ? 1 : 0;

    LNXPROC_DEBUG("recursive %d\n", recursive);

    LNXPROC_VECTOR_T *wvec = NULL;

    LNXPROC_ERROR_T ret = lnxproc_vector_new(&wvec, limit->expected, recursive);

    if (ret) {
        LNXPROC_ERROR_DEBUG(ret, "Create array\n");
        return NULL;
    }

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < limit->expected; i++) {
            LNXPROC_VECTOR_T *f = array_create(limits, dim, depth);

            if (!f) {
                LNXPROC_DEBUG("Create array failed\n");
                return NULL;
            }
            ret = lnxproc_vector_set_last_child(wvec, i, f);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "Create array\n");
                return NULL;
            }
        }
    }
    return wvec;
}

LNXPROC_ERROR_T
lnxproc_array_new(LNXPROC_ARRAY_T **array, LNXPROC_LIMITS_T limits[],
                  size_t dim)
{

    LNXPROC_DEBUG("Array * %p Limits %p Dim %d\n", array, limits, dim);
    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_ADDRESS_NULL, "Malloc array\n");
        return LNXPROC_ERROR_ARRAY_ADDRESS_NULL;
    }

    if (*array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_ADDRESS_CONTENTS_NOT_NULL,
                            "Malloc array\n");
        return LNXPROC_ERROR_ARRAY_ADDRESS_CONTENTS_NOT_NULL;
    }

    if (!limits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_LIMITS_NULL;
    }

    LNXPROC_ARRAY_T *p = calloc(1, sizeof(LNXPROC_ARRAY_T));

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_MALLOC_HEADER,
                            "Malloc array\n");
        return LNXPROC_ERROR_ARRAY_MALLOC_HEADER;
    }

    if (dim > 0) {
        LNXPROC_ERROR_T ret = lnxproc_limits_dup(&p->limits, limits, dim);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "Malloc array\n");
            LNXPROC_ARRAY_FREE(p);
            return ret;
        }
        p->dim = dim;

        p->saved = calloc(dim, sizeof(LNXPROC_VECTOR_T *));
        if (!p->saved) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_SAVED_MALLOC,
                                "Malloc array\n");
            LNXPROC_ARRAY_FREE(p);
            return LNXPROC_ERROR_ARRAY_SAVED_MALLOC;
        }

        p->vector = array_create(limits, dim, 0);
        if (!p->vector) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_VECTOR_MALLOC,
                                "Malloc array\n");
            LNXPROC_ARRAY_FREE(p);
            return LNXPROC_ERROR_ARRAY_VECTOR_MALLOC;
        }
    }
    else {
        LNXPROC_DEBUG("Scalar\n");
        LNXPROC_ERROR_T ret = lnxproc_limits_dup(&p->limits, limits, 1);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "Malloc array\n");
            LNXPROC_ARRAY_FREE(p);
            return ret;
        }
        p->dim = 0;

        p->vector = NULL;
        ret = lnxproc_vector_new(&p->vector, 1, 0);
        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "Malloc array\n");
            LNXPROC_ARRAY_FREE(p);
            return ret;
        }
        p->saved = NULL;
    }

    *array = p;
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ARRAY_T *
lnxproc_array_free(LNXPROC_ARRAY_T *array)
{
    LNXPROC_DEBUG("Array %p\n", array);

    if (array) {
        if (array->limits) {
            LNXPROC_LIMITS_FREE(array->limits, array->dim < 1 ? 1 : array->dim);
        }
        if (array->saved) {
            free(array->saved);
            array->saved = NULL;
        }
        if (array->vector) {
            LNXPROC_VECTOR_FREE(array->vector);
        }

        LNXPROC_DEBUG("Free array %p\n", array);
        free(array);
        array = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return array;
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
    LNXPROC_DEBUG("Idx %s Dim %zd\n", buf, (ddd));\
}
#else
#define DEBUG_IDX(iii,ddd)
#endif

LNXPROC_ERROR_T
lnxproc_array_set(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim, char *val)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (!array->vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_VECTOR_NULL;
    }

    if (dim != array->dim) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION, "\n");
        return LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION;
    }

    if (dim == 0) {
        LNXPROC_ERROR_T ret = lnxproc_vector_set_value(array->vector, 0, val);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }

        LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    if (!array->saved) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_SAVED_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_SAVED_NULL;
    }

    array->saved[0] = array->vector;
    LNXPROC_DEBUG("Saved[%d] = %p\n", 0, array->saved[0]);

    LNXPROC_ERROR_T ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        LNXPROC_DEBUG("Saved[%d] = %p\n", i, array->saved[i]);
        LNXPROC_VECTOR_T *f = NULL;

        ret = lnxproc_vector_child(array->saved[i], idx[i], &f);
        if (ret && (ret != LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE)) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret = lnxproc_vector_new(&f, array->limits[j].expected, recursive);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
            ret = lnxproc_vector_set_child(array->saved[i], idx[i], f);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }
        array->saved[j] = f;
        LNXPROC_DEBUG("Saved[%d] = %p\n", j, array->saved[j]);
    }
    ret = lnxproc_vector_set_value(array->saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        LNXPROC_ERROR_DEBUG(ret, "\n");
        return ret;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_array_set_last(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim,
                       char *val)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (!array->vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_VECTOR_NULL;
    }

    if (dim != array->dim) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION, "\n");
        return LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION;
    }

    if (dim == 0) {
        LNXPROC_ERROR_T ret = lnxproc_vector_set_value(array->vector, 0, val);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }

        LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    array->saved[0] = array->vector;
    LNXPROC_DEBUG("Saved[%d] = %p\n", 0, array->saved[0]);

    LNXPROC_ERROR_T ret;
    int i;

    for (i = 0; i < dim - 1; i++) {
        LNXPROC_DEBUG("Saved[%d] = %p\n", i, array->saved[i]);
        LNXPROC_VECTOR_T *f = NULL;

        ret = lnxproc_vector_child(array->saved[i], idx[i], &f);
        if (ret && (ret != LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE)) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }

        int j = i + 1;

        if (!f) {
            int recursive = j < dim - 1 ? 1 : 0;

            ret = lnxproc_vector_new(&f, array->limits[j].expected, recursive);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
            ret = lnxproc_vector_set_last_child(array->saved[i], idx[i], f);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }
        array->saved[j] = f;
        LNXPROC_DEBUG("Saved[%d] = %p\n", j, array->saved[j]);
    }
    ret =
        lnxproc_vector_set_last_value(array->saved[dim - 1], idx[dim - 1], val);
    if (ret) {
        LNXPROC_ERROR_DEBUG(ret, "\n");
        return ret;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_array_get(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim,
                  char **value)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
#endif

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (!array->vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_VECTOR_NULL;
    }

    if (dim != array->dim) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION, "\n");
        return LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION;
    }

    LNXPROC_ERROR_T ret;

    if (dim == 0) {
        ret = lnxproc_vector_value(array->vector, 0, value);
        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
        LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    array->saved[0] = array->vector;

    int i;
    int j = 0;

    for (i = 1; i < array->dim; i++) {
        j = i - 1;
        LNXPROC_VECTOR_T *f = NULL;

        ret = lnxproc_vector_child(array->saved[j], idx[j], &f);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
        array->saved[i] = f;
    }
    ret = lnxproc_vector_value(array->saved[dim - 1], idx[dim - 1], value);
    if (ret) {
        LNXPROC_ERROR_DEBUG(ret, "\n");
        return ret;
    }

    LNXPROC_DEBUG("Success (val = %1$p '%1$s')\n", *value);
    return LNXPROC_OK;
}

struct array_iterate_t {
    char *data;
    size_t *idx;
    int dim;
    int depth;
    LNXPROC_ARRAY_ITERATE_FUNC func;
};

static LNXPROC_ERROR_T
array_vector_iterate_func(LNXPROC_VECTOR_DATA_T * val, int recursive,
                          void *data, size_t idx)
{

    LNXPROC_DEBUG("Val %p Rec %d Data %p Idx %zd\n", val, recursive, data, idx);

    struct array_iterate_t *adata = data;
    size_t *aidx = adata->idx;
    size_t adim = adata->dim;

#ifdef DEBUG
    LNXPROC_DEBUG("Adata: Data %p Depth %d\n", adata->data, adata->depth);
    DEBUG_IDX(aidx, adim);
#endif

    LNXPROC_ERROR_T ret = LNXPROC_OK;

    aidx[adata->depth] = idx;
    if (recursive) {
        struct array_iterate_t adata1;

        memcpy(&adata1, adata, sizeof(adata1));
        adata1.depth++;
        ret = lnxproc_vector_iterate(val->child, &adata1, -1, -1,
                                     array_vector_iterate_func);

    }
    else {
        ret = adata->func(val->value, adata->data, aidx, adim);
    }

    return ret;
}

LNXPROC_ERROR_T
lnxproc_array_iterate(LNXPROC_ARRAY_T *array,
                      void *data, LNXPROC_ARRAY_ITERATE_FUNC func)
{
    LNXPROC_DEBUG("Array %p Data %p Func %p\n", array, data, func);

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (!array->vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_VECTOR_NULL;
    }

    if (array->dim > 0) {
/* Allocating on the stack confuses valgrind which reports a use of an 
 * uninitialised variable. Use the calloc version when testing with valgrind
 */
#ifdef DEBUG
        size_t *idx = calloc(array->dim, sizeof(size_t));
#else
        size_t idx[array->dim];

        idx[0] = 0;
#endif
        struct array_iterate_t adata = {
            .data = data,
            .idx = idx,
            .dim = array->dim,
            .depth = 0,
            .func = func,
        };

        LNXPROC_ERROR_T ret =
            lnxproc_vector_iterate(array->vector, &adata, -1, -1,
                                   array_vector_iterate_func);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
#ifdef DEBUG
            free(idx);
#endif
            return ret;
        }
#ifdef DEBUG
        free(idx);
#endif
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
array_print_internal(char *val, void *data, size_t idx[], size_t dim)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Val %p '%s' Data %p\n", val, val, data);
    DEBUG_IDX(idx, dim);
#endif
    int i;

    for (i = 0; i < dim; i++) {
        printf("%zd ", idx[i]);
    }
    printf(":%1$p '%1$s'\n", val);

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_array_print(LNXPROC_ARRAY_T *array, int allocated, void *data)
{

    LNXPROC_DEBUG("Array %p Alloc %d Data %p\n", array, allocated, data);

    printf("Array at %p\n", array);

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    LNXPROC_ERROR_T ret;

    printf("Array limits at  %p\n", array->limits);
    printf("Array dim %zd\n", array->dim);
    lnxproc_limits_print(array->limits, array->dim);
    if (array->dim > 0) {
        ret = lnxproc_array_iterate(array, data, array_print_internal);
        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
    }
    else {
        size_t idx = 0;
        char *val = NULL;

        ret = lnxproc_vector_value(array->vector, idx, &val);
        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }

        ret = array_print_internal(val, data, &idx, 1);
        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
    }

    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
