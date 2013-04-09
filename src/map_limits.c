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
#include "map_limits.h"

char *
lnxproc_map_chr(LNXPROC_MAP_LIMITS_T * maplimit, char c)
{
    char *ret = NULL;

//    ret = strchr(maplimit->chars, c);
    int i;

    for (i = 0; i < maplimit->len; i++) {
        if (c == maplimit->chars[i]) {
            return maplimit->chars + i;
        }
    }
    return ret;
}

int
lnxproc_map_limits_print(LNXPROC_MAP_LIMITS_T maplimits[], int mapdim)
{
    if (!maplimits) {
        return 1;
    }

    int i;

    for (i = 0; i < mapdim; i++) {
        char buf[64];
        char *p = lnxproc_map_limit_print(maplimits + i, buf, sizeof buf);

        printf("Maplimit %d :%s:\n", i, p);
    }
    return LNXPROC_OK;
}

char *
lnxproc_map_limit_print(LNXPROC_MAP_LIMITS_T * maplimit, char *buf,
                        size_t buflen)
{
    buf[0] = '\0';
    if (!maplimit) {
        return buf;
    }

    int m = 0;
    char *b = buf;

    m = snprintf(b, buflen, "E%zd ", maplimit->expected);
    b += m;
    buflen -= m;

    const char *s = maplimit->chars;
    int n = maplimit->len;

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

LNXPROC_MAP_LIMITS_T *
lnxproc_map_limits_dup(LNXPROC_ERROR_CALLBACK callback,
                       LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim)
{
    LNXPROC_MAP_LIMITS_T *newmaplimits = NULL;

    if (mapdim > 0 && maplimits) {
        LNXPROC_DEBUG("Malloc maplimits %zd\n", mapdim);
        newmaplimits = malloc(mapdim * sizeof(LNXPROC_MAP_LIMITS_T));
        if (!newmaplimits) {
            LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_MALLOC_MAPLIMITS);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC_MAPLIMITS,
                                "Malloc maplimits\n");
            return newmaplimits;
        }
        LNXPROC_DEBUG("Malloc maplimits %p\n", newmaplimits);
        int i;

        for (i = 0; i < mapdim; i++) {

#ifdef DEBUG
            char buf[64];
            char *p = lnxproc_map_limit_print(maplimits + i, buf, sizeof buf);

            LNXPROC_DEBUG("Malloc old maplimit %zd :%s:\n", i, p);
#endif
            newmaplimits[i].chars = strdup(maplimits[i].chars);
            if (!newmaplimits[i].chars) {
                LNXPROC_SET_ERROR(callback,
                                  LNXPROC_ERROR_MALLOC_MAPLIMITS_ENTRY);
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC_MAPLIMITS_ENTRY,
                                    "Malloc maplimits entry\n");
                int j;

                for (j = i - 1; j >= 0; j--) {
                    free(newmaplimits[j].chars);
                }
                return NULL;
            }
            newmaplimits[i].expected = maplimits[i].expected;
            newmaplimits[i].len = maplimits[i].len;

#ifdef DEBUG
            p = lnxproc_map_limit_print(newmaplimits + i, buf, sizeof buf);
            LNXPROC_DEBUG("Malloc new maplimit %zd :%s:\n", i, p);
#endif

        }
    }
    return newmaplimits;
}

LNXPROC_MAP_LIMITS_T *
lnxproc_map_limits_free(LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim)
{
    if (mapdim > 0 && maplimits) {
        LNXPROC_DEBUG("Free Maplimits buffer %zd\n", mapdim);
        int i;

        for (i = 0; i < mapdim; i++) {
            LNXPROC_DEBUG("Free Maplimit buffer %d\n", i);
            free(maplimits[i].chars);
            maplimits[i].chars = NULL;
        }
        LNXPROC_DEBUG("Free Maplimits buffer %p\n", maplimits);
        free(maplimits);
    }
    return NULL;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
