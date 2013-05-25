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
#include <stdio.h>              //snprintf
#include <stdlib.h>             //calloc
#include <string.h>             //memcpy

/*
 * string conversions
 */
int
float2str(float value, char *buf, size_t len)
{
    int n = 0;

    if (buf && len > 2) {
        n = snprintf(buf, len, "%f", value);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

int
int2str(int value, char *buf, size_t len)
{
    int n = 0;

    if (buf && len > 2) {
        n = snprintf(buf, len, "%d", value);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

int
long2str(long value, char *buf, size_t len)
{
    int n = 0;

    if (buf && len > 2) {
        n = snprintf(buf, len, "%ld", value);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

int
unsigned2str(unsigned value, char *buf, size_t len)
{
    int n = 0;

    if (buf && len > 2) {
        n = snprintf(buf, len, "%u", value);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

int
unsignedlong2str(unsigned long value, char *buf, size_t len)
{
    int n = 0;

    if (buf && len > 2) {
        n = snprintf(buf, len, "%lu", value);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

/*
 * Reference counting
 */
#ifdef UNUSED
struct allocate_t {
    unsigned count;
    size_t size;
    void *data;
};
#endif

void *
Allocate(void *p, size_t size)
{
#ifdef UNUSED
    ALLOCATE_T *a = NULL;

    if (p) {
        a = p - sizeof(ALLOCATE_T);
        if (a->size == size) {
            return p;
        }
        ALLOCATE_T *b = realloc(a, sizeof(ALLOCATE_T) + size);

        if (!b)
            return NULL;
        if (b != a) {
            b->data = b + sizeof(ALLOCATE_T);
            b->size = size;
        }
        return b->data;
    }
    else {
        a = calloc(1, sizeof(ALLOCATE_T) + size);

        if (a) {
            a->data = a + sizeof(ALLOCATE_T);
            a->size = size;
            a->count = 1;
            return a->data;
        }
    }
    return NULL;
#else
    if (p) {
        return realloc(p, size);
    }
    else {
        p = calloc(1, size);
    }
    return p;
#endif
}

void *
Release(void *p)
{
    if (p)
#ifdef UNUSED
    {
        ALLOCATE_T *a = p - sizeof(ALLOCATE_T);

        a->count--;
        if (a->count < 1) {
            free(a);
        }
    }
#else
        free(p);
#endif
    return NULL;
}

void
Retain(void *p)
{
#ifdef UNUSED
    if (p) {
        ALLOCATE_T *a = p - sizeof(ALLOCATE_T);

        a->count++;
    }
#endif
}

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
