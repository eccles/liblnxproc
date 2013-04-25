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

#include <lnxproc/error.h>
#include "vector_private.h"

static _LNXPROC_VECTOR_DATA_T *
vector_malloc_data(size_t size)
{
    size_t n = size * sizeof(_LNXPROC_VECTOR_DATA_T);
    _LNXPROC_VECTOR_DATA_T *p = calloc(1, n);

    return p;
}

static _LNXPROC_VECTOR_DATA_T *
vector_realloc_data(_LNXPROC_VECTOR_DATA_T * old, size_t oldsize,
                    size_t addsize)
{
    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(_LNXPROC_VECTOR_DATA_T);
    _LNXPROC_VECTOR_DATA_T *p = realloc(old, n);

    if (p) {
        _LNXPROC_VECTOR_DATA_T *prev = p + oldsize;

        n = addsize * sizeof(_LNXPROC_VECTOR_DATA_T);
        memset(prev, 0, n);
    }

    return p;
}

LNXPROC_ERROR_T
_lnxproc_data_child(_LNXPROC_VECTOR_DATA_T * val, _LNXPROC_VECTOR_T ** vector)
{
    if (!val) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_DATA, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_DATA;
    }
    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }

    *vector = val->child;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_data_value(_LNXPROC_VECTOR_DATA_T * val, char **value)
{
    if (!val) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_DATA, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_DATA;
    }
    if (!value) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*value) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }

    *value = val->value;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_new(_LNXPROC_VECTOR_T ** vector, size_t size, int recursive)
{

    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof _LNXPROC_VECTOR_T %d\n", sizeof(_LNXPROC_VECTOR_T));
    LNXPROC_DEBUG("Size %zd Recursive %d\n", size, recursive);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }

    if (*vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }

    _LNXPROC_VECTOR_T *p = malloc(sizeof(_LNXPROC_VECTOR_T));

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_HEADER,
                            "Malloc vector\n");
        return LNXPROC_ERROR_VECTOR_MALLOC_HEADER;
    }

    p->length = 0;
    p->size = size;
    p->recursive = recursive;

    /* We have to malloc this separately as other objects may hold
     * references to 'vector' and we do not want 'vector' to change on a
     * realloc
     */
    if (size > 0) {
        LNXPROC_DEBUG("Malloc %zd words of child\n", size);

        p->data = vector_malloc_data(size);

        if (!p->data) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_DATA,
                                "Malloc data\n");
            _LNXPROC_VECTOR_FREE(p);
            return LNXPROC_ERROR_VECTOR_MALLOC_DATA;
        }
    }

    else {
        p->data = NULL;
    }

    *vector = p;
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

