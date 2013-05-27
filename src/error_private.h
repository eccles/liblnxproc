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

#ifndef LIBLNXPROC_ERROR_PRIVATE_H
#define LIBLNXPROC_ERROR_PRIVATE_H 1

#include "util_private.h"
#include <lnxproc/error.h>

int _lnxproc_error_check(void);

#define _LNXPROC_ERROR_CHECK() _lnxproc_error_check()

#ifdef DEBUG

#include <stdio.h>

void
_lnxproc_debug(const char *filename,
               int lineno, const char *funcname, char *fmt, ...
    )
WARN_FORMAT(4, 5);

#define _LNXPROC_DEBUG(fmt, args...) \
                          _lnxproc_debug(__FILE__,__LINE__,__func__, fmt, ##args)

#define _LNXPROC_ERROR_DEBUG(s, fmt, args...) do {\
    char errdbgbuf[32];\
    const char *c = lnxproc_strerror(s,errdbgbuf,sizeof errdbgbuf);\
    char errdbgbuf1[64];\
    snprintf(errdbgbuf1,sizeof errdbgbuf1,"Error : %d %s",s,c);\
    char errdbgbuf2[512];\
    snprintf(errdbgbuf2,sizeof errdbgbuf2,fmt, ##args);\
    _lnxproc_debug(__FILE__,__LINE__,__func__, "%s: %s\n", errdbgbuf1, errdbgbuf2);\
} while(0)

#else
#define _LNXPROC_DEBUG(fmt, args...)
#define _LNXPROC_ERROR_DEBUG(s, fmt, args...)
#endif

#endif                          // LIBLNXPROC_ERROR_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
