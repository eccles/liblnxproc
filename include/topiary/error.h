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

#ifndef TOPIARY_ERROR_H
#define TOPIARY_ERROR_H 1

#ifdef __cplusplus
extern "C" {
#endif

    enum topiary_error_t {
        TOPIARY_OK = 0,
        TOPIARY_ERROR_MISMATCHED_STRINGS,
        TOPIARY_ERROR_MALLOC,
        TOPIARY_ERROR_ILLEGAL_ARG,
        TOPIARY_ERROR_BASE_READ_OVERFLOW,
        TOPIARY_ERROR_BASE_REGEX_FAILURE,
        TOPIARY_ERROR_BASE_GLOB_FAILURE,
        TOPIARY_ERROR_NOT_FOUND,
        TOPIARY_ERROR_BASE_READ_SSIZE_MAX,
        TOPIARY_ERROR_SIZE
    };

    const char *topiary_strerror(int err, char *buf, size_t buflen);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_ERROR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
