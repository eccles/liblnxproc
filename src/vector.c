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
    return calloc(size, sizeof(_LNXPROC_VECTOR_T *));
}

static _LNXPROC_VECTOR_T **
vector_realloc_children(_LNXPROC_VECTOR_T ** old, size_t oldsize,
                        size_t addsize)
{
    _LNXPROC_DEBUG("Old %p oldsize %zd addsize %zd\n", old, oldsize, addsize);

    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(_LNXPROC_VECTOR_T *);

    _LNXPROC_DEBUG("nsize %zd n %zd\n", nsize, n);

    _LNXPROC_VECTOR_T **p = realloc(old, n);

    _LNXPROC_DEBUG("New %p\n", p);
    if (p) {
        _LNXPROC_VECTOR_T **extra = p + oldsize;

        _LNXPROC_DEBUG("Extra %p\n", extra);

        n = addsize * sizeof(_LNXPROC_VECTOR_T *);
        _LNXPROC_DEBUG("n %zd\n", n);
        memset(extra, 0, n);
    }

    return p;
}

static char **
vector_malloc_values(size_t size)
{
    return calloc(size, sizeof(char *));
}

static char **
vector_realloc_values(char **old, size_t oldsize, size_t addsize)
{
    _LNXPROC_DEBUG("Old %p oldsize %zd addsize %zd\n", old, oldsize, addsize);

    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(char *);

    _LNXPROC_DEBUG("nsize %zd n %zd\n", nsize, n);

    char **p = realloc(old, n);

    _LNXPROC_DEBUG("New %p\n", p);

    if (p) {
        char **extra = p + oldsize;

        _LNXPROC_DEBUG("Extra %p\n", extra);

        n = addsize * sizeof(char *);
        _LNXPROC_DEBUG("n %zd\n", n);
        memset(extra, 0, n);
    }

    return p;
}

