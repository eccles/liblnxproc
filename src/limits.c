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

#include "config.h"

#include <string.h>             //strdup

#include "allocate.h"
#include "reference.h"
#include "strlcpy.h"
#include "val2str.h"
#include "util_private.h"
#include "error_private.h"
#include "limits_private.h"

int
_topiary_limits_print(_TOPIARY_LIMITS_T *limits)
{
    if (!limits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    printf("Size %zd\n", limits->size);
    printf("Dim %zd\n", limits->dim);
    int i;

    for (i = 0; i < limits->dim; i++) {
        char buf[64];

        _topiary_limit_print(limits->row + i, buf, sizeof buf);

        printf("Limit %d :%s:\n", i, buf);
    }
    return TOPIARY_OK;
}

int
_topiary_chars_print(char *chars, size_t nchars, char *buf, size_t buflen)
{

    static const char *cstr[] = {
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
        return TOPIARY_OK;
    }

    int m = 0;
    int len = buflen - 1;

    for (i = 0; i < nchars && len > 1; i++) {
        c = chars[i];
        if (c < 33) {
            STRLCAT(buf, cstr[(int) c], m, len);
        }
        else if (c == 127) {
            STRLCAT(buf, "DEL", m, len);
        }
        else if (c > 127) {
            m = int2str(c, buf + m, len - m);
        }
        else {
            buf[m] = c;
            len--;
        }
        buf[m] = ' ';
        len--;
    }
    buf[m] = '\0';

    return TOPIARY_OK;
}

int
_topiary_limit_print(_TOPIARY_LIMITS_ROW_T *row, char *buf, size_t buflen)
{
    buf[0] = '\0';

    if (!row) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits row");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int m = 0;
    char *b = buf;
    int len = buflen - 1;

    m = snprintf(b, len, "Exp %zd: Len %d: ", row->expected, row->len);
    b += m;
    len -= m;

    return _topiary_chars_print(row->chars, row->len, b, len);
}

int
_topiary_limits_new(_TOPIARY_LIMITS_T **newlimits, size_t dim)
{
    if (!newlimits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits address");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (*newlimits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits is not null");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    size_t size =
        sizeof(_TOPIARY_LIMITS_T) + (dim * sizeof(_TOPIARY_LIMITS_ROW_T));
    _TOPIARY_DEBUG("Malloc limits %zd\n", dim);
    _TOPIARY_LIMITS_T *nlimits = Acquire(NULL, size);

    if (!nlimits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Limits");
        return TOPIARY_ERROR_MALLOC;
    }
    nlimits->dim = dim;
    *newlimits = nlimits;
    return TOPIARY_OK;
}

int
_topiary_limits_size(_TOPIARY_LIMITS_T *limits, size_t * size)
{
    if (!size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Size");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!limits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size =
        sizeof(_TOPIARY_LIMITS_T) +
        (limits->dim * sizeof(_TOPIARY_LIMITS_ROW_T));
    int i;

    for (i = 0; i < limits->dim; i++) {
        _TOPIARY_LIMITS_ROW_T *row = limits->row + i;

        if (row->chars)
            *size += strlen(row->chars) + 1;
    }
    return TOPIARY_OK;
}

int
_topiary_limits_set(_TOPIARY_LIMITS_T *limits, int pos, size_t expected,
                    char *chars, int len)
{
    if (!limits) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!chars) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Chars is null");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (len < 1) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Len = %d", len);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (pos < 0 || pos >= limits->dim) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Limits pos = %d", pos);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    char *c = strdup(chars);

    if (!c) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Limits pos = %d", pos);
        return TOPIARY_ERROR_MALLOC;
    }

    _TOPIARY_LIMITS_ROW_T *row = limits->row + pos;

    row->expected = expected;
    DESTROY(row->chars);
    row->chars = c;
    row->len = len;
    return TOPIARY_OK;
}

static void
limits_rows_free(void *ptr)
{
    if (ptr) {
        _TOPIARY_LIMITS_T *mylimits = ptr;

        _TOPIARY_DEBUG("Free Limits buffer %zd\n", mylimits->dim);

        int i;

        for (i = 0; i < mylimits->dim; i++) {
            _TOPIARY_DEBUG("Free Limit buffer %d\n", i);
            _TOPIARY_LIMITS_ROW_T *row = mylimits->row + i;

            DESTROY(row->chars);
        }
    }
}

int
_topiary_limits_free(_TOPIARY_LIMITS_T **limits)
{
    if (limits) {
        _TOPIARY_DEBUG("Free Limits buffer %p\n", *limits);
        RELEASE(*limits, limits_rows_free);
    }
    return TOPIARY_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
