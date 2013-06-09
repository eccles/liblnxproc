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

#ifndef LIBLNXPROC_PRINT_H
#define LIBLNXPROC_PRINT_H 1

#ifdef __cplusplus
extern "C" {
#endif

    enum lnxproc_print_t {
        LNXPROC_PRINT_ALL = 0,
        LNXPROC_PRINT_VALUES,
        LNXPROC_PRINT_JSON,
        LNXPROC_PRINT_SIZE      // must be last
    };

    typedef enum lnxproc_print_t LNXPROC_PRINT_T;

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_PRINT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
