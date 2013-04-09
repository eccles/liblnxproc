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

#ifndef LIBLNXPROC_MAP_LIMITS_H
#define LIBLNXPROC_MAP_LIMITS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"

    struct lnxproc_map_limits_t {
        size_t expected;
        char *chars;
        int len;
    };
    typedef struct lnxproc_map_limits_t LNXPROC_MAP_LIMITS_T;

    char *lnxproc_map_chr(LNXPROC_MAP_LIMITS_T * maplimit, char c);

    char *lnxproc_map_limit_print(LNXPROC_MAP_LIMITS_T * maplimit, char *buf,
                                  size_t buflen);
    int lnxproc_map_limits_print(LNXPROC_MAP_LIMITS_T maplimits[], int mapdim);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_MAP_LIMITS_T *lnxproc_map_limits_dup(LNXPROC_ERROR_CALLBACK
                                                 callback,
                                                 LNXPROC_MAP_LIMITS_T
                                                 maplimits[],
                                                 size_t mapdim) WARN_UNUSED;
    LNXPROC_MAP_LIMITS_T *lnxproc_map_limits_free(LNXPROC_MAP_LIMITS_T
                                                  maplimits[],
                                                  size_t mapdim) WARN_UNUSED;

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_MAP_LIMITS_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
