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
array_create(LNXPROC_ERROR_CALLBACK callback,
             LNXPROC_LIMITS_T limits[], size_t dim, int depth)
{
    LNXPROC_DEBUG("callback %p\n", callback);
    LNXPROC_DEBUG("limits %p\n", limits);
    LNXPROC_DEBUG("dim %d\n", dim);
    LNXPROC_DEBUG("depth %d\n", depth);

    LNXPROC_LIMITS_T *limit = limits + depth;

    LNXPROC_DEBUG("limit %p\n", limit);
#ifdef DEBUG
    char buf[64];

    LNXPROC_DEBUG("limit %s\n", lnxproc_limit_print(limit, buf, sizeof buf));
#endif

    int recursive = depth < dim - 1 ? 1 : 0;

    LNXPROC_DEBUG("recursive %d\n", recursive);

    LNXPROC_VECTOR_T *wvec =
        lnxproc_vector_new(limit->expected, recursive, callback);

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < limit->expected; i++) {
            LNXPROC_VECTOR_T *f = array_create(callback,
                                               limits, dim, depth);

            if (!f) {
                return NULL;
            }
            lnxproc_vector_set_last_child(wvec, i, f);
        }
    }
    return wvec;
}

LNXPROC_ARRAY_T *
lnxproc_array_new(LNXPROC_LIMITS_T limits[], size_t dim,
                  LNXPROC_ERROR_CALLBACK callback)
{

    LNXPROC_DEBUG("Dim %d Callback %p\n", dim, callback);

    LNXPROC_ARRAY_T *array = NULL;

    void *p = calloc(1, sizeof(LNXPROC_ARRAY_T));

    if (!p) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_ARRAY_MALLOC_HEADER);
        LNXPROC_ERROR_DEBUG(-errno, "Malloc array\n");
        return p;
    }

    array = p;
    array->callback = callback;

    if (limits && (dim == 0)) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION);
        return lnxproc_array_free(array);
    }

    if (limits) {
        array->limits = lnxproc_limits_dup(callback, limits, dim);
        if (!array->limits) {
            return lnxproc_array_free(array);
        }
        array->dim = dim;
    }
    else {
        array->limits = NULL;
        array->dim = 0;
    }

    if (dim > 0) {
        array->saved = calloc(dim, sizeof(LNXPROC_VECTOR_T *));
        if (!array->saved) {
            return lnxproc_array_free(array);
        }

        array->vector = array_create(callback, limits, dim, 0);
        if (!array->vector) {
            return lnxproc_array_free(array);
        }
    }
    else {
        LNXPROC_DEBUG("Scalar array\n");
        array->vector = lnxproc_vector_new(0, 0, callback);
        if (!array->vector) {
            return lnxproc_array_free(array);
        }
        array->saved = NULL;
    }
    LNXPROC_DEBUG("Success\n");
    return array;
}

LNXPROC_ARRAY_T *
lnxproc_array_free(LNXPROC_ARRAY_T *array)
{
    LNXPROC_DEBUG("Array %p\n", array);

    if (array) {
        if (array->limits) {
            array->limits = lnxproc_limits_free(array->limits, array->dim);
        }
        if (array->saved) {
            free(array->saved);
            array->saved = NULL;
        }
        if (array->vector) {
            array->vector = lnxproc_vector_free(array->vector);
        }

        LNXPROC_DEBUG("Free array %p\n", array);
        free(array);
        array = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return array;
}

LNXPROC_ERROR_CALLBACK
lnxproc_array_callback(LNXPROC_ARRAY_T *array)
{
    LNXPROC_DEBUG("Array %p\n", array);

    LNXPROC_ERROR_CALLBACK callback = NULL;

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return callback;
    }

    callback = array->callback;
    LNXPROC_DEBUG("Success (callback = %p)\n", callback);
    return callback;
}

#ifdef DEBUG
#define DEBUG_IDX(idx,dim) {\
    char buf[32] = "";\
    int i;\
    int s = sizeof buf;\
    char *c = buf;\
    for( i=0; i < dim; i++ ) {\
        int n = snprintf(c, s,"%zd,",idx[i]);\
        c += n;\
        s -= n;\
    }\
    LNXPROC_DEBUG("Idx %s Dim %zd\n", buf, dim);\
}
#else
#define DEBUG_IDX(idx,dim)
#endif

