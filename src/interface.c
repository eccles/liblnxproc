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

static LNXPROC_MODULE_T mymodules[] = {
    {.new = lnxproc_proc_cgroups_new,.base = NULL,},
    {.new = lnxproc_proc_diskstats_new,.base = NULL,},
    {.new = lnxproc_proc_domainname_new,.base = NULL,},
    {.new = lnxproc_proc_hostname_new,.base = NULL,},
    {.new = lnxproc_proc_osrelease_new,.base = NULL,},
    {.new = lnxproc_sys_cpufreq_new,.base = NULL,},
    {.new = lnxproc_sys_disksectors_new,.base = NULL,},
};

static size_t nmodules = sizeof(mymodules) / sizeof(mymodules[0]);

LNXPROC_ERROR_T
lnxproc_init(LNXPROC_MODULE_T ** modules)
{
    LNXPROC_MODULE_T *p = calloc(1, sizeof(mymodules));

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE,
                            "Malloc interface\n");
        return LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE;
    }
    memcpy(p, mymodules, sizeof mymodules);
    *modules = p;
    return LNXPROC_OK;
}

LNXPROC_MODULE_T *
lnxproc_free(LNXPROC_MODULE_T * modules)
{
    if (modules) {
        int i;

        for (i = 0; i < nmodules; i++) {
            _LNXPROC_BASE_FREE(modules[i].base);
        }
        free(modules);
    }
    return NULL;
}

LNXPROC_RESULTS_T *
lnxproc_read(LNXPROC_MODULE_T * modules, LNXPROC_MODULE_TYPE_T type)
{

    if (modules) {
        if (type == LNXPROC_ALL) {
        }
        else {
            LNXPROC_MODULE_T *module = modules + type + 1;

            if (!module->base) {
                LNXPROC_ERROR_T ret = module->new(&module->base);

                if (ret) {
                    return NULL;
                }
            }

            return module->base->read(module->base);
        }
    }
    return NULL;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
