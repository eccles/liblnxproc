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

#ifndef LIBLNXPROC_LIMITS_CHR_H
#define LIBLNXPROC_LIMITS_CHR_H 1

#include "limits_private.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * replaces
 * char *ret = strchr(limit->chars, c); 
 *
 * because we want to include the trailing NUL char if required
 */

    static inline char *limit_chr(_LNXPROC_LIMITS_ROW_T * limit, char c) {
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

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_LIMITS_CHR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
