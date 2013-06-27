/*
 * This file is part of topiary.
 *
 *  topiary is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  topiary is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with topiary.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

/* This file is a re-implementation of the 'list' type from Python
 */

#include <stdio.h>              //printf
#include <string.h>             //memset

#include "allocate.h"
#include "error_private.h"
#include "vector_private.h"

static _TOPIARY_VECTOR_T **
vector_malloc_children(size_t size)
{
    return Allocate(NULL, size * sizeof(_TOPIARY_VECTOR_T *));
}

static _TOPIARY_VECTOR_T **
vector_realloc_children(_TOPIARY_VECTOR_T **old, size_t oldsize, size_t addsize)
{
    _TOPIARY_DEBUG("Old %p oldsize %zd addsize %zd\n", old, oldsize, addsize);

    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(_TOPIARY_VECTOR_T *);

    _TOPIARY_DEBUG("nsize %zd n %zd\n", nsize, n);

    _TOPIARY_VECTOR_T **p = Allocate(old, n);

    _TOPIARY_DEBUG("New %p\n", p);
    if (p) {
        _TOPIARY_VECTOR_T **extra = p + oldsize;

        _TOPIARY_DEBUG("Extra %p\n", extra);

        n = addsize * sizeof(_TOPIARY_VECTOR_T *);
        _TOPIARY_DEBUG("n %zd\n", n);
        memset(extra, 0, n);
    }

    return p;
}

static char **
vector_malloc_values(size_t size)
{
    return Allocate(NULL, size * sizeof(char *));
}

static char **
vector_realloc_values(char **old, size_t oldsize, size_t addsize)
{
    _TOPIARY_DEBUG("Old %p oldsize %zd addsize %zd\n", old, oldsize, addsize);

    size_t nsize = oldsize + addsize;
    size_t n = nsize * sizeof(char *);

    _TOPIARY_DEBUG("nsize %zd n %zd\n", nsize, n);

    char **p = Allocate(old, n);

    _TOPIARY_DEBUG("New %p\n", p);

    if (p) {
        char **extra = p + oldsize;

        _TOPIARY_DEBUG("Extra %p\n", extra);

        n = addsize * sizeof(char *);
        _TOPIARY_DEBUG("n %zd\n", n);
        memset(extra, 0, n);
    }

    return p;
}

