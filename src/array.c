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
#include "array.h"

struct lnxproc_array_t {
    LNXPROC_ERROR_CALLBACK callback;
    size_t length;
    size_t size;
    int recursive;
    void *data;
};

static void *array_malloc(size_t size)
{
    size_t n = size * sizeof(void *);
    void *p = malloc(n);
    if (p) {
        memset(p, 0, n);
    }
    return p;
}

static void *array_realloc(void *old, size_t osize, size_t size)
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

LNXPROC_ARRAY_T *lnxproc_array_new(size_t size, int recursive,
                                   LNXPROC_ERROR_CALLBACK callback)
{

    LNXPROC_ARRAY_T *array = NULL;
    void *p = malloc(sizeof(LNXPROC_ARRAY_T));
    if (!p) {
        lnxproc_set_error(callback, __func__,
                          LNXPROC_ERROR_ARRAY_MALLOC_HEADER);
        return p;
    }
    array = p;
    array->length = 0;
    array->size = size;
    array->recursive = recursive;
    array->callback = callback;

    /* We have to malloc this separately as other objects may hold 
     * references to 'array' and we do not want 'array' to change on a 
     * realloc 
     */
    if (size > 0) {
        array->data = array_malloc(size);
        if (!array->data) {
            free(array);
            array = NULL;
            lnxproc_set_error(callback, __func__,
                              LNXPROC_ERROR_ARRAY_MALLOC_DATA);
        }
    }
    else {
        array->data = NULL;
    }
    return array;
}

LNXPROC_ARRAY_T *lnxproc_array_free(LNXPROC_ARRAY_T * array)
{
    if (array) {
        if (array->data) {
            if (array->recursive) {
                int i;
                for (i = 0; i < array->size; i++) {
                    void *a;
                    memcpy(&a, array->data + (i * sizeof(void *)),
                           sizeof(void *));
                    if (a) {
                        a = lnxproc_array_free(a);
                    }
                }
            }
            free(array->data);
            array->data = NULL;
        }
        free(array);
        array = NULL;
    }
    return array;
}

int lnxproc_array_resize(LNXPROC_ARRAY_T * array, size_t size)
{
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    size_t osize = array->size;
    void *p = array_realloc(array->data, osize, size);
    if (!p) {
        lnxproc_set_error(array->callback, __func__,
                          LNXPROC_ERROR_ARRAY_REALLOC_DATA);
        return LNXPROC_ERROR_ARRAY_REALLOC_DATA;
    }
    array->data = p;
    array->size += size;
    return LNXPROC_OK;
}

void *lnxproc_array_addr(LNXPROC_ARRAY_T * array, size_t idx)
{
    void *val = NULL;
    if (!array) {
        return val;
    }
    if (idx >= array->size) {
        lnxproc_set_error(array->callback, __func__,
                          LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE);
        return val;
    }
    val = array->data + (idx * sizeof(void *));
    return val;
}

void *lnxproc_array_get(LNXPROC_ARRAY_T * array, size_t idx)
{
    void *val = NULL;
    if (!array) {
        return val;
    }
    if (idx >= array->size) {
        lnxproc_set_error(array->callback, __func__,
                          LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE);
        return val;
    }
    memcpy(&val, array->data + (idx * sizeof(void *)), sizeof(void *));
    return val;
}

size_t lnxproc_array_size(LNXPROC_ARRAY_T * array)
{
    size_t size = -1;
    if (!array) {
        return size;
    }
    size = array->size;
    return size;
}

int lnxproc_array_set(LNXPROC_ARRAY_T * array, size_t idx, void *val)
{
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    if (idx < array->length) {
        memcpy(array->data + (idx * sizeof(void *)), &val, sizeof(void *));
    }
    else if (idx == array->length) {
        if (idx == array->size) {
            int err = lnxproc_array_resize(array, 1);
            if (err) {
                lnxproc_set_error(array->callback, __func__, err);
                return err;
            }
        }
        memcpy(array->data + (idx * sizeof(void *)), &val, sizeof(void *));
        array->length++;
    }
    else {
        lnxproc_set_error(array->callback, __func__,
                          LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE;
    }
    return LNXPROC_OK;
}

int lnxproc_array_set_last(LNXPROC_ARRAY_T * array, size_t idx, void *val)
{
    int ret = lnxproc_array_set(array, idx, val);
    if (ret == LNXPROC_OK) {
        array->length = idx + 1;
    }
    return ret;
}

int lnxproc_array_append(LNXPROC_ARRAY_T * array, void *val)
{
    return lnxproc_array_set(array, array->length, val);
}

int lnxproc_array_set_length(LNXPROC_ARRAY_T * array, size_t idx)
{
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    if (idx >= array->size) {
        lnxproc_set_error(array->callback, __func__,
                          LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE);
        return LNXPROC_ERROR_ARRAY_INDEX_OUT_OF_RANGE;
    }
    array->length = idx + 1;
    return LNXPROC_OK;
}

int lnxproc_array_iterate(LNXPROC_ARRAY_T * array,
                          void *data,
                          int start, int end, LNXPROC_ARRAY_ITERATE_FUNC func)
{
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    int i;
    if (start < 0) {
        start = 0;
    }
    else if (start > array->length) {
        start = array->length;
    }
    if (end < 0) {
        end = array->length;
    }
    else if (end > array->size) {
        end = array->size;
    }
    for (i = start; i < end; i++) {
        func(array, data, i);
    }
    return LNXPROC_OK;
}

struct array_print_var_t {
    int depth;
};

static int array_print_depth(struct array_print_var_t *printvar)
{
    int depth = 0;
    if (printvar)
        depth = printvar->depth;
    return depth;
}

static void array_print_indent(int depth)
{
    int i;
    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static int array_print_internal(LNXPROC_ARRAY_T * array, void *data, int idx)
{
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    void *a = lnxproc_array_addr(array, idx);
    void *p = lnxproc_array_get(array, idx);
    int depth = array_print_depth(data);
    array_print_indent(depth);
    if (array->recursive) {
        struct array_print_var_t printvar = {
            .depth = depth + 1,
        };
        printf("--> %d:Addr %p Value %p\n", idx, a, p);
        if (p)
            lnxproc_array_print(p, &printvar);
    }
    else {
        printf("--> %d:Addr %p Value %p '%s'\n", idx, a, p, (char *) p);
    }
    return LNXPROC_OK;
}

int lnxproc_array_print(LNXPROC_ARRAY_T * array, void *data)
{
    int depth = array_print_depth(data);
    array_print_indent(depth);
    printf("Array at %p\n", array);
    if (!array) {
        return LNXPROC_ERROR_ARRAY_NULL;
    }
    array_print_indent(depth);
    printf("Array size %zd\n", array->size);
    array_print_indent(depth);
    printf("Array length %zd\n", array->length);
    array_print_indent(depth);
    printf("Array recursive %d\n", array->recursive);
    array_print_indent(depth);
    printf("Array data at %p\n", array->data);
    //array_iterate(array,data,-1,array->size,array_print_internal);
    lnxproc_array_iterate(array, data, -1, -1, array_print_internal);
    printf("\n");
    return LNXPROC_OK;
}

/* 
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab 
 */
