/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef TOPIARY_REFERENCE_H
#define TOPIARY_REFERENCE_H 1

#include "config.h"

#include <stddef.h>             //offsetof
#include "topiary_stdlib.h"     //calloc

/*-----------------------------------------------------------------------------
 * Reference counting
 */

struct allocate_t {
    unsigned count;
    void *data[];
};
typedef struct allocate_t ALLOCATE_T;

static inline void *
Acquire(void *p, size_t size)
{
    ALLOCATE_T *a = NULL;

    if (p) {
        a = p - offsetof(ALLOCATE_T, data);

        a->count++;
        return p;
    }
    else {

        a = calloc(1, sizeof(ALLOCATE_T) + size);

        if (a) {
            a->count = 1;
            return a->data;
        }
    }
    return NULL;
}

typedef void (*RELEASE_METHOD) (void *);

static inline void *
Release(void *p, RELEASE_METHOD func)
{
    if (p) {
        ALLOCATE_T *a = p - offsetof(ALLOCATE_T, data);

        a->count--;
        if (a->count < 1) {
            if (func)
                func(a->data);
            free(a);
        }
    }
    return NULL;
}

#define RELEASE(p,f) p = Release((p),f)

#endif                          // TOPIARY_REFERENCE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
