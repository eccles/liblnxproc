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

#include <lnxproc/error.h>
#include <lnxproc/limits.h>

char *
lnxproc_limit_chr(LNXPROC_LIMITS_T * limit, char c)
{

/*
 * replaces
 * char *ret = strchr(limit->chars, c); 
 *
 * because we want to include the trailing NUL char if required
 */
    if (limit) {
        int i;

        for (i = 0; i < limit->len; i++) {
            if (c == limit->chars[i]) {
                return limit->chars + i;
            }
        }
    }
    return NULL;
}

LNXPROC_ERROR_T
lnxproc_limits_print(LNXPROC_LIMITS_T limits[], int dim)
{
    if (!limits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    int i;

    for (i = 0; i < dim; i++) {
        char buf[64];

        lnxproc_limit_print(limits + i, buf, sizeof buf);

        printf("Limit %d :%s:\n", i, buf);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_chars_print(char *chars, size_t nchars, char *buf, size_t buflen)
{

    char *cstr[] = {
        "NUL '\\0'", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK",
        "BEL '\\a'", "BS '\\b'", "HT '\\t'", "LF '\\n'", "VT '\\v'", "FF '\\f'",
        "CR '\\r'",
        "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
        "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE",
    };

    int i;
    char c;

    int m = 0;
    char *b = buf;
    int len = buflen - 1;

    for (i = 0; i < nchars && len > 0; i++) {
        c = chars[i];
        if (c < 33) {
            m = snprintf(b, len, "%s ", cstr[(int) c]);
            b += m;
            len -= m;
        }
        else if (c == 127) {
            m = snprintf(b, len, "DEL ");
            b += m;
            len -= m;
        }
        else if (c > 127) {
            m = snprintf(b, len, "%d ", c);
            b += m;
            len -= m;
        }
        else {
            m = snprintf(b, len, "%c ", c);
            b += m;
            len -= m;
        }
    }

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_limit_print(LNXPROC_LIMITS_T * limit, char *buf, size_t buflen)
{
    buf[0] = '\0';

    if (!limit) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMIT_NULL;
    }

    int m = 0;
    char *b = buf;
    int len = buflen - 1;

    m = snprintf(b, len, "Exp %zd: Len %d: ", limit->expected, limit->len);
    b += m;
    len -= m;

    return lnxproc_chars_print(limit->chars, limit->len, b, len);
}

LNXPROC_ERROR_T
lnxproc_limits_dup(LNXPROC_LIMITS_T ** newlimits,
                   LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof LNXPROC_LIMITS_T %d\n", sizeof(LNXPROC_LIMITS_T));

    if (!newlimits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_NULL;
    }
    if (*newlimits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (dim < 1) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION, "\n");
        return LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION;
    }
    if (!limits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    LNXPROC_DEBUG("Malloc limits %zd\n", dim);
    LNXPROC_LIMITS_T *nlimits = calloc(dim, sizeof(LNXPROC_LIMITS_T));

    if (!nlimits) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_MALLOC, "Malloc limits\n");
        return LNXPROC_ERROR_LIMITS_MALLOC;
    }

    LNXPROC_DEBUG("Malloc limits %p\n", nlimits);
    int i;

    for (i = 0; i < dim; i++) {

#ifdef DEBUG
        char buf[64];

        lnxproc_limit_print(limits + i, buf, sizeof buf);

        LNXPROC_DEBUG("Malloc old limit %zd :%s:\n", i, buf);
#endif
        nlimits[i].chars = malloc(limits[i].len * sizeof(char));
        if (!nlimits[i].chars) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_MALLOC_ENTRY,
                                "Malloc limits entry\n");
            int j;

            for (j = i - 1; j >= 0; j--) {
                free(nlimits[j].chars);
            }
            free(nlimits);
            return LNXPROC_ERROR_LIMITS_MALLOC_ENTRY;
        }
        memcpy(nlimits[i].chars, limits[i].chars, limits[i].len);
        nlimits[i].expected = limits[i].expected;
        nlimits[i].len = limits[i].len;

#ifdef DEBUG
        lnxproc_limit_print(nlimits + i, buf, sizeof buf);
        LNXPROC_DEBUG("Malloc new limit %zd :%s:\n", i, buf);
#endif
    }
    *newlimits = nlimits;
    return LNXPROC_OK;
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
