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

#include <string.h>             //strdup

#include "allocate.h"
#include "reference.h"
#include "error_private.h"
#include "opt_private.h"
#include "interface_private.h"

int
lnxproc_opt_new(LNXPROC_OPT_T **optptr)
{
    if (!optptr) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option address");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    LNXPROC_OPT_T *p = Acquire(*optptr, sizeof(LNXPROC_OPT_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Module");
        return LNXPROC_ERROR_MALLOC;
    }
    *optptr = p;
    return LNXPROC_OK;
}

static void
lnxproc_opt_release(void *arg)
{
    if (arg) {
        LNXPROC_OPT_T *opt = arg;

        DESTROY(opt->fileglob);
        _LNXPROC_RESULTS_FREE(opt->results);
        DESTROY(opt->master);
        LNXPROC_FREE(opt->module);
    }
}

int
lnxproc_opt_free(LNXPROC_OPT_T **optptr)
{
    if (optptr) {
        RELEASE(*optptr, lnxproc_opt_release);
    }
    return LNXPROC_OK;
}

int
lnxproc_opt_set_fileglob(LNXPROC_OPT_T *opt, char *fileglob)
{
    if (!opt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    DESTROY(opt->fileglob);
    if (fileglob)
        opt->fileglob = strdup(fileglob);
    return LNXPROC_OK;
}

int
lnxproc_opt_set_master(LNXPROC_OPT_T *opt, char *master)
{
    if (!opt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    DESTROY(opt->master);
    if (master)
        opt->master = strdup(master);
    return LNXPROC_OK;
}

int
lnxproc_opt_set_module(LNXPROC_OPT_T *opt, LNXPROC_MODULE_T *module)
{
    if (!opt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    LNXPROC_FREE(opt->module);
    if (module) {
        lnxproc_new(&module, module->nmodules);
        opt->module = module;
    }
    return LNXPROC_OK;
}

int
lnxproc_opt_set_pgrp(LNXPROC_OPT_T *opt, int pgrp)
{
    if (!opt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    opt->pgrp = pgrp;
    return LNXPROC_OK;
}

int
_lnxproc_opt_set_results(LNXPROC_OPT_T *opt, _LNXPROC_RESULTS_T *results)
{
    if (!opt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Option");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    opt->results = Acquire(results, 0);
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
