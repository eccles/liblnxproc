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

#ifndef TOPIARY_VAL2STR_H
#define TOPIARY_VAL2STR_H 1

#include <stdio.h>              //snprintf

/*-----------------------------------------------------------------------------
 * string conversions
 */
#define SNPRINTF(buf,len,fmt, args... ) do {\
    int n = 0;\
\
    if ((buf) && (len) > 2) {\
        n = snprintf((buf), (len), (fmt), ##args);\
        if (n >= (len)) {\
            n = (len) - 1;\
        }\
    }\
    return n;\
} while(0)

static inline int
fixed2str(float value, int width, int prec, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%*.*f", width, prec, value);
}

#define FIXEDCAT(dest,width, prec,src,offset,size) \
        offset += fixed2str((src), (prec), (dest) + (offset), (size) - (offset))

static inline int
float2str(float value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%f", value);
}

#define FLOATCAT(dest,src,offset,size) \
        offset += float2str((src), (dest) + (offset), (size) - (offset))

static inline int
ptr2str(void *value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%p", value);
}

#define PTRCAT(dest,src,offset,size) \
        offset += ptr2str((src), (dest) + (offset), (size) - (offset))

static inline int
int2str(int value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%d", value);
}

#define INTCAT(dest,src,offset,size) \
        offset += int2str((src), (dest) + (offset), (size) - (offset))

static inline int
sizet2str(size_t value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%zd", value);
}

#define SIZETCAT(dest,src,offset,size) \
        offset += sizet2str((src), (dest) + (offset), (size) - (offset))

static inline int
long2str(long value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%ld", value);
}

#define LONGCAT(dest,src,offset,size) \
        offset += long2str((src), (dest) + (offset), (size) - (offset))

static inline int
unsigned2str(unsigned value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%u", value);
}

#define UNSIGNEDCAT(dest,src,offset,size) \
        offset += unsigned2str((src), (dest) + (offset), (size) - (offset))

static inline int
unsignedlong2str(unsigned long value, char *buf, size_t len)
{
    SNPRINTF(buf, len, "%lu", value);
}

#define UNSIGNEDLONGCAT(dest,src,offset,size) \
        offset += unsignedlong2str((src), (dest) + (offset), (size) - (offset))

#endif                          // TOPIARY_VAL2STR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