LNXPROC_ERROR_T
_lnxproc_vector_new(_LNXPROC_VECTOR_T ** vector, size_t size, int recursive)
{

    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_VECTOR_T %lu\n", sizeof(_LNXPROC_VECTOR_T));
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

        if (recursive) {
            p->children = vector_malloc_children(size);

            if (!p->children) {
                _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_DATA,
                                     "Malloc data\n");
                _LNXPROC_VECTOR_FREE(p);
                return LNXPROC_ERROR_VECTOR_MALLOC_DATA;
            }
        }

        p->values = vector_malloc_values(size);

        if (!p->values) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_MALLOC_DATA,
                                 "Malloc data\n");
            _LNXPROC_VECTOR_FREE(p);
            return LNXPROC_ERROR_VECTOR_MALLOC_DATA;
        }
    }

    *vector = p;
    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_vector_free(_LNXPROC_VECTOR_T ** vectorptr)
{
    _LNXPROC_DEBUG("Vector %p\n", vectorptr);

    if (vectorptr && *vectorptr) {
        _LNXPROC_VECTOR_T *vector = *vectorptr;

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
    return LNXPROC_OK;
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

    if (vector->recursive) {
        _LNXPROC_VECTOR_T **p =
            vector_realloc_children(vector->children, osize, size);

        if (!p) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_REALLOC_DATA,
                                 "Realloc children\n");
            return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
        }

        if (p != vector->children) {
            _LNXPROC_DEBUG("Children of vector %p moved from %p to %p\n",
                           vector, vector->children, p);
            vector->children = p;
        }
    }
    char **q = vector_realloc_values(vector->values, osize, size);

    if (!q) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_REALLOC_DATA,
                             "Realloc values\n");
        return LNXPROC_ERROR_VECTOR_REALLOC_DATA;
    }
    if (q != vector->values) {
        _LNXPROC_DEBUG("Values of vector %p moved from %p to %p\n", vector,
                       vector->values, q);

        _LNXPROC_VECTOR_T *par = vector->parent;

        _LNXPROC_DEBUG("Parent of vector %p is %p\n", vector, par);

        if (par) {
            int i;

            for (i = 0; i < par->size; i++) {
                if (par->values[i] == (char *) vector->values) {
                    _LNXPROC_DEBUG
                        ("Changed parent vector at position %d from %p to %p\n",
                         i, par->values[i], q);
                    par->values[i] = (char *) q;
                }
            }
        }
        vector->values = q;
    }
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

    if (idx > vector->length) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    if (idx == vector->length) {
        _LNXPROC_DEBUG("Append entry Idx %zd Length %zd\n", idx,
                       vector->length);

        if (idx >= vector->size) {
            _LNXPROC_DEBUG("Resize data Size %zd\n", vector->size);
            LNXPROC_ERROR_T ret = _lnxproc_vector_resize(vector, 1);

            if (ret) {
                _LNXPROC_ERROR_DEBUG(ret, "\n");
                return ret;
            }
        }
        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);
    }
    _LNXPROC_DEBUG("Set Vector %p child %zd to %p\n", vector, idx, child);
    vector->children[idx] = child;
    _LNXPROC_DEBUG("Set parent of %p to %p(%zd)\n", child, vector, idx);
    child->parent = vector;
    child->idx = idx;
    _LNXPROC_DEBUG("Set Vector %p value %zd to %p\n", vector, idx,
                   child->values);
    vector->values[idx] = (char *) child->values;

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

    if (idx > vector->length) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE,
                             "Index %zd Length %zd\n", idx, vector->length);
        return LNXPROC_ERROR_VECTOR_INDEX_OUT_OF_RANGE;
    }

    if (idx == vector->length) {
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

        vector->length = idx + 1;
        _LNXPROC_DEBUG("Vector length is now %zd\n", vector->length);

    }

    _LNXPROC_DEBUG("Set Vector %p entry Idx %zd to %p\n", vector, idx, val);
    vector->values[idx] = val;

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
                        int idx, int depth, int allocated, void *data,
                        _LNXPROC_VECTOR_ITERATE_FUNC func)
{
    _LNXPROC_DEBUG("Vector %p Depth %d Data %p Func %p\n", vector,
                   depth, data, func);

    if (!vector) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_VECTOR_NULL, "\n");
        return LNXPROC_ERROR_VECTOR_NULL;
    }

    func(vector, idx, depth, data);

    if (vector->children) {
        int end;

        if (allocated) {
            end = vector->size;
        }
        else {
            end = vector->length;
        }

        _LNXPROC_DEBUG("Iterate from 0 to %d\n", end);
        int i;

        depth++;
        for (i = 0; i < end; i++) {
            if (vector->children[i]) {
                _lnxproc_vector_iterate(vector->children[i], i, depth,
                                        allocated, data, func);
            }
        }
        depth--;
    }
    _LNXPROC_DEBUG("Success\n");
    return LNXPROC_OK;
}

static void
vector_print_depth(int depth)
{
    int i;

    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static LNXPROC_ERROR_T
vector_print_internal(_LNXPROC_VECTOR_T * vector, int idx, int depth,
                      void *data)
{
    _LNXPROC_DEBUG("Vector %p Idx %d, Depth %d Data %p\n", vector, idx, depth,
                   data);

    vector_print_depth(depth);
    printf("Vector at %p(%d)\n", vector, idx);
    vector_print_depth(depth);
    printf("Vector parent %p(%d)\n", vector->parent, vector->idx);
    vector_print_depth(depth);
    printf("Vector size %zd\n", vector->size);
    vector_print_depth(depth);
    printf("Vector length %zd\n", vector->length);
    vector_print_depth(depth);
    printf("Vector recursive %d\n", vector->recursive);
    if (vector->children) {
        vector_print_depth(depth);
        printf("Vector children at %p\n", vector->children);
        vector_print_depth(depth);
        printf("Vector values at %p\n", vector->values);
        if (vector->values) {
            int i;

            for (i = 0; i < vector->length; i++) {
                vector_print_depth(depth);
                printf("--> %1$d: %2$p\n", i, vector->values[i]);
            }
        }
    }
    else if (vector->values) {
        vector_print_depth(depth);
        printf("Vector values at %p\n", vector->values);
        int i;

        for (i = 0; i < vector->length; i++) {
            vector_print_depth(depth);
            printf("--> %1$d: %2$p '%2$s'\n", i, vector->values[i]);
        }
    }
    printf("\n");
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

    _lnxproc_vector_iterate(vector, 0, 0, allocated, NULL,
                            vector_print_internal);
    printf("\n");
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
