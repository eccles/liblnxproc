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

static LNXPROC_VECTOR_T **
vector_malloc_children(size_t size)
{
    size_t n = size * sizeof(LNXPROC_VECTOR_T *);
    LNXPROC_VECTOR_T **p = calloc(1, n);

    return p;
}

static char **
vector_malloc_values(size_t size)
{
    size_t n = size * sizeof(char *);
    char **p = calloc(1, n);

    return p;
}

static LNXPROC_VECTOR_T **
vector_realloc_children(LNXPROC_VECTOR_T ** old, size_t oldsize, size_t addsize)
{
    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(LNXPROC_VECTOR_T *);
    LNXPROC_VECTOR_T **p = realloc(old, n);

    if (p) {
        LNXPROC_VECTOR_T **prev = p + oldsize;

        n = addsize * sizeof(LNXPROC_VECTOR_T *);
        memset(prev, 0, n);
    }

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

LNXPROC_VECTOR_T *
lnxproc_vector_new(size_t size, int recursive, LNXPROC_ERROR_CALLBACK callback)
{

    LNXPROC_DEBUG("Size %zd Recursive %d Callback %p\n", size, recursive,
                  callback);

    LNXPROC_VECTOR_T *vector = NULL;
    void *p = malloc(sizeof(LNXPROC_VECTOR_T));

    if (!p) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_VECTOR_MALLOC_HEADER);
        LNXPROC_ERROR_DEBUG(-errno, "Malloc vector\n");
        return p;
    }

    vector = p;
    vector->length = 0;
    vector->size = size;
    vector->recursive = recursive;
    vector->callback = callback;

    /* We have to malloc this separately as other objects may hold
     * references to 'vector' and we do not want 'vector' to change on a
     * realloc
     */
    if (size > 0) {

        if (recursive) {
            LNXPROC_DEBUG("Malloc %zd words of children\n", size);
            vector->children = vector_malloc_children(size);

            if (!vector->children) {
                LNXPROC_SET_ERROR(callback,
                                  LNXPROC_ERROR_VECTOR_MALLOC_CHILDREN);
                LNXPROC_ERROR_DEBUG(-errno, "Malloc children\n");
                return lnxproc_vector_free(vector);
            }
            vector->values = NULL;
        }
        else {
            LNXPROC_DEBUG("Malloc %zd words of children\n", size);
            vector->values = vector_malloc_values(size);

            if (!vector->values) {
                LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_VECTOR_MALLOC_VALUES);
                LNXPROC_ERROR_DEBUG(-errno, "Malloc values\n");
                return lnxproc_vector_free(vector);
            }
            vector->children = NULL;
        }

    }

    else {
        vector->children = NULL;
        vector->values = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return vector;
}

LNXPROC_VECTOR_T *
lnxproc_vector_free(LNXPROC_VECTOR_T * vector)
{
    LNXPROC_DEBUG("Array %p\n", vector);

    if (vector) {
        if (vector->children) {
            if (vector->recursive) {
                int i;

                for (i = 0; i < vector->size; i++) {
                    LNXPROC_VECTOR_T *v = vector->children[i];

                    v = lnxproc_vector_free(v);
                }
            }

            LNXPROC_DEBUG("Free children %p\n", vector->children);
            free(vector->children);
            vector->children = NULL;
        }
        if (vector->values) {
            free(vector->values);
            vector->values = NULL;
        }

        LNXPROC_DEBUG("Free vector %p\n", vector);
        free(vector);
        vector = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return vector;
}

int
lnxproc_vector_resize(LNXPROC_VECTOR_T * vector, size_t size)
{
    LNXPROC_DEBUG("Array %p Size %zd\n", vector, size);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    size_t osize = vector->size;
    char **p = vector_realloc_values(vector->values, osize, size);

    if (!p) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_REALLOC_VALUES);
        LNXPROC_ERROR_DEBUG(-errno, "Realloc values\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_VALUES;
    }
    vector->values = p;

    if (vector->recursive) {
        LNXPROC_VECTOR_T **v =
            vector_realloc_children(vector->children, osize, size);
        if (!v) {
            LNXPROC_SET_ERROR(vector->callback,
                              LNXPROC_ERROR_VECTOR_REALLOC_CHILDREN);
            LNXPROC_ERROR_DEBUG(-errno, "Realloc children\n");
            return LNXPROC_ERROR_VECTOR_REALLOC_CHILDREN;
        }
        vector->children = v;
    }

    vector->size += size;
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_VECTOR_T *
lnxproc_vector_child(LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Array %p Idx %zd\n", vector, idx);

    LNXPROC_VECTOR_T *val = NULL;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return val;
    }

    if (!vector->children) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return val;
    }

    if (idx >= vector->size) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= %zd\n", idx, vector->size);
        return val;
    }

    val = vector->children[idx];
    LNXPROC_DEBUG("Success (val = %p)\n", val);
    return val;
}