int
lnxproc_array_set(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim, char *val)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (dim != array->dim) {
        LNXPROC_SET_ERROR(array->callback,
                          LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION);
        return LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION;
    }

    if (dim == 0) {
        lnxproc_vector_set_value(array->vector, 0, val);
        LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    array->saved[0] = array->vector;

    int i;
    int j = 0;

    for (i = 1; i < array->dim; i++) {
        j = i - 1;
        LNXPROC_VECTOR_T *f = lnxproc_vector_child(array->saved[j], idx[j]);

        if (!f) {
            int recursive = i < array->dim - 1 ? 1 : 0;

            f = lnxproc_vector_new(array->limits[i].expected, recursive,
                                   array->callback);
            if (!f) {
                return 1;
            }
            if (recursive) {
                lnxproc_vector_set_child(array->saved[j], idx[j], f);
            }
        }
        array->saved[i] = f;
    }
    lnxproc_vector_set_value(array->saved[dim - 1], idx[dim - 1], val);

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
lnxproc_array_set_last(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim,
                       char *val)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
    LNXPROC_DEBUG("Val %p\n", val);
#endif

    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (dim != array->dim) {
        LNXPROC_SET_ERROR(array->callback,
                          LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION);
        return LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION;
    }

    if (dim == 0) {
        lnxproc_vector_set_value(array->vector, 0, val);
        LNXPROC_DEBUG("Success\n");
        return LNXPROC_OK;
    }

    array->saved[0] = array->vector;
    lnxproc_vector_set_length(array->saved[0], idx[0]);

    int i;
    int j = 0;

    for (i = 1; i < array->dim; i++) {
        j = i - 1;
        LNXPROC_VECTOR_T *f = lnxproc_vector_child(array->saved[j], idx[j]);

        if (!f) {
            int recursive = i < array->dim - 1 ? 1 : 0;

            f = lnxproc_vector_new(array->limits[i].expected, recursive,
                                   array->callback);
            if (!f) {
                return 1;
            }
            if (recursive) {
                lnxproc_vector_set_last_child(array->saved[j], idx[j], f);
            }
        }
        lnxproc_vector_set_length(array->saved[j], idx[j]);
        array->saved[i] = f;
    }
    lnxproc_vector_set_last_value(array->saved[dim - 1], idx[dim - 1], val);

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

char *
lnxproc_array_get(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim)
{
#ifdef DEBUG
    LNXPROC_DEBUG("Array %p\n", array);
    DEBUG_IDX(idx, dim);
#endif

    if (!array) {
        return NULL;
    }

    if (dim != array->dim) {
        LNXPROC_SET_ERROR(array->callback,
                          LNXPROC_ERROR_ARRAY_ILLEGAL_DIMENSION);
        return NULL;
    }

    char *val = NULL;

    if (dim == 0) {
        val = lnxproc_vector_value(array->vector, 0);
        LNXPROC_DEBUG("Success\n");
        return val;
    }

    array->saved[0] = array->vector;

    int i;
    int j = 0;

    for (i = 1; i < array->dim; i++) {
        j = i - 1;
        LNXPROC_VECTOR_T *f = lnxproc_vector_child(array->saved[j], idx[j]);

        if (!f) {
            return NULL;
        }
        array->saved[i] = f;
    }
    val = lnxproc_vector_value(array->saved[dim - 1], idx[dim - 1]);

    LNXPROC_DEBUG("Success (val = %1$p '%1$s')\n", val);
    return val;
}

int
lnxproc_array_iterate(LNXPROC_ARRAY_T *array,
                      void *data, LNXPROC_VECTOR_ITERATE_FUNC func)
{
    LNXPROC_DEBUG("Array %p Data %p Func %p\n", array, data, func);

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (!array->vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    if (array->dim > 0) {
        int ret = lnxproc_vector_iterate(array->vector, NULL, -1, -1, func);

        if (ret) {
            return ret;
        }
    }
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
lnxproc_array_print(LNXPROC_ARRAY_T *array, int allocated, void *data)
{
    LNXPROC_DEBUG("Array %p Data %p\n", array, data);

    printf("Array at %p\n", array);

    if (!array) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_ARRAY_NULL;
    }

    printf("Array callback %p\n", array->callback);
    printf("Array limits at  %p\n", array->limits);
    printf("Array dim %zd\n", array->dim);
    lnxproc_limits_print(array->limits, array->dim);

    lnxproc_vector_print(array->vector, allocated, data);
    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
