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

#ifndef LIBLNXPROC_INTERFACE_PRIVATE_H
#define LIBLNXPROC_INTERFACE_PRIVATE_H 1

#include "base_private.h"
#include <lnxproc/opt.h>
#include <lnxproc/interface.h>

typedef int (*_LNXPROC_METHOD) (_LNXPROC_BASE_T **base,
                                LNXPROC_OPT_T *optional);

struct _lnxproc_module_row_t {
    const char *tag;
    LNXPROC_MODULE_TYPE_T type;
    _LNXPROC_METHOD new;
    _LNXPROC_BASE_T *base;
    LNXPROC_OPT_T *optional;
};
typedef struct _lnxproc_module_row_t _LNXPROC_MODULE_ROW_T;

struct lnxproc_module_t {
    size_t nmodules;
    _LNXPROC_MODULE_ROW_T row[];
};

int _lnxproc_create(LNXPROC_MODULE_T *modules);
int _lnxproc_set_fileglob(LNXPROC_MODULE_T *modules, char *fileglob);

#endif                          // LIBLNXPROC_INTERFACE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
