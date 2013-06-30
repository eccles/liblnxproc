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

#ifndef TOPIARY_MEMDUP_H
#define TOPIARY_MEMDUP_H 1

#include "config.h"

#include <string.h>             // memcpy

static inline void *
memdup(void *old, size_t len)
{
    void *ret = NULL;

    if (old && len > 0) {
        ret = calloc(1, len);
        if (ret) {
            memcpy(ret, old, len);
        }
    }
    return ret;
}

#endif                          // TOPIARY_MEMDUP_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
