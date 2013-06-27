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