char *
lnxproc_vector_value(LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Array %p Idx %zd\n", vector, idx);

    char *val = NULL;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return val;
    }

    if (!vector->values) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return val;
    }

    if (idx >= vector->size) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= %zd\n", idx, vector->size);
        return val;
    }

    val = vector->values[idx];
    LNXPROC_DEBUG("Success (val = %p)\n", val);
    return val;
}

size_t
lnxproc_vector_size(LNXPROC_VECTOR_T * vector)
{
    LNXPROC_DEBUG("Array %p\n", vector);

    size_t size = -1;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return size;
    }

    size = vector->size;
    LNXPROC_DEBUG("Success (size = %zd)\n", size);
    return size;
}

LNXPROC_ERROR_CALLBACK
lnxproc_vector_callback(LNXPROC_VECTOR_T * vector)
{
    LNXPROC_DEBUG("Array %p\n", vector);

    LNXPROC_ERROR_CALLBACK callback = NULL;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return callback;
    }

    callback = vector->callback;
    LNXPROC_DEBUG("Success (callback = %p)\n", callback);
    return callback;
}

int
lnxproc_vector_set_child(LNXPROC_VECTOR_T * vector, size_t idx,
                         LNXPROC_VECTOR_T * child)
{
    LNXPROC_DEBUG("Array %p Idx %zd Child %p\n", vector, idx, child);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->children) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_CHILDREN, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_CHILDREN;
    }

    if (idx < vector->length) {
        LNXPROC_DEBUG("Set normal entry (not an append) Idx %zd Length %zd\n",
                      idx, vector->length);
        vector->children[idx] = child;
//        vector->values[idx] = (char *)child->values;
    }

    else if (idx == vector->length) {
        LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx, vector->length);

        if (idx == vector->size) {
            LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            int err = lnxproc_vector_resize(vector, 1);

            if (err) {
                LNXPROC_SET_ERROR(vector->callback, err);
                LNXPROC_ERROR_DEBUG(err, "\n");
                return err;
            }
        }

        LNXPROC_DEBUG("Set entry %zd to %p\n", idx, child);
        vector->children[idx] = child;
//        vector->values[idx] = (char *)child->values;
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);

    }

    else {
        LNXPROC_DEBUG("Illegal index %zd Length %zd\n", idx, vector->length);
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
lnxproc_vector_set_value(LNXPROC_VECTOR_T * vector, size_t idx, char *val)
{
    LNXPROC_DEBUG("Array %p Idx %zd Val %p '%s'\n", vector, idx, val, val);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (!vector->values) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL_VALUES, "\n");
        return LNXPROC_ERROR_VECTOR_NULL_VALUES;
    }

    if (idx < vector->length) {
        LNXPROC_DEBUG("Set normal entry (not an append) Idx %zd Length %zd\n",
                      idx, vector->length);
        vector->values[idx] = val;
    }

    else if (idx == vector->length) {
        LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx, vector->length);

        if (idx == vector->size) {
            LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            int err = lnxproc_vector_resize(vector, 1);

            if (err) {
                LNXPROC_SET_ERROR(vector->callback, err);
                LNXPROC_ERROR_DEBUG(err, "\n");
                return err;
            }
        }

        LNXPROC_DEBUG("Set entry %zd to %p\n", idx, val);
        vector->values[idx] = val;
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);

    }

    else {
        LNXPROC_DEBUG("Illegal index %zd Length %zd\n", idx, vector->length);
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
lnxproc_vector_set_last_child(LNXPROC_VECTOR_T * vector, size_t idx,
                              LNXPROC_VECTOR_T * val)
{
    LNXPROC_DEBUG("Array %p Idx %zd Val %p\n", vector, idx, val);

    int ret = lnxproc_vector_set_child(vector, idx, val);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);
    }

    return ret;
}

