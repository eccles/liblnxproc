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

LNXPROC_ERROR_T
_lnxproc_limits_print(_LNXPROC_LIMITS_T * limits)
{
    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    printf("Dim %zd\n", limits->dim);
    int i;

    for (i = 0; i < limits->dim; i++) {
        char buf[64];

        _lnxproc_limit_print(limits->row + i, buf, sizeof buf);

        printf("Limit %d :%s:\n", i, buf);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_chars_print(char *chars, size_t nchars, char *buf, size_t buflen)
{

    const char *cstr[] = {
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

    for (i = 0; i < nchars && len > 1; i++) {
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
_lnxproc_limit_print(_LNXPROC_LIMITS_ROW_T * row, char *buf, size_t buflen)
{
    buf[0] = '\0';

    if (!row) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMIT_NULL;
    }

    int m = 0;
    char *b = buf;
    int len = buflen - 1;

    m = snprintf(b, len, "Exp %zd: Len %d: ", row->expected, row->len);
    b += m;
    len -= m;

    return _lnxproc_chars_print(row->chars, row->len, b, len);
}

LNXPROC_ERROR_T
_lnxproc_limits_new(_LNXPROC_LIMITS_T ** newlimits, size_t dim)
{
    if (!newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_NULL;
    }
    if (*newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL;
    }
    _LNXPROC_DEBUG("Malloc limits %zd\n", dim);
    _LNXPROC_LIMITS_T *nlimits = calloc(1,
                                        sizeof(_LNXPROC_LIMITS_T) +
                                        (dim * sizeof(_LNXPROC_LIMITS_ROW_T)));
    if (!nlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_MALLOC, "Malloc limits\n");
        return LNXPROC_ERROR_LIMITS_MALLOC;
    }
    nlimits->dim = dim;
    *newlimits = nlimits;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_limits_set(_LNXPROC_LIMITS_T * limits, int pos, size_t expected,
                    char *chars, int len)
{
    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }
    if (pos < 0 || pos >= limits->dim) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION,
                             "pos = %d\n", pos);
        return LNXPROC_ERROR_LIMITS_ILLEGAL_DIMENSION;
    }

    char *c = strdup(chars);

    if (!c) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_CHARS_MALLOC,
                             "Malloc chars\n");
        return LNXPROC_ERROR_LIMITS_CHARS_MALLOC;
    }

    _LNXPROC_LIMITS_ROW_T *row = limits->row + pos;

    row->expected = expected;
    if (row->chars)
        free(row->chars);
    row->chars = c;
    row->len = len;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_limits_dup(_LNXPROC_LIMITS_T ** newlimits, _LNXPROC_LIMITS_T * limits)
{
    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_LIMITS_T %lu\n", sizeof(_LNXPROC_LIMITS_T));

    if (!newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_NULL;
    }
    if (*newlimits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_LIMITS_ADDRESS_CONTENTS_NOT_NULL;
    }
    if (!limits) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_LIMITS_NULL, "\n");
        return LNXPROC_ERROR_LIMITS_NULL;
    }

    LNXPROC_ERROR_T ret = _lnxproc_limits_new(newlimits, limits->dim);

    if (ret) {
        return ret;
    }

    _LNXPROC_DEBUG("Malloc limits %p\n", *newlimits);
    int i;

    for (i = 0; i < limits->dim; i++) {
        _LNXPROC_LIMITS_ROW_T *row = limits->row + i;

        _lnxproc_limits_set(*newlimits, i, row->expected, row->chars, row->len);
#ifdef DEBUG
        char buf[64];

        _lnxproc_limit_print(row, buf, sizeof buf);
#endif
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_limits_free(_LNXPROC_LIMITS_T ** limits)
{
    if (limits && *limits) {
        _LNXPROC_LIMITS_T *mylimits = *limits;

        _LNXPROC_DEBUG("Free Limits buffer %zd\n", mylimits->dim);

        int i;

        for (i = 0; i < mylimits->dim; i++) {
            _LNXPROC_DEBUG("Free Limit buffer %d\n", i);
            _LNXPROC_LIMITS_ROW_T *row = mylimits->row + i;

            if (row->chars) {
                free(row->chars);
                row->chars = NULL;
            }
        }

        _LNXPROC_DEBUG("Free Limits buffer %p\n", mylimits);
        free(mylimits);
        *limits = NULL;
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
