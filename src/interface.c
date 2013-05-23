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

#include <stdlib.h>
#include <string.h>
#include "error_private.h"
#include "base_private.h"
#include "interface_private.h"
#include "proc_cgroups.h"
#include "proc_diskstats.h"
#include "proc_domainname.h"
#include "proc_hostname.h"
#include "proc_osrelease.h"
#include "sys_cpufreq.h"
#include "sys_disksectors.h"
#include "proc_pid_stat.h"

static _LNXPROC_MODULE_ROW_T mymodules[] = {
    {.new = _lnxproc_proc_cgroups_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_diskstats_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_domainname_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_hostname_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_osrelease_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_sys_cpufreq_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_sys_disksectors_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_pid_stat_new,.base = NULL,.optional = NULL,},
};

static size_t nmodules = sizeof(mymodules) / sizeof(mymodules[0]);

int
lnxproc_new(LNXPROC_MODULE_T ** moduleptr, size_t nmodule)
{
    if (!moduleptr) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Module address");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (nmodules < 0) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Module: nmodules < 0");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    LNXPROC_MODULE_T *p;

    if (nmodule == 0) {
        p = calloc(1,
                   sizeof(LNXPROC_MODULE_T) +
                   (nmodules * sizeof(_LNXPROC_MODULE_ROW_T)));
    }
    else {
        p = calloc(1,
                   sizeof(LNXPROC_MODULE_T) +
                   (nmodule * sizeof(_LNXPROC_MODULE_ROW_T)));
    }

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Module");
        return LNXPROC_ERROR_MALLOC;
    }
    if (nmodule == 0) {
        p->nmodules = nmodules;
        memcpy(p->row, mymodules, nmodules * sizeof(_LNXPROC_MODULE_ROW_T));
    }
    else {
        p->nmodules = nmodule;
    }
    *moduleptr = p;
    return LNXPROC_OK;
}

int
lnxproc_set(LNXPROC_MODULE_T * module, size_t pos, LNXPROC_MODULE_TYPE_T type,
            void *optional, size_t optlen)
{
    if (!module) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Module");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (pos < 0 || pos >= module->nmodules) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Pos = %zd", pos);
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    _LNXPROC_MODULE_ROW_T *row = module->row + pos;

    void *p = NULL;

    if (optional && optlen > 0) {
        p = memdup(optional, optlen);

        if (!p) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Module optional at %zd",
                                 pos);
            return LNXPROC_ERROR_MALLOC;
        }
    }
    if (row->optional)
        free(row->optional);

    memcpy(row, mymodules + type - 1, sizeof(_LNXPROC_MODULE_ROW_T));

    row->optional = p;
    return LNXPROC_OK;
}

int
lnxproc_free(LNXPROC_MODULE_T ** modulesptr)
{
    if (modulesptr && *modulesptr) {
        LNXPROC_MODULE_T *modules = *modulesptr;

        int i;

        for (i = 0; i < modules->nmodules; i++) {
            _LNXPROC_MODULE_ROW_T *row = modules->row + i;

            _LNXPROC_BASE_FREE(row->base);
            if (row->optional)
                free(row->optional);
        }
        free(modules);
        *modulesptr = NULL;
    }
    return LNXPROC_OK;
}

int
lnxproc_read(LNXPROC_MODULE_T * modules)
{

    if (!modules) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Modules");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    int i;
    int ret;

    for (i = 0; i < modules->nmodules; i++) {
        _LNXPROC_MODULE_ROW_T *row = modules->row + i;

        if (!row->base) {
            ret = row->new(&row->base, row->optional);

            if (ret) {
                return ret;
            }
        }
        _LNXPROC_BASE_T *base = row->base;

        ret = _lnxproc_base_rawread(base);
        if (ret) {
            return ret;
        }
        ret = _lnxproc_base_normalize(base);
        if (ret) {
            return ret;
        }
    }
    return LNXPROC_OK;
}

int
lnxproc_performance(LNXPROC_MODULE_T * modules,
                    long *rawread_time, long *map_time, long *hash_time,
                    long *normalize_time)
{

    *rawread_time = 0;
    *map_time = 0;
    *hash_time = 0;
    *normalize_time = 0;

    if (!modules) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Modules");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    int i;

    for (i = 0; i < modules->nmodules; i++) {
        _LNXPROC_MODULE_ROW_T *row = modules->row + i;
        _LNXPROC_BASE_T *base = row->base;

        if (base) {
            _LNXPROC_BASE_DATA_T *base_data = base->current;

            if (base_data) {
                *rawread_time += base_data->rawread_time;
                *map_time += base_data->map_time;
                *hash_time += base_data->hash_time;
                *normalize_time += base_data->normalize_time;
            }
        }
    }
    return LNXPROC_OK;
}

int
lnxproc_print(LNXPROC_MODULE_T * modules)
{
    if (!modules) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Modules");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    int i;

    for (i = 0; i < modules->nmodules; i++) {
        _LNXPROC_MODULE_ROW_T *row = modules->row + i;
        _LNXPROC_BASE_T *base = row->base;

        if (base) {
            _LNXPROC_BASE_DATA_T *base_data = base->current;

            if (base_data) {
                _lnxproc_results_print(base_data->results);
            }
        }
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
