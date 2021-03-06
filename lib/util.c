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

#include "config.h"

#include <sys/time.h>           //gettimeofday
#include <stdio.h>              //snprintf

#include "util_private.h"

/*-----------------------------------------------------------------------------
 * Time stamp handling
 */

float
topiary_timeval_secs(struct timeval *tv)
{
    float res = 0.0;

    if (tv) {
        res = (1.e-6 * tv->tv_usec) + tv->tv_usec;
    }
    return res;
}

long
topiary_timeval_diff(struct timeval *start, struct timeval *end)
{
    long res = 0;

    if (start && end) {
        long secdiff = end->tv_sec - start->tv_sec;
        long usecdiff = (long) end->tv_usec - (long) start->tv_usec;

        if (usecdiff < 0) {
            usecdiff += 1000000;
            secdiff -= 1;
        }
        res = usecdiff + (1000000 * secdiff);
    }
    return res;
}

char *
topiary_timeval_print(struct timeval *tv, char *buf, size_t len)
{
    if (tv && buf && (len > 0)) {
        snprintf(buf, len, "%lu.%06lu", (unsigned long) tv->tv_sec,
                 (unsigned long) tv->tv_usec);
    }
    return buf;
}

struct timeval
topiary_timeval(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