int
lnxproc_vector_set_last_value(LNXPROC_VECTOR_T * vector, size_t idx, char *val)
{
    LNXPROC_DEBUG("Array %p Idx %zd Val %p\n", vector, idx, val);

    int ret = lnxproc_vector_set_value(vector, idx, val);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);
    }

    return ret;
}

int
lnxproc_vector_append(LNXPROC_VECTOR_T * vector, LNXPROC_VECTOR_T * child,
                      char *val)
{
    LNXPROC_DEBUG("Array %p Child %p Val %p\n", vector, child, val);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    int ret = lnxproc_vector_set_child(vector, vector->length, child);

    if (ret == LNXPROC_OK) {
        ret = lnxproc_vector_set_value(vector, vector->length, val);
    }
    return ret;
}

int
lnxproc_vector_set_length(LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Array %p Idx %zd\n", vector, idx);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (idx >= vector->size) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    vector->length = idx + 1;
    LNXPROC_DEBUG("Array length is now %zd\n", vector->length);
    return LNXPROC_OK;
}

int
lnxproc_vector_iterate(LNXPROC_VECTOR_T * vector,
                       void *data,
                       int start, int end, LNXPROC_VECTOR_ITERATE_FUNC func)
{
    LNXPROC_DEBUG("Array %p Data %p Start %d End %d Func %p\n", vector, data,
                  start, end, func);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
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
        func(vector, data, i);
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

struct vector_print_var_t {
    int depth;
    int allocated;
};

static int
vector_print_depth(struct vector_print_var_t *printvar)
{
    int depth = 0;

    if (printvar) {
        depth = printvar->depth;
    }

    return depth;
}

static int
vector_print_allocated(struct vector_print_var_t *printvar)
{
    int allocated = 0;

    if (printvar) {
        allocated = printvar->allocated;
    }

    return allocated;
}

static void
vector_print_indent(int depth)
{
    int i;

    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static int
vector_print_internal(LNXPROC_VECTOR_T * vector, void *data, int idx)
{
    LNXPROC_DEBUG("Array %p Data %p Idx %d\n", vector, data, idx);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    LNXPROC_VECTOR_T *child = lnxproc_vector_child(vector, idx);
    char *p = lnxproc_vector_value(vector, idx);
    int depth = vector_print_depth(data);
    int allocated = vector_print_allocated(data);

    vector_print_indent(depth);

    if (vector->recursive && vector->children) {
        struct vector_print_var_t printvar = {
            .depth = depth + 1,
            .allocated = allocated,
        };
        printf("--> %d:Child %p Value %p\n", idx, child, p);

        if (child) {
            lnxproc_vector_print(child, allocated, &printvar);
        }

    }

    else {
        printf("--> %d:Child %p Value %p '%s'\n", idx, child, p, p);
    }

    return LNXPROC_OK;
}

int
lnxproc_vector_print(LNXPROC_VECTOR_T * vector, int allocated, void *data)
{
    LNXPROC_DEBUG("Array %p Data %p\n", vector, data);

    int depth = vector_print_depth(data);

    vector_print_indent(depth);
    printf("Array at %p\n", vector);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    vector_print_indent(depth);
    printf("Array size %zd\n", vector->size);
    vector_print_indent(depth);
    printf("Array length %zd\n", vector->length);
    vector_print_indent(depth);
    printf("Array recursive %d\n", vector->recursive);
    vector_print_indent(depth);
    printf("Array children at %p\n", vector->children);
    vector_print_indent(depth);
    printf("Array values at %p\n", vector->values);

    struct vector_print_var_t printvar = {
        .depth = depth + 1,
        .allocated = allocated,
    };

    if (allocated) {
        lnxproc_vector_iterate(vector, &printvar, -1, vector->size,
                               vector_print_internal);
    }
    else {
        lnxproc_vector_iterate(vector, &printvar, -1, -1,
                               vector_print_internal);
    }
    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
