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

#include "map_limits.h"

char *
lnxproc_map_chr(LNXPROC_MAP_LIMITS_T * pat, char c)
{
    char *ret = NULL;

//    ret = strchr(pat->chars, c);
    int i;

    for (i = 0; i < pat->len; i++) {
        if (c == pat->chars[i]) {
            return pat->chars + i;
        }
    }
    return ret;
}

char *
lnxproc_print_map_limit(LNXPROC_MAP_LIMITS_T * maplimit, char *buf,
                        size_t buflen)
{
    buf[0] = '\0';
    if (!maplimit) {
        return buf;
    }

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
    int m = 0;
    char *b = buf;

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

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
