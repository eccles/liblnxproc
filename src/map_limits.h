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

    struct lnxproc_map_limits_t {
        char *chars;
        int len;
    };
    typedef struct lnxproc_map_limits_t LNXPROC_MAP_LIMITS_T;

    char *lnxproc_map_chr(LNXPROC_MAP_LIMITS_T * pat, char c);

    char *lnxproc_print_map_limit(LNXPROC_MAP_LIMITS_T * maplimit,
                                  char *buf, size_t buflen);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_MAP_LIMITS_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
