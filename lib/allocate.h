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

#ifndef TOPIARY_ALLOCATE_H
#define TOPIARY_ALLOCATE_H 1

#include "config.h"

#include "topiary_stdlib.h"     //calloc,realloc,free

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
