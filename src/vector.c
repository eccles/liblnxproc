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

static _LNXPROC_VECTOR_T **
vector_malloc_children(size_t size)
{
    size_t n = size * sizeof(_LNXPROC_VECTOR_T *);
    _LNXPROC_VECTOR_T **p = calloc(1, n);

    return p;
}

static _LNXPROC_VECTOR_T **
vector_realloc_children(_LNXPROC_VECTOR_T ** old, size_t oldsize,
                        size_t addsize)
{
    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(_LNXPROC_VECTOR_T *);
    _LNXPROC_VECTOR_T **p = realloc(old, n);

    if (p) {
        _LNXPROC_VECTOR_T **prev = p + oldsize;

        n = addsize * sizeof(_LNXPROC_VECTOR_T *);
        memset(prev, 0, n);
    }

    return p;
}

static char **
vector_malloc_values(size_t size)
{
    size_t n = size * sizeof(char *);
    char **p = calloc(1, n);

    return p;
}

static char **
vector_realloc_values(char **old, size_t oldsize, size_t addsize)
{
    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(char *);
    char **p = realloc(old, n);

    if (p) {
        char **prev = p + oldsize;

        n = addsize * sizeof(char *);
        memset(prev, 0, n);
    }

    return p;
}

LNXPROC_ERROR_T
_lnxproc_vector_new(_LNXPROC_VECTOR_T ** vector, size_t size, int recursive)
{

    _LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_VECTOR_T %d\n", sizeof(_LNXPROC_VECTOR_T));
    _LNXPROC_DEBUG("Size %zd Recursive %d\n", size, recursive);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }

    if (*vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }

    _LNXPROC_VECTOR_T *p = calloc(1, sizeof(_LNXPROC_VECTOR_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_HEADER,
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
        _LNXPROC_DEBUG("Malloc %zd words of child\n", size);

        p->children = vector_malloc_children(size);

        if (!p->children) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_DATA,
                                 "Malloc data\n");
            _LNXPROC_VECTOR_FREE(p);
            return LNXPROC_ERROR_VECTOR_MALLOC_DATA;
        }

        p->values = vector_malloc_values(size);

        if (!p->values) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_DATA,
                                 "Malloc data\n");
            _LNXPROC_VECTOR_FREE(p);
            return LNXPROC_ERROR_VECTOR_MALLOC_DATA;
        }
    }

    else {
        p->children = NULL;
        p->values = NULL;
    }

    *vector = p;
    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

_LNXPROC_VECTOR_T *
_lnxproc_vector_free(_LNXPROC_VECTOR_T * vector)
{
    _LNXPROC_DEBUG("Vector %p\n", vector);

    if (vector) {
        if (vector->children) {
            int i;

            for (i = 0; i < vector->size; i++) {
                _LNXPROC_VECTOR_FREE(vector->children[i]);
            }
            free(vector->children);
            vector->children = NULL;
        }
        if (vector->values) {
            free(vector->values);
            vector->values = NULL;
        }
        _LNXPROC_DEBUG("Free vector %p\n", vector);
        free(vector);
        vector = NULL;
    }

    _LNXPROC_DEBUG("Success\n");
    return vector;
}