_LNXPROC_VECTOR_T *
_lnxproc_vector_free(_LNXPROC_VECTOR_T * vector)
{
    LNXPROC_DEBUG("Vector %p\n", vector);

    if (vector) {
        if (vector->data) {
            if (vector->recursive) {
                int i;

                for (i = 0; i < vector->size; i++) {
                    _LNXPROC_VECTOR_FREE(vector->data[i].child);
                }
            }
            free(vector->data);
            vector->data = NULL;
        }
        LNXPROC_DEBUG("Free vector %p\n", vector);
        free(vector);
        vector = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return vector;
}

LNXPROC_ERROR_T
_lnxproc_vector_resize(_LNXPROC_VECTOR_T * vector, size_t size)
{
    LNXPROC_DEBUG("Vector %p Size %zd\n", vector, size);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    size_t osize = vector->size;
    _LNXPROC_VECTOR_DATA_T *p = vector_realloc_data(vector->data, osize, size);

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_REALLOC_DATA,
                            "Realloc values\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
    }
    vector->data = p;
    vector->size += size;
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                      _LNXPROC_VECTOR_T ** child)
{
    LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }
    if (!child) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*child) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (!vector->recursive || !vector->data) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_CHILDREN;
    }
    if (idx >= vector->size) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    *child = vector->data[idx].child;
    LNXPROC_DEBUG("Success (child = %p)\n", *child);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_value(_LNXPROC_VECTOR_T * vector, size_t idx, char **value)
{
    LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }
    if (!value) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*value) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (vector->recursive || !vector->data) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_VALUES;
    }

    if (idx >= vector->size) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    *value = vector->data[idx].value;
    LNXPROC_DEBUG("Success (val = %p '%s')\n", *value, *value);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_size(_LNXPROC_VECTOR_T * vector, size_t * size)
{
    LNXPROC_DEBUG("Vector %p\n", vector);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }
    if (!size) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }

    *size = vector->size;
    LNXPROC_DEBUG("Success (size = %zd)\n", *size);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                          _LNXPROC_VECTOR_T * child)
{
    LNXPROC_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->recursive || !vector->data) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_CHILDREN;
    }

    if (idx < vector->length) {
        LNXPROC_DEBUG
            ("Set normal entry (not an append) Idx %zd Length %zd\n", idx,
             vector->length);
        vector->data[idx].child = child;
    }

    else if (idx == vector->length) {
        LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx, vector->length);

        if (idx == vector->size) {
            LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            LNXPROC_ERROR_T ret = _lnxproc_vector_resize(vector, 1);

            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }

        LNXPROC_DEBUG("Set entry %zd to %p\n", idx, child);
        vector->data[idx].child = child;
        vector->length = idx + 1;
        LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);

    }

    else {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_value(_LNXPROC_VECTOR_T * vector, size_t idx, char *val)
{
    LNXPROC_DEBUG("Vector %p Idx %zd Val %p '%s'\n", vector, idx, val, val);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (vector->recursive || !vector->data) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_VALUES;
    }

    if (idx < vector->length) {
        LNXPROC_DEBUG
            ("Set normal entry (not an append) Idx %zd Length %zd\n", idx,
             vector->length);
        vector->data[idx].value = val;
    }

    else if (idx == vector->length) {
        LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx, vector->length);

        if (idx == vector->size) {
            LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            LNXPROC_ERROR_T ret = _lnxproc_vector_resize(vector, 1);

            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }

        LNXPROC_DEBUG("Set entry %zd to %p\n", idx, val);
        vector->data[idx].value = val;
        vector->length = idx + 1;
        LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);

    }

    else {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_last_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                               _LNXPROC_VECTOR_T * child)
{
    LNXPROC_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    LNXPROC_ERROR_T ret = _lnxproc_vector_set_child(vector, idx, child);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_last_value(_LNXPROC_VECTOR_T * vector, size_t idx,
                               char *val)
{
    LNXPROC_DEBUG("Vector %p Idx %zd Val %p\n", vector, idx, val);

    LNXPROC_ERROR_T ret = _lnxproc_vector_set_value(vector, idx, val);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_length(_LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (idx < 0 || idx >= vector->size) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Index %zd Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    vector->length = idx + 1;
    LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_iterate(_LNXPROC_VECTOR_T * vector,
                        void *data,
                        int start, int end, _LNXPROC_VECTOR_ITERATE_FUNC func)
{
    LNXPROC_DEBUG("Vector %p Data %p Start %d End %d Func %p\n", vector,
                  data, start, end, func);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->data) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_DATA, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_DATA;
    }

    if (start < 0) {
        start = 0;
    }
    else if (start > vector->length) {
        start = vector->length;
    }

    if (end < 0) {
        end = vector->length;
    }
    else if (end > vector->size) {
        end = vector->size;
    }

    LNXPROC_DEBUG("Iterate from %d to %d\n", start, end);
    int i;

    for (i = start; i < end; i++) {
        func(vector->data + i, vector->recursive, data, i);
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

struct printvar_t {
    int depth;
    int allocated;
};

static void
vector_print_depth(int depth)
{
    int i;

    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static LNXPROC_ERROR_T
vector_print_internal(_LNXPROC_VECTOR_DATA_T * val, int recursive,
                      void *data, size_t idx)
{
    LNXPROC_DEBUG("Val %p Rec %d Data %p Idx %zd\n", val, recursive, data, idx);

    if (val) {
        int depth = 0;
        int allocated = 0;
        struct printvar_t *printvar = NULL;

        if (data) {
            printvar = data;
            depth = printvar->depth;
            allocated = printvar->allocated;
        }

        vector_print_depth(depth);
        if (recursive) {
            printf("--> %zd:Child %p\n", idx, val->child);

            _lnxproc_vector_print(val->child, allocated, data);
        }
        else {
            printf("--> %zd:Value %p '%s'\n", idx, val->value, val->value);
        }
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_print(_LNXPROC_VECTOR_T * vector, int allocated, void *data)
{
    LNXPROC_DEBUG("Vector %p Data %p\n", vector, data);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    struct printvar_t printvar;

    if (data) {
        struct printvar_t *oldprintvar = data;

        printvar.depth = oldprintvar->depth + 1;
        printvar.allocated = oldprintvar->allocated;
    }
    else {
        printvar.depth = 0;
        printvar.allocated = allocated;
    }

    vector_print_depth(printvar.depth);
    printf("Vector at %p\n", vector);
    vector_print_depth(printvar.depth);
    printf("Vector size %zd\n", vector->size);
    vector_print_depth(printvar.depth);
    printf("Vector length %zd\n", vector->length);
    vector_print_depth(printvar.depth);
    printf("Vector recursive %d\n", vector->recursive);
    vector_print_depth(printvar.depth);
    if (vector->recursive) {
        printf("Vector children at %p\n", vector->data);
    }
    else {
        printf("Vector values at %p\n", vector->data);
    }
    if (allocated) {
        _lnxproc_vector_iterate(vector, &printvar, -1, vector->size,
                                vector_print_internal);
    }
    else {
        _lnxproc_vector_iterate(vector, &printvar, -1, -1,
                                vector_print_internal);
    }
    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
