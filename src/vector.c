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

static void *
vector_malloc(size_t size)
{
    size_t n = size * sizeof(void *);
    void *p = malloc(n);

    if (p) {
        memset(p, 0, n);
    }

    return p;
}

static void *
vector_realloc(void *old, size_t osize, size_t size)
{
    size_t nsize = osize + size;
    size_t n = nsize * sizeof(void *);
    void *p = realloc(old, n);

    if (p) {
        void *prev = p + (osize * sizeof(void *));

        n = size * sizeof(void *);
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
        LNXPROC_DEBUG("Malloc %zd words of data\n", size);
        vector->data = vector_malloc(size);

        if (!vector->data) {
            free(vector);
            vector = NULL;
            LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_VECTOR_MALLOC_DATA);
            LNXPROC_ERROR_DEBUG(-errno, "Malloc data\n");
            return vector;
        }

    }

    else {
        vector->data = NULL;
    }

    LNXPROC_DEBUG("Success\n");
    return vector;
}

LNXPROC_VECTOR_T *
lnxproc_vector_free(LNXPROC_VECTOR_T * vector)
{
    LNXPROC_DEBUG("Array %p\n", vector);

    if (vector) {
        if (vector->data) {
            if (vector->recursive) {
                int i;

                for (i = 0; i < vector->size; i++) {
                    void *a;

                    memcpy(&a, vector->data + (i * sizeof(void *)),
                           sizeof(void *));

                    if (a) {
                        LNXPROC_DEBUG("Free sub vector %d %p\n", i, a);
                        a = lnxproc_vector_free(a);
                    }
                }
            }

            LNXPROC_DEBUG("Free data %p\n", vector->data);
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

int
lnxproc_vector_resize(LNXPROC_VECTOR_T * vector, size_t size)
{
    LNXPROC_DEBUG("Array %p Size %zd\n", vector, size);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    size_t osize = vector->size;
    void *p = vector_realloc(vector->data, osize, size);

    if (!p) {
        LNXPROC_SET_ERROR(vector->callback, LNXPROC_ERROR_VECTOR_REALLOC_DATA);
        LNXPROC_ERROR_DEBUG(-errno, "Realloc data\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
    }

    vector->data = p;
    vector->size += size;
    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

void *
lnxproc_vector_addr(LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Array %p Idx %zd\n", vector, idx);

    void *val = NULL;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return val;
    }

    if (idx >= vector->size) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= %zd\n", idx, vector->size);
        return val;
    }

    val = vector->data + (idx * sizeof(void *));
    LNXPROC_DEBUG("Success (val = %p)\n", val);
    return val;
}

void *
lnxproc_vector_get(LNXPROC_VECTOR_T * vector, size_t idx)
{
    LNXPROC_DEBUG("Array %p Idx %zd\n", vector, idx);

    void *val = NULL;

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return val;
    }

    if (idx >= vector->size) {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                            "Idx %zd >= %zd\n", idx, vector->size);
        return val;
    }

    memcpy(&val, vector->data + (idx * sizeof(void *)), sizeof(void *));
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
lnxproc_vector_set(LNXPROC_VECTOR_T * vector, size_t idx, void *val)
{
    LNXPROC_DEBUG("Array %p Idx %zd Val %p\n", vector, idx, val);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    if (idx < vector->length) {
        LNXPROC_DEBUG("Set normal entry (not an append) Idx %zd Length %zd\n",
                      idx, vector->length);
        memcpy(vector->data + (idx * sizeof(void *)), &val, sizeof(void *));

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
        memcpy(vector->data + (idx * sizeof(void *)), &val, sizeof(void *));
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);

    }

    else {
        LNXPROC_SET_ERROR(vector->callback,
                          LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

int
lnxproc_vector_set_last(LNXPROC_VECTOR_T * vector, size_t idx, void *val)
{
    LNXPROC_DEBUG("Array %p Idx %zd Val %p\n", vector, idx, val);

    int ret = lnxproc_vector_set(vector, idx, val);

    if (ret == LNXPROC_OK) {
        vector->length = idx + 1;
        LNXPROC_DEBUG("Array length is now %zd\n", vector->length);
    }

    return ret;
}

int
lnxproc_vector_append(LNXPROC_VECTOR_T * vector, void *val)
{
    LNXPROC_DEBUG("Array %p Val %p\n", vector, val);

    if (!vector) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    return lnxproc_vector_set(vector, vector->length, val);
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

    void *a = lnxproc_vector_addr(vector, idx);
    void *p = lnxproc_vector_get(vector, idx);
    int depth = vector_print_depth(data);
    int allocated = vector_print_allocated(data);

    vector_print_indent(depth);

    if (vector->recursive) {
        struct vector_print_var_t printvar = {
            .depth = depth + 1,
            .allocated = allocated,
        };
        printf("--> %d:Addr %p Value %p\n", idx, a, p);

        if (p) {
            lnxproc_vector_print(p, allocated, &printvar);
        }

    }

    else {
        printf("--> %d:Addr %p Value %p '%s'\n", idx, a, p, (char *) p);
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
    printf("Array data at %p\n", vector->data);
    if (allocated) {
        lnxproc_vector_iterate(vector, data, -1, vector->size,
                               vector_print_internal);
    }
    else {
        lnxproc_vector_iterate(vector, data, -1, -1, vector_print_internal);
    }
    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
