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

#ifndef LIBLNXPROC_REFERENCE_H
#define LIBLNXPROC_REFERENCE_H 1

#include <stddef.h>             //offsetof
#include <stdlib.h>             //calloc

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

#define RELEASE(p,f) p = Release(p,f)

#endif                          // LIBLNXPROC_REFERENCE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
