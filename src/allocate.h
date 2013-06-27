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

#ifndef TOPIARY_ALLOCATE_H
#define TOPIARY_ALLOCATE_H 1

#include "config.h"

#include <stdlib.h>             //calloc,realloc,free

/*-----------------------------------------------------------------------------
 * Memory allocation
 */

static inline void *
Allocate(void *p, size_t size)
{
    if (p) {
        p = realloc(p, size);
    }
    else {
        p = calloc(1, size);
    }
    return p;
}

static inline void *
Destroy(void *p)
{
    if (p)
        free(p);
    return NULL;
}

#define DESTROY(p) p = Destroy(p)

#endif                          // TOPIARY_ALLOCATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
