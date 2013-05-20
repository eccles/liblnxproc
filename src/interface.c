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

static LNXPROC_MODULE_T mymodules[] = {
    {.new = _lnxproc_proc_cgroups_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_diskstats_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_domainname_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_hostname_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_osrelease_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_sys_cpufreq_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_sys_disksectors_new,.base = NULL,.optional = NULL,},
    {.new = _lnxproc_proc_pid_stat_new,.base = NULL,.optional = NULL,},
    {.new = NULL,.base = NULL,.optional = NULL,},
};

static size_t nmodules = sizeof(mymodules) / sizeof(mymodules[0]);

LNXPROC_ERROR_T
lnxproc_init(LNXPROC_MODULE_T ** modulesptr, LNXPROC_MODULE_TYPE_T type,
             void *optional)
{
    int n;

    if (type == LNXPROC_ALL) {
        n = nmodules;
    }
    else {
        n = 2;
    }
    LNXPROC_MODULE_T *p = calloc(n, sizeof(LNXPROC_MODULE_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE,
                             "Malloc interface\n");
        return LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE;
    }
    if (type == LNXPROC_ALL) {
        memcpy(p, mymodules, n * sizeof(LNXPROC_MODULE_T));
    }
    else {
        memcpy(p, mymodules + type - 1, sizeof(LNXPROC_MODULE_T));
        memcpy(p + 1, mymodules + nmodules - 1, sizeof(LNXPROC_MODULE_T));
    }
    *modulesptr = p;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_free(LNXPROC_MODULE_T ** modulesptr)
{
    if (modulesptr && *modulesptr) {
        LNXPROC_MODULE_T *modules = *modulesptr;
        LNXPROC_MODULE_T *module = modules;

        while (module->new) {
            _LNXPROC_BASE_FREE(module->base);
            module++;
        }
        free(modules);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_read(LNXPROC_MODULE_T * modules)
{

    LNXPROC_ERROR_T ret = LNXPROC_ERROR_INTERFACE_NULL;

    if (modules) {
        LNXPROC_MODULE_T *module = modules;

        while (module->new) {
            if (!module->base) {
                ret = module->new(&module->base);

                if (ret) {
                    return ret;
                }
            }
            _LNXPROC_BASE_T *base = module->base;

            ret = _lnxproc_base_rawread(base);
            if (ret) {
                return ret;
            }
            ret = _lnxproc_base_normalize(base);
            if (ret) {
                return ret;
            }
            module++;
        }
    }
    return ret;
}

LNXPROC_ERROR_T
lnxproc_performance(LNXPROC_MODULE_T * modules,
                    long *rawread_time, long *map_time, long *hash_time,
                    long *normalize_time)
{

    *rawread_time = 0;
    *map_time = 0;
    *hash_time = 0;
    *normalize_time = 0;
    if (modules) {
        LNXPROC_MODULE_T *module = modules;

        while (module->new) {
            _LNXPROC_BASE_T *base = module->base;

            if (base) {
                _LNXPROC_BASE_DATA_T *base_data = base->current;

                if (base_data) {
                    *rawread_time += base_data->rawread_time;
                    *map_time += base_data->map_time;
                    *hash_time += base_data->hash_time;
                    *normalize_time += base_data->normalize_time;
                }
            }
            module++;
        }
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_print(LNXPROC_MODULE_T * modules)
{
    if (modules) {
        LNXPROC_MODULE_T *module = modules;

        while (module->new) {
            _LNXPROC_BASE_T *base = module->base;

            if (base) {
                _LNXPROC_BASE_DATA_T *base_data = module->base->current;

                if (base_data) {
                    _lnxproc_results_print(base_data->results);
                }
            }
            module++;
        }
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
