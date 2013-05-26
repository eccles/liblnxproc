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

#include <sys/time.h>           //gettimeofday
#include <stddef.h>             //offsetof
#include <stdio.h>              //snprintf
#include <stdlib.h>             //calloc
#include <string.h>             //memcpy

#include "util_private.h"

/*-----------------------------------------------------------------------------
 * Reference counting
 */

struct allocate_t {
    unsigned count;
    void *data[];
};
typedef struct allocate_t ALLOCATE_T;

void *
Acquire(void *p, size_t size)
{
    ALLOCATE_T *a = NULL;

    if (p) {
        a = p - offsetof(ALLOCATE_T, data);
        a->count++;
        return a->data;
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

#ifdef LNXPROC_UNUSED
int
NextRelease(void *p)
{
    int ret = 0;

    if (p) {
        ALLOCATE_T *a = p - offsetof(ALLOCATE_T, data);

        ret = a->count < 2 ? 1 : 0;
    }
    return ret;
}
#endif

void *
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

/*-----------------------------------------------------------------------------
 * Time stamp handling
 */

float
lnxproc_timeval_secs(struct timeval *tv)
{
    float res = 0.0;

    if (tv) {
        res = (1.e-6 * tv->tv_usec) + tv->tv_usec;
    }
    return res;
}

long
lnxproc_timeval_diff(struct timeval *start, struct timeval *end)
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
lnxproc_timeval_print(struct timeval *tv, char *buf, size_t len)
{
    if (tv && buf && (len > 0)) {
        snprintf(buf, len, "%lu.%06lu", (unsigned long) tv->tv_sec,
                 (unsigned long) tv->tv_usec);
    }
    return buf;
}

struct timeval
lnxproc_timeval(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
