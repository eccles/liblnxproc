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

#ifndef LIBLNXPROC_VAL2STR_H
#define LIBLNXPROC_VAL2STR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>              //snprintf

/*-----------------------------------------------------------------------------
 * string conversions
 */
#define SNPRINTF(buf,len,fmt,value) do {\
    int n = 0;\
\
    if ((buf) && (len) > 2) {\
        n = snprintf((buf), (len), (fmt), (value));\
        if (n >= (len)) {\
            n = (len) - 1;\
        }\
    }\
    return n;\
} while(0)

/*
 * float
 */
    static inline int
     float2str(float value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%f", value);
    }
/*
 * pointer
 */ static inline int
     ptr2str(void *value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%p", value);
    }

/*
 * int
 */
    static inline int
     int2str(int value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%d", value);
    }

/*
 * size_t
 */
    static inline int
     sizet2str(size_t value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%zd", value);
    }

/*
 * long
 */
    static inline int
     long2str(long value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%ld", value);
    }

/*
 * unsigned
 */
    static inline int
     unsigned2str(unsigned value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%u", value);
    }

/*
 * unsigned long
 */
    static inline int
     unsignedlong2str(unsigned long value, char *buf, size_t len) {
        SNPRINTF(buf, len, "%lu", value);
    }

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_VAL2STR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