LNXPROC_ERROR_T
_lnxproc_vector_resize(_LNXPROC_VECTOR_T * vector, size_t size)
{
    _LNXPROC_DEBUG("Vector %p Size %zd\n", vector, size);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    size_t osize = vector->size;
    _LNXPROC_VECTOR_T **p =
        vector_realloc_children(vector->children, osize, size);

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_REALLOC_DATA,
                             "Realloc values\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
    }
    vector->children = p;

    char **q = vector_realloc_values(vector->values, osize, size);

    if (!q) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_REALLOC_DATA,
                             "Realloc values\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
    }
    vector->values = q;
    vector->size += size;
    _LNXPROC_DEBUG("Success\n");

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                      _LNXPROC_VECTOR_T ** child)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }
    if (!child) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*child) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (!vector->recursive || !vector->children) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_CHILDREN;
    }
    if (idx >= vector->size) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Idx %zd >= Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    *child = vector->children[idx];
    _LNXPROC_DEBUG("Success (child = %p)\n", *child);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_value(_LNXPROC_VECTOR_T * vector, size_t idx, char **value)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }
    if (!value) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_NULL;
    }
    if (*value) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_VECTOR_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (vector->recursive || !vector->values) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_VALUES;
    }

    if (idx >= vector->size) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Idx %zd >= Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    *value = vector->values[idx];
    _LNXPROC_DEBUG("Success (val = %p '%s')\n", *value, *value);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                          _LNXPROC_VECTOR_T * child)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->recursive || !vector->children || !vector->values) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_CHILDREN;
    }

    if (idx < vector->length) {
        _LNXPROC_DEBUG("Set entry %zd to %p\n", idx, child);
        vector->children[idx] = child;
        _LNXPROC_DEBUG("Set value %zd to %p\n", idx, child->values);
        vector->values[idx] = (char *) child->values;
    }

    else if (idx == vector->length) {
        _LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx,
                       vector->length);

        if (idx == vector->size) {
            _LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            LNXPROC_ERROR_T ret = _lnxproc_vector_resize(vector, 1);

            if (ret) {
                _LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }

        _LNXPROC_DEBUG("Append entry %zd to %p\n", idx, child);
        vector->children[idx] = child;
        _LNXPROC_DEBUG("Append value %zd to %p\n", idx, child->values);
        vector->values[idx] = (char *) child->values;
        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);

    }

    else {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_value(_LNXPROC_VECTOR_T * vector, size_t idx, char *val)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd Val %p '%s'\n", vector, idx, val, val);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (vector->recursive || !vector->values) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_VALUES;
    }

    if (idx < vector->length) {
        _LNXPROC_DEBUG
            ("Set normal entry (not an append) Idx %zd Length %zd\n", idx,
             vector->length);
        vector->values[idx] = val;
    }

    else if (idx == vector->length) {
        _LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx,
                       vector->length);

        if (idx == vector->size) {
            _LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            LNXPROC_ERROR_T ret = _lnxproc_vector_resize(vector, 1);

            if (ret) {
                _LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }

        _LNXPROC_DEBUG("Set entry %zd to %p\n", idx, val);
        vector->values[idx] = val;
        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);

    }

    else {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_last_child(_LNXPROC_VECTOR_T * vector, size_t idx,
                               _LNXPROC_VECTOR_T * child)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    LNXPROC_ERROR_T ret = _lnxproc_vector_set_child(vector, idx, child);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_last_value(_LNXPROC_VECTOR_T * vector, size_t idx,
                               char *val)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd Val %p\n", vector, idx, val);

    LNXPROC_ERROR_T ret = _lnxproc_vector_set_value(vector, idx, val);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

LNXPROC_ERROR_T
_lnxproc_vector_set_length(_LNXPROC_VECTOR_T * vector, size_t idx)
{
    _LNXPROC_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (idx < 0 || idx >= vector->size) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Index %zd Size %zd\n", idx, vector->size);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    vector->length = idx + 1;
    _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_iterate(_LNXPROC_VECTOR_T * vector,
                        void *data,
                        int start, int end, _LNXPROC_VECTOR_ITERATE_FUNC func)
{
    _LNXPROC_DEBUG("Vector %p Data %p Start %d End %d Func %p\n", vector,
                   data, start, end, func);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->children || !vector->values) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_DATA, "\n");
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

    _LNXPROC_DEBUG("Iterate from %d to %d\n", start, end);
    int i;

    for (i = start; i < end; i++) {
        func(vector->children[i], vector->values[i], vector->recursive, data,
             i);
    }

    _LNXPROC_DEBUG("Success\n");
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
vector_print_internal(_LNXPROC_VECTOR_T * child, char *value, int recursive,
                      void *data, size_t idx)
{
    _LNXPROC_DEBUG("Child %p Val %p Rec %d Data %p Idx %zd\n", child, value,
                   recursive, data, idx);

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
        printf("--> %zd:Value %p\n", idx, value);
        vector_print_depth(depth);
        printf("--> %zd:Child %p\n", idx, child);

        _lnxproc_vector_print(child, allocated, data);
    }
    else {
        printf("--> %1$zd:Value %2$p '%2$s'\n", idx, value);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_print(_LNXPROC_VECTOR_T * vector, int allocated, void *data)
{
    _LNXPROC_DEBUG("Vector %p Data %p\n", vector, data);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
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
    if (vector->children) {
        vector_print_depth(printvar.depth);
        printf("Vector children at %p\n", vector->children);
    }
    if (vector->values) {
        vector_print_depth(printvar.depth);
        printf("Vector values at %p\n", vector->values);
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
