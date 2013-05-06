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

#include "error_private.h"
#include "limits_private.h"

char *
_lnxproc_limit_chr(_LNXPROC_LIMITS_T * limit, char c)
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
_lnxproc_limits_print(_LNXPROC_LIMITS_T limits[], int dim)
{
    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    int i;

    for (i = 0; i < dim; i++) {
        char buf[64];

        _lnxproc_limit_print(limits + i, buf, sizeof buf);

        printf("Limit %d :%s:\n", i, buf);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_chars_print(char *chars, size_t nchars, char *buf, size_t buflen)
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

    if (nchars == 0) {
        buf[0] = '\0';
        return LNXPROC_OK;
    }

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
_lnxproc_limit_print(_LNXPROC_LIMITS_T * limit, char *buf, size_t buflen)
{
    buf[0] = '\0';

    if (!limit) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMIT_NULL;
    }

    int m = 0;
    char *b = buf;
    int len = buflen - 1;

    m = snprintf(b, len, "Exp %zd: Len %d: ", limit->expected, limit->len);
    b += m;
    len -= m;

    return _lnxproc_chars_print(limit->chars, limit->len, b, len);
}

LNXPROC_ERROR_T
_lnxproc_limits_dup(_LNXPROC_LIMITS_T ** newlimits,
                    _LNXPROC_LIMITS_T limits[], size_t dim)
{
    _LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_LIMITS_T %d\n", sizeof(_LNXPROC_LIMITS_T));

    if (!newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_NULL;
    }
    if (*newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (dim < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION, "\n");
        return LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION;
    }
    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    _LNXPROC_DEBUG("Malloc limits %zd\n", dim);
    _LNXPROC_LIMITS_T *nlimits = calloc(dim, sizeof(_LNXPROC_LIMITS_T));

    if (!nlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_MALLOC, "Malloc limits\n");
        return LNXPROC_ERROR_LIMITS_MALLOC;
    }

    _LNXPROC_DEBUG("Malloc limits %p\n", nlimits);
    int i;

    for (i = 0; i < dim; i++) {

#ifdef DEBUG
        char buf[64];

        _lnxproc_limit_print(limits + i, buf, sizeof buf);

        _LNXPROC_DEBUG("Malloc old limit %zd :%s:\n", i, buf);
#endif
        nlimits[i].chars = malloc(limits[i].len * sizeof(char));
        if (!nlimits[i].chars) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_MALLOC_ENTRY,
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
        _lnxproc_limit_print(nlimits + i, buf, sizeof buf);
        _LNXPROC_DEBUG("Malloc new limit %zd :%s:\n", i, buf);
#endif
    }
    *newlimits = nlimits;
    return LNXPROC_OK;
}

_LNXPROC_LIMITS_T *
_lnxproc_limits_free(_LNXPROC_LIMITS_T limits[], size_t dim)
{
    if (dim > 0 && limits) {
        _LNXPROC_DEBUG("Free Limits buffer %zd\n", dim);

        int i;

        for (i = 0; i < dim; i++) {
            _LNXPROC_DEBUG("Free Limit buffer %d\n", i);
            free(limits[i].chars);
            limits[i].chars = NULL;
        }

        _LNXPROC_DEBUG("Free Limits buffer %p\n", limits);
        free(limits);
    }
    return NULL;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
