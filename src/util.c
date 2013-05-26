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

/*-----------------------------------------------------------------------------
 * string conversions
 */
#define SNPRINTF(buf,len,fmt,value) {\
    int n = 0;\
\
    if ((buf) && (len) > 2) {\
        n = snprintf((buf), (len), (fmt), (value));\
        if (n >= (len)) {\
            n = (len) - 1;\
        }\
    }\
    return n;\
}

int
ptr2str(void *value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%p", value);
}

int
float2str(float value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%f", value);
}

int
int2str(int value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%d", value);
}

int
sizet2str(size_t value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%zd", value);
}

int
long2str(long value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%ld", value);
}

int
unsigned2str(unsigned value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%u", value);
}

int
unsignedlong2str(unsigned long value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%lu", value);
}

/*-----------------------------------------------------------------------------
 * Memory allocation
 */

void *
Allocate(void *p, size_t size)
{
    if (p) {
        return realloc(p, size);
    }
    else {
        p = calloc(1, size);
    }
    return p;
}

void *
Destroy(void *p)
{
    if (p)
        free(p);
    return NULL;
}

/*-----------------------------------------------------------------------------
 * Reference counting
 */

struct allocate_t {
    unsigned count;
    void *data[];
};
typedef struct allocate_t ALLOCATE_T;

void *
Acquire(size_t size)
{
    ALLOCATE_T *a = NULL;

    a = calloc(1, sizeof(ALLOCATE_T) + size);

    if (a) {
        a->count = 1;
        return a->data;
    }
    return NULL;
}

void *
Release(void *p)
{
    if (p) {
        ALLOCATE_T *a = p - offsetof(ALLOCATE_T, data);

        a->count--;
        if (a->count < 1) {
            free(a);
        }
    }
    return NULL;
}

void *
Retain(void *p)
{
    if (p) {
        ALLOCATE_T *a = p - offsetof(ALLOCATE_T, data);

        a->count++;
    }
    return p;
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