int
_topiary_vector_new(_TOPIARY_VECTOR_T **vector, size_t size, int recursive)
{

    _TOPIARY_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _TOPIARY_DEBUG("sizeof _TOPIARY_VECTOR_T %lu\n", sizeof(_TOPIARY_VECTOR_T));
    _TOPIARY_DEBUG("Size %zd Recursive %d\n", size, recursive);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (*vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    _TOPIARY_VECTOR_T *p = Allocate(NULL, sizeof(_TOPIARY_VECTOR_T));

    if (!p) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Vector");
        return TOPIARY_ERROR_MALLOC;
    }

    p->length = 0;
    p->size = size;
    p->recursive = recursive;

    /* We have to malloc this separately as other objects may hold
     * references to 'vector' and we do not want 'vector' to change on a
     * realloc
     */
    if (size > 0) {
        _TOPIARY_DEBUG("Malloc %zd words of child\n", size);

        if (recursive) {
            p->children = vector_malloc_children(size);

            if (!p->children) {
                _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Vector data");
                _TOPIARY_VECTOR_FREE(p);
                return TOPIARY_ERROR_MALLOC;
            }
        }

        p->values = vector_malloc_values(size);

        if (!p->values) {
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Vector values");
            _TOPIARY_VECTOR_FREE(p);
            return TOPIARY_ERROR_MALLOC;
        }
    }

    *vector = p;
    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_vector_free(_TOPIARY_VECTOR_T **vectorptr)
{
    _TOPIARY_DEBUG("Vector %p\n", vectorptr);

    if (vectorptr && *vectorptr) {
        _TOPIARY_VECTOR_T *vector = *vectorptr;

        if (vector->children) {
            int i;

            for (i = 0; i < vector->size; i++) {
                _TOPIARY_VECTOR_FREE(vector->children[i]);
            }
            DESTROY(vector->children);
        }
        DESTROY(vector->values);
        _TOPIARY_DEBUG("Free vector %p\n", vector);
        DESTROY(vector);
        *vectorptr = NULL;
    }

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

struct vector_size_t {
    size_t size;
};
static int
vector_size_internal(_TOPIARY_VECTOR_T *vector, int idx, int depth, void *data)
{
    _TOPIARY_DEBUG("Vector %p Idx %d, Depth %d Data %p\n", vector, idx, depth,
                   data);

    struct vector_size_t *vsize = data;

    vsize->size += sizeof(*vector);
    if (vector->children) {
        vsize->size += sizeof(vector->size * sizeof(*(vector->children)));
    }
    if (vector->values) {
        vsize->size += sizeof(vector->size * sizeof(*(vector->values)));
    }
    return TOPIARY_OK;
}

int
_topiary_vector_size(_TOPIARY_VECTOR_T *vector, size_t * size)
{
    if (!size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Size");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    struct vector_size_t vsize = {.size = 0, };

    _topiary_vector_iterate(vector, 0, 0, 1, &vsize, vector_size_internal);
    *size = vsize.size;
    return TOPIARY_OK;
}

int
_topiary_vector_resize(_TOPIARY_VECTOR_T *vector, size_t size)
{
    _TOPIARY_DEBUG("Vector %p Size %zd\n", vector, size);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    size_t osize = vector->size;

    if (vector->recursive) {
        _TOPIARY_VECTOR_T **p =
            vector_realloc_children(vector->children, osize, size);

        if (!p) {
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC,
                                 "Realloc vector children");
            return TOPIARY_ERROR_MALLOC;
        }

        if (p != vector->children) {
            _TOPIARY_DEBUG("Children of vector %p moved from %p to %p\n",
                           vector, vector->children, p);
            vector->children = p;
        }
    }
    char **q = vector_realloc_values(vector->values, osize, size);

    if (!q) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Realloc vector values");
        return TOPIARY_ERROR_MALLOC;
    }
    if (q != vector->values) {
        _TOPIARY_DEBUG("Values of vector %p moved from %p to %p\n", vector,
                       vector->values, q);

        _TOPIARY_VECTOR_T *par = vector->parent;

        _TOPIARY_DEBUG("Parent of vector %p is %p\n", vector, par);

        if (par) {
            int i;

            for (i = 0; i < par->size; i++) {
                if (par->values[i] == (char *) vector->values) {
                    _TOPIARY_DEBUG
                        ("Changed parent vector at position %d from %p to %p\n",
                         i, par->values[i], q);
                    par->values[i] = (char *) q;
                }
            }
        }
        vector->values = q;
    }
    vector->size += size;
    _TOPIARY_DEBUG("Success\n");

    return TOPIARY_OK;
}

int
_topiary_vector_child(_TOPIARY_VECTOR_T *vector, size_t idx,
                      _TOPIARY_VECTOR_T **child)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!child) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector child");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (*child) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector child");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!vector->recursive || !vector->children) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector not recursive");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (idx >= vector->size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Idx %zd >= Size %zd", idx, vector->size);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    *child = vector->children[idx];
    _TOPIARY_DEBUG("Success (child = %p)\n", *child);
    return TOPIARY_OK;
}

int
_topiary_vector_value(_TOPIARY_VECTOR_T *vector, size_t idx, char **value)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!value) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector value");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (*value) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector value");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (vector->recursive || !vector->values) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector is recursive");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx >= vector->size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Idx %zd >= Size %zd", idx, vector->size);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    *value = vector->values[idx];
    _TOPIARY_DEBUG("Success (val = %p '%s')\n", *value, *value);
    return TOPIARY_OK;
}

