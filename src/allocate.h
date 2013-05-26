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

#ifndef LIBLNXPROC_ALLOCATE_H
#define LIBLNXPROC_ALLOCATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>             //calloc
#include <string.h>             //memcpy

/*-----------------------------------------------------------------------------
 * Memory allocation
 */

    static inline void *Allocate(void *p, size_t size) {
        if (p) {
            return realloc(p, size);
        }
        else {
            p = calloc(1, size);
        }
        return p;
    }

    static inline void *Destroy(void *p) {
        if (p)
            free(p);
        return NULL;
    }

#define DESTROY(p) p = Destroy(p)

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ALLOCATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
