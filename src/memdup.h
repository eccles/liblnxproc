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

#ifndef TOPIARY_MEMDUP_H
#define TOPIARY_MEMDUP_H 1

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
