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

#ifndef TOPIARY_PRINT_H
#define TOPIARY_PRINT_H 1

#ifdef __cplusplus
extern "C" {
#endif

    enum topiary_print_t {
        TOPIARY_PRINT_ALL = 0,
        TOPIARY_PRINT_VALUES,
        TOPIARY_PRINT_JSON,
        TOPIARY_PRINT_SIZE      // must be last
    };

    typedef enum topiary_print_t TOPIARY_PRINT_T;

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_PRINT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
