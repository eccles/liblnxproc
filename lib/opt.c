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

#include "config.h"

#include <string.h>             //strdup

#include "allocate.h"
#include "reference.h"
#include "error_private.h"
#include "opt_private.h"
#include "interface_private.h"

int
topiary_opt_new(TOPIARY_OPT_T **optptr)
{
    if (!optptr) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option address");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    TOPIARY_OPT_T *p = Acquire(*optptr, sizeof(TOPIARY_OPT_T));

    if (!p) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Module");
        return TOPIARY_ERROR_MALLOC;
    }
    *optptr = p;
    return TOPIARY_OK;
}

static void
topiary_opt_release(void *arg)
{
    if (arg) {
        TOPIARY_OPT_T *opt = arg;

        DESTROY(opt->fileglob);
        _TOPIARY_RESULTS_FREE(opt->results);
        DESTROY(opt->master);
        TOPIARY_FREE(opt->module);
    }
}

int
topiary_opt_free(TOPIARY_OPT_T **optptr)
{
    if (optptr) {
        RELEASE(*optptr, topiary_opt_release);
    }
    return TOPIARY_OK;
}

int
topiary_opt_set_fileglob(TOPIARY_OPT_T *opt, char *fileglob)
{
    if (!opt) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    DESTROY(opt->fileglob);
    if (fileglob)
        opt->fileglob = strdup(fileglob);
    return TOPIARY_OK;
}

int
topiary_opt_set_master(TOPIARY_OPT_T *opt, char *master)
{
    if (!opt) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    DESTROY(opt->master);
    if (master)
        opt->master = strdup(master);
    return TOPIARY_OK;
}

int
topiary_opt_set_module(TOPIARY_OPT_T *opt, TOPIARY_MODULE_T *module)
{
    if (!opt) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    TOPIARY_FREE(opt->module);
    if (module) {
        topiary_new(&module, module->nmodules);
        opt->module = module;
    }
    return TOPIARY_OK;
}

int
topiary_opt_set_pgrp(TOPIARY_OPT_T *opt, int pgrp)
{
    if (!opt) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    opt->pgrp = pgrp;
    return TOPIARY_OK;
}

int
_topiary_opt_set_results(TOPIARY_OPT_T *opt, _TOPIARY_RESULTS_T *results)
{
    if (!opt) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Option");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    opt->results = Acquire(results, 0);
    return TOPIARY_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
