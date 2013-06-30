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

#ifndef TOPIARY_OPT_PRIVATE_H
#define TOPIARY_OPT_PRIVATE_H 1

#include <topiary/opt.h>
#include "results_private.h"

struct topiary_opt_t {
    char *fileglob;
    _TOPIARY_RESULTS_T *results;
    char *master;
    int pgrp;
    TOPIARY_MODULE_T *module;
};

int _topiary_opt_set_results(TOPIARY_OPT_T *opt, _TOPIARY_RESULTS_T *results);

#endif                          // TOPIARY_OPT_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
