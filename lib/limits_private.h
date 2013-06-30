/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef TOPIARY_LIMITS_PRIVATE_H
#define TOPIARY_LIMITS_PRIVATE_H 1

struct _topiary_limits_row_t {
    size_t expected;
    char *chars;
    int len;
};
typedef struct _topiary_limits_row_t _TOPIARY_LIMITS_ROW_T;

struct _topiary_limits_t {
    size_t size;
    size_t dim;
    _TOPIARY_LIMITS_ROW_T row[];
};
typedef struct _topiary_limits_t _TOPIARY_LIMITS_T;

int _topiary_limits_new(_TOPIARY_LIMITS_T **newlimits, size_t dim);
int _topiary_limits_size(_TOPIARY_LIMITS_T *limits, size_t * size);
int _topiary_limits_set(_TOPIARY_LIMITS_T *limits, int pos,
                        size_t expected, char *chars, int len);

int _topiary_limits_free(_TOPIARY_LIMITS_T **limits);

#define _TOPIARY_LIMITS_FREE(ll) _topiary_limits_free(&ll)

int _topiary_chars_print(char *chars, size_t nchars, char *buf, size_t buflen);

int _topiary_limit_print(_TOPIARY_LIMITS_ROW_T *limit,
                         char *buf, size_t buflen);
int _topiary_limits_print(_TOPIARY_LIMITS_T *limits);

#endif                          // TOPIARY_LIMITS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
