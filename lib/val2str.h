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

#ifndef TOPIARY_VAL2STR_H
#define TOPIARY_VAL2STR_H 1

#include <stdio.h>              //snprintf
#include <stdarg.h>              //vsnprintf

/*-----------------------------------------------------------------------------
 * string conversions
 */

static inline int
mysnprintf(char *buf, size_t len, const char *fmt, ... ) 
{
    va_list ap;

    int n = 0;

    if (buf && (len > 2) ) {
        va_start(ap,fmt);
        n = vsnprintf(buf, len, fmt, ap);
        va_end(ap);
        if (n >= len) {
            n = len - 1;
        }
    }
    return n;
}

static inline int
fixed2str(float value, int width, int prec, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%*.*f", width, prec, value);
}

#define FIXEDCAT(dest,width, prec,src,offset,size) \
        offset += fixed2str((src), (prec), (dest) + (offset), (size) - (offset))

static inline int
float2str(float value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%f", value);
}

#define FLOATCAT(dest,src,offset,size) \
        offset += float2str((src), (dest) + (offset), (size) - (offset))

static inline int
ptr2str(void *value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%p", value);
}

#define PTRCAT(dest,src,offset,size) \
        offset += ptr2str((src), (dest) + (offset), (size) - (offset))

static inline int
int2str(int value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%d", value);
}

#define INTCAT(dest,src,offset,size) \
        offset += int2str((src), (dest) + (offset), (size) - (offset))

static inline int
sizet2str(size_t value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%zd", value);
}

#define SIZETCAT(dest,src,offset,size) \
        offset += sizet2str((src), (dest) + (offset), (size) - (offset))

static inline int
long2str(long value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%ld", value);
}

#define LONGCAT(dest,src,offset,size) \
        offset += long2str((src), (dest) + (offset), (size) - (offset))

static inline int
unsigned2str(unsigned value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%u", value);
}

#define UNSIGNEDCAT(dest,src,offset,size) \
        offset += unsigned2str((src), (dest) + (offset), (size) - (offset))

static inline int
unsignedlong2str(unsigned long value, char *buf, size_t len)
{
    return mysnprintf(buf, len, "%lu", value);
}

#define UNSIGNEDLONGCAT(dest,src,offset,size) \
        offset += unsignedlong2str((src), (dest) + (offset), (size) - (offset))

#endif                          // TOPIARY_VAL2STR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