int
_topiary_vector_set_child(_TOPIARY_VECTOR_T *vector, size_t idx,
                          _TOPIARY_VECTOR_T *child)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!vector->recursive || !vector->children || !vector->values) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Vector is not recursive");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx > vector->length) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Index %zd Length %zd", idx, vector->length);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx == vector->length) {
        _TOPIARY_DEBUG("Append entry Idx %zd Length %zd\n", idx,
                       vector->length);

        if (idx >= vector->size) {
            _TOPIARY_DEBUG("Resize data Size %zd\n", vector->size);
            int ret = _topiary_vector_resize(vector, 1);

            if (ret) {
                return ret;
            }
        }
        vector->length = idx + 1;
        _TOPIARY_DEBUG("Vector length is now %zd\n", vector->length);
    }
    _TOPIARY_DEBUG("Set Vector %p child %zd to %p\n", vector, idx, child);
    vector->children[idx] = child;
    _TOPIARY_DEBUG("Set parent of %p to %p(%zd)\n", child, vector, idx);
    child->parent = vector;
    child->idx = idx;
    _TOPIARY_DEBUG("Set Vector %p value %zd to %p\n", vector, idx,
                   child->values);
    vector->values[idx] = (char *) child->values;

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_vector_set_value(_TOPIARY_VECTOR_T *vector, size_t idx, char *val)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd Val %p '%s'\n", vector, idx, val, val);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (vector->recursive || !vector->values) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector is recursive");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx > vector->length) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Index %zd Length %zd", idx, vector->length);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx == vector->length) {
        _TOPIARY_DEBUG("Append entry Idx %zd Length %zd\n", idx,
                       vector->length);

        if (idx == vector->size) {
            _TOPIARY_DEBUG("Resize data Size %zd\n", vector->size);
            int ret = _topiary_vector_resize(vector, 1);

            if (ret) {
                return ret;
            }
        }

        vector->length = idx + 1;
        _TOPIARY_DEBUG("Vector length is now %zd\n", vector->length);

    }

    _TOPIARY_DEBUG("Set Vector %p entry Idx %zd to %p\n", vector, idx, val);
    vector->values[idx] = val;

    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

int
_topiary_vector_set_last_child(_TOPIARY_VECTOR_T *vector, size_t idx,
                               _TOPIARY_VECTOR_T *child)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd Child %p\n", vector, idx, child);

    int ret = _topiary_vector_set_child(vector, idx, child);

    if (ret == TOPIARY_OK) {
        vector->length = idx + 1;
        _TOPIARY_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

int
_topiary_vector_set_last_value(_TOPIARY_VECTOR_T *vector, size_t idx, char *val)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd Val %p\n", vector, idx, val);

    int ret = _topiary_vector_set_value(vector, idx, val);

    if (ret == TOPIARY_OK) {
        vector->length = idx + 1;
        _TOPIARY_DEBUG("Vector length is now %zd\n", vector->length);
    }

    return ret;
}

int
_topiary_vector_set_length(_TOPIARY_VECTOR_T *vector, size_t idx)
{
    _TOPIARY_DEBUG("Vector %p Idx %zd\n", vector, idx);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (idx < 0 || idx >= vector->size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Index %zd Size %zd", idx, vector->size);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    vector->length = idx + 1;
    _TOPIARY_DEBUG("Vector length is now %zd\n", vector->length);
    return TOPIARY_OK;
}

int
_topiary_vector_iterate(_TOPIARY_VECTOR_T *vector,
                        int idx, int depth, int allocated, void *data,
                        _TOPIARY_VECTOR_ITERATE_FUNC func)
{
    _TOPIARY_DEBUG("Vector %p Depth %d Data %p Func %p\n", vector,
                   depth, data, func);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
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

        _TOPIARY_DEBUG("Iterate from 0 to %d\n", end);
        int i;

        depth++;
        for (i = 0; i < end; i++) {
            if (vector->children[i]) {
                _topiary_vector_iterate(vector->children[i], i, depth,
                                        allocated, data, func);
            }
        }
        depth--;
    }
    _TOPIARY_DEBUG("Success\n");
    return TOPIARY_OK;
}

static void
vector_print_depth(int depth)
{
    int i;

    for (i = 0; i < depth; i++) {
        printf("    ");
    }
}

static int
vector_print_internal(_TOPIARY_VECTOR_T *vector, int idx, int depth, void *data)
{
    _TOPIARY_DEBUG("Vector %p Idx %d, Depth %d Data %p\n", vector, idx, depth,
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
    return TOPIARY_OK;
}

int
_topiary_vector_print(_TOPIARY_VECTOR_T *vector, int allocated, void *data)
{
    _TOPIARY_DEBUG("Vector %p Data %p\n", vector, data);

    if (!vector) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Vector");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    _topiary_vector_iterate(vector, 0, 0, allocated, NULL,
                            vector_print_internal);
    printf("\n");
    return TOPIARY_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
