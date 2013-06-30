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
