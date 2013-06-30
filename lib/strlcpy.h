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

#ifndef TOPIARY_STRLCPY_H
#define TOPIARY_STRLCPY_H 1

#define STRLCAT(dest,src,offset,size) \
        offset += strlcpy((dest) + (offset), (src), (size) - (offset))

/*
 * Replaces strncpy() - more efficient and returns the number of chars 
 * copied not including the terminating null so catenation can be 
 * implemented efficiently
 */
static inline int
strlcpy(void *dest, const void *src, int len)
{
    int i = 0;
    char *d = dest;
    char *s = (char *) src;

    while (*s && i < len - 1) {
        *d++ = *s++;
        i++;
    }
    *d = '\0';

    return i;
}

#endif                          // TOPIARY_STRLCPY_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
