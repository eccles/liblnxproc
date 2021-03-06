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

#ifndef TOPIARY_IO_H
#define TOPIARY_IO_H 1

#include "config.h"

#include <string.h>             //strlen()
#include <unistd.h>             //write()

/*-----------------------------------------------------------------------------
 * Direct I/O
 */

static inline ssize_t
writec(int fd, const char c)
{
    ssize_t nw;

    do {
        nw = write(fd, &c, 1);
        if (nw < 0)
            break;
    } while (nw < 1);
    return nw;
}

static inline ssize_t
writen(int fd, const void *ptr, size_t n)
{
    ssize_t nl = 0;
    ssize_t nw;

    while (nl < n) {
        nw = write(fd, ptr + nl, n - nl);
        if (nw < 0) {
            if (nl == 0)
                return -1;
            return nl;
        }
        nl += nw;
    }
    return n;
}

static inline ssize_t
writestring(int fd, const char *str)
{
    return writen(fd, str, strlen(str));
}

#endif                          // TOPIARY_IO_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
