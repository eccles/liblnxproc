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

#ifdef DEBUG
#include <stdarg.h>
#endif

static const char *errstr[] = {
    "No error",
    "System error",
    "Fail to malloc array header",
    "Fail to malloc array data",
    "Array arg is null",
/*5*/ "Fail to realloc array data",
    "Array index out of range",
    "Array data is null",
    "Array dimension out of range",
    "Base arg is null",
/*10*/ "Fail to malloc base",
    "Fail to malloc base buffer",
    "Fail to malloc base array",
    "Fail to malloc base timestamp",
    "Fail to malloc limits",
    "Base limits is null",
/*16*/ "Fail to malloc limits entry",
    "Fail to malloc db",
    "Db arg is null",
    "Fail to malloc template",
    "Template arg is null",
/*21*/ "Fail to malloc timestamp",
    "Timestamp arg is null",
    "Fail to malloc vector header",
    "Fail to malloc vector children",
    "Fail to malloc vector values",
/*26*/ "Vector arg is null",
    "Fail to realloc vector children",
    "Fail to realloc vector values",
    "Vector index out of range",
    "Vector children is null",
    "Vector values is null",
    "Error enum and string have mismatched sizes",
};

#define ERRSTR_SIZE sizeof(errstr)/sizeof(errstr[0])

static const char unknown_error[] = "Unknown error";

#ifdef DEBUG
void
lnxproc_debug(const char *filename, int lineno, const char *funcname, char *fmt,
              ...)
{
    va_list args;

    printf("Debug: %s[%d] %s -> ", filename, lineno, funcname);

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

}
#endif
const char *
lnxproc_strerror(LNXPROC_ERROR_T err, char *buf, size_t buflen)
{
    if (err < 0) {
        strerror_r(-((int) err), buf, buflen);
        return buf;
    }
    if (err >= ERRSTR_SIZE) {
        return unknown_error;
    }

    return errstr[err];
}

void
lnxproc_set_error(LNXPROC_ERROR_CALLBACK callback,
                  const char *filename,
                  int lineno, const char *funcname, LNXPROC_ERROR_T err)
{
    if (callback) {
        callback(filename, lineno, funcname, err);
    }
}

void
lnxproc_error_print_callback(const char *filename,
                             int lineno,
                             const char *funcname, LNXPROC_ERROR_T err)
{
    if (err >= ERRSTR_SIZE) {
        printf("Error: %s[%d] %s -> %s = %d\n", filename, lineno, funcname,
               unknown_error, err);
    }

    else if (err > 0) {
        printf("Error: %s[%d] %s -> %s\n", filename, lineno, funcname,
               errstr[err]);

    }

    else if (err < 0) {
        char buf[128];

        strerror_r(-((int) err), buf, sizeof buf);
        printf("System Error: %s[%d] %s -> %s\n", filename, lineno, funcname,
               buf);
    }
}

#ifdef DEBUG
int
lnxproc_error_check(void)
{
    if (LNXPROC_ERROR_SIZE != ERRSTR_SIZE) {
        int err = LNXPROC_ERROR_MISMATCHED_STRINGS;

        printf("Error: %s %d != %zd\n", errstr[err], LNXPROC_ERROR_SIZE,
               ERRSTR_SIZE);
        return err;
    }
    return LNXPROC_OK;
}
#endif

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
