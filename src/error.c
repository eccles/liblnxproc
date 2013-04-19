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

#ifdef DEBUG
#include <stdarg.h>
#endif

static const char *errstr[] = {
    "No error",
    "Error enum and string have mismatched sizes",
    "System error",
    "Fail to malloc array header",
    "Fail to malloc array data",
    "Array arg is null",
/*5*/ "Fail to realloc array data",
    "Array index out of range",
    "Array vector is null",
    "Fail to malloc array vector",
    "Fail to malloc array saved",
    "Array saved is null",
    "Array address argument is null",
    "Array address contents is not null",
    "Array limits is null",
    "Array dimension out of range",
    "Base arg is null",
    "Base - bad filename specified",
/*13*/ "Fail to malloc base",
    "Fail to malloc base buffer",
    "Fail to malloc base filename",
    "Fail to malloc base file prefix",
    "Fail to malloc base file suffix",
    "Fail to malloc base array",
    "Fail to malloc base results",
    "Overflow of raw read buffer",
    "Fail to malloc limits",
    "Limits argument is null",
    "Limit argument is null",
/*24*/ "Fail to malloc limits entry",
    "Limits address argument is null",
    "Limits address contents is not null",
    "Limits dimension is less than 1",
    "Db arg is null",
    "Db database is not open",
/*30*/ "Fail to open database",
    "Fail to close database",
    "Fail to fetch from database",
    "Fail to store in database",
    "Fail to iterate database",
    "Results arg is null",
    "Results buf is null",
    "Results buflength is less than 1",
    "Results timeval address is null",
/*39*/ "Results address is null",
    "Results address contents is not null",
    "Fail to malloc results",
    "Results key is null",
    "Results keylength is less than 1",
    "Results value address is null",
    "Fail to malloc vector header",
    "Fail to malloc vector data",
/*47*/ "Vector arg is null",
    "Fail to realloc vector data",
    "Vector index out of range",
    "Vector data is null",
    "Vector children is null",
    "Vector values is null",
/*53*/ "Vector address is null",
    "Vector address contents is not null",
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

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
