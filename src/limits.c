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

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "limits.h"

char *
lnxproc_limit_chr(LNXPROC_LIMITS_T * limit, char c)
{

/*
 * replaces
 * char *ret = strchr(limit->chars, c); 
 *
 * because we want to include the trailing NUL char if required
 */
    int i;

    for (i = 0; i < limit->len; i++) {
        if (c == limit->chars[i]) {
            return limit->chars + i;
        }
    }
    return NULL;
}

int
lnxproc_limits_print(LNXPROC_LIMITS_T limits[], int dim)
{
    if (!limits) {
        return 1;
    }

    int i;

    for (i = 0; i < dim; i++) {
        char buf[64];
        char *p = lnxproc_limit_print(limits + i, buf, sizeof buf);

        printf("Limit %d :%s:\n", i, p);
    }
    return LNXPROC_OK;
}

char *
lnxproc_limit_print(LNXPROC_LIMITS_T * limit, char *buf, size_t buflen)
{
    buf[0] = '\0';
    if (!limit) {
        return buf;
    }

    int m = 0;
    char *b = buf;

    m = snprintf(b, buflen, "Exp %zd: Len %d: ", limit->expected, limit->len);
    b += m;
    buflen -= m;

    const char *s = limit->chars;
    int n = limit->len;

    char *cstr[] = {
        "NUL '\\0'", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK",
        "BEL '\\a'", "BS '\\b'", "HT '\\t'", "LF '\\n'", "VT '\\v'", "FF '\\f'",
        "CR '\\r'",
        "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
        "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE",
    };

    int i;
    char c;

    for (i = 0; i < n; i++) {
        c = s[i];
        if (c < 33) {
            m = snprintf(b, buflen, "%s ", cstr[(int) c]);
            b += m;
            buflen -= m;
        }
        else if (c == 127) {
            m = snprintf(b, buflen, "DEL ");
            b += m;
            buflen -= m;
        }
        else if (c > 127) {
            m = snprintf(b, buflen, "%d ", c);
            b += m;
            buflen -= m;
        }
        else {
            m = snprintf(b, buflen, "%c ", c);
            b += m;
            buflen -= m;
        }
    }

    return buf;
}

LNXPROC_LIMITS_T *
lnxproc_limits_dup(LNXPROC_ERROR_CALLBACK callback,
                   LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_LIMITS_T *newlimits = NULL;

    if (dim > 0 && limits) {
        LNXPROC_DEBUG("Malloc limits %zd\n", dim);
        newlimits = malloc(dim * sizeof(LNXPROC_LIMITS_T));
        if (!newlimits) {
            LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_MALLOC_LIMITS);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC_LIMITS, "Malloc limits\n");
            return newlimits;
        }
        LNXPROC_DEBUG("Malloc limits %p\n", newlimits);
        int i;

        for (i = 0; i < dim; i++) {

#ifdef DEBUG
            char buf[64];
            char *p = lnxproc_limit_print(limits + i, buf, sizeof buf);

            LNXPROC_DEBUG("Malloc old limit %zd :%s:\n", i, p);
#endif
            newlimits[i].chars = malloc(limits[i].len * sizeof(char));
            if (!newlimits[i].chars) {
                LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_MALLOC_LIMITS_ENTRY);
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC_LIMITS_ENTRY,
                                    "Malloc limits entry\n");
                int j;

                for (j = i - 1; j >= 0; j--) {
                    free(newlimits[j].chars);
                }
                free(newlimits);
                return NULL;
            }
            memcpy(newlimits[i].chars, limits[i].chars, limits[i].len);
            newlimits[i].expected = limits[i].expected;
            newlimits[i].len = limits[i].len;

#ifdef DEBUG
            p = lnxproc_limit_print(newlimits + i, buf, sizeof buf);
            LNXPROC_DEBUG("Malloc new limit %zd :%s:\n", i, p);
#endif

        }
    }
    return newlimits;
}

LNXPROC_LIMITS_T *
lnxproc_limits_free(LNXPROC_LIMITS_T limits[], size_t dim)
{
    if (dim > 0 && limits) {
        LNXPROC_DEBUG("Free Limits buffer %zd\n", dim);

        int i;

        for (i = 0; i < dim; i++) {
            LNXPROC_DEBUG("Free Limit buffer %d\n", i);
            free(limits[i].chars);
            limits[i].chars = NULL;
        }

        LNXPROC_DEBUG("Free Limits buffer %p\n", limits);
        free(limits);
    }
    return NULL;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
