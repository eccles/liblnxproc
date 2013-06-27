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
