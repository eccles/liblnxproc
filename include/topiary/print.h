#ifndef TOPIARY_PRINT_H
#define TOPIARY_PRINT_H 1

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup print The topiary print API.
 *
 * This module controls output of the various print functions in topiary.
 *
 * @section topiary_print Print control.
 *
 * @{
 */

/**
 * @brief Print selection.
 *
 * TOPIARY_PRINT_ALL Print all data.
 *
 * TOPIARY_PRINT_VALUES Print only values in key,value form.
 *
 * TOPIARY_PRINT_JSON Print only values in JSON form.
 *
 */
    enum topiary_print_t {
        TOPIARY_PRINT_ALL = 0,
        TOPIARY_PRINT_VALUES,
        TOPIARY_PRINT_JSON,
        TOPIARY_PRINT_SIZE      // must be last
    };

/** A convenient typedef for the TOPIARY_PRINT_T object */
    typedef enum topiary_print_t TOPIARY_PRINT_T;

/** @} ******************************************************************/

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_PRINT_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
