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

#ifndef TOPIARY_INTERFACE_PRIVATE_H
#define TOPIARY_INTERFACE_PRIVATE_H 1

#include "base_private.h"
#include <topiary/opt.h>
#include <topiary/interface.h>

typedef int (*_TOPIARY_METHOD) (_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);

struct _topiary_module_row_t {
    const char *tag;
    TOPIARY_MODULE_TYPE_T type;
    _TOPIARY_METHOD new;
    _TOPIARY_BASE_T *base;
    TOPIARY_OPT_T *optional;
};
typedef struct _topiary_module_row_t _TOPIARY_MODULE_ROW_T;

struct topiary_module_t {
    size_t nmodules;
    _TOPIARY_MODULE_ROW_T row[];
};

int _topiary_create(TOPIARY_MODULE_T *modules);
int _topiary_set_fileglob(TOPIARY_MODULE_T *modules, char *fileglob);

#endif                          // TOPIARY_INTERFACE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
