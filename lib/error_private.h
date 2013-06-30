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

#ifndef TOPIARY_ERROR_PRIVATE_H
#define TOPIARY_ERROR_PRIVATE_H 1

#include "util_private.h"
#include <topiary/error.h>

int _topiary_error_check(void);

#define _TOPIARY_ERROR_CHECK() _topiary_error_check()

#ifdef DEBUG

#include <stdio.h>

void
_topiary_debug(const char *filename,
               int lineno, const char *funcname, char *fmt, ...
    )
WARN_FORMAT(4, 5);

#define _TOPIARY_DEBUG(fmt, args...) \
                          _topiary_debug(__FILE__,__LINE__,__func__, fmt, ##args)

#define _TOPIARY_ERROR_DEBUG(s, fmt, args...) do {\
    char errdbgbuf[32];\
    const char *c = topiary_strerror(s,errdbgbuf,sizeof errdbgbuf);\
    char errdbgbuf1[64];\
    snprintf(errdbgbuf1,sizeof errdbgbuf1,"Error : %d %s",s,c);\
    char errdbgbuf2[512];\
    snprintf(errdbgbuf2,sizeof errdbgbuf2,fmt, ##args);\
    _topiary_debug(__FILE__,__LINE__,__func__, "%s: %s\n", errdbgbuf1, errdbgbuf2);\
} while(0)

#else
#define _TOPIARY_DEBUG(fmt, args...)
#define _TOPIARY_ERROR_DEBUG(s, fmt, args...)
#endif

#endif                          // TOPIARY_ERROR_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
