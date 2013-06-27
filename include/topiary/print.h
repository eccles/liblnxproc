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
