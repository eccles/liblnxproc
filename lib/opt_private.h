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
