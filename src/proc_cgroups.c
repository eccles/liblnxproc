/*
This file is part of liblnxproc.

 liblnxproc is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 liblnxproc is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Paul Hewlett, phewlett76@gmail.com

typical contents of /proc/cgroups file::

   subsys_name hierarchy num_cgroups enabled
   cpuset     1     4 1
   cpu        2     4 1
   cpuacct    3     4 1
   memory     4     4 1
   devices    5     4 1
   freezer    6     4 1
   blkio      7     4 1
   perf_event 8     1 1

*/

#include <stdlib.h>

#include "base_private.h"
#include "proc_cgroups.h"
/*
    struct proc_cgroups_sub_t {
        int hierarchy;
        int num_cgroups;
        int enabled;
    };

    struct {
        struct proc_cgroups_sub_t cpuset;
        struct proc_cgroups_sub_t cpu;
        struct proc_cgroups_sub_t cpuacct;
        struct proc_cgroups_sub_t memory;
        struct proc_cgroups_sub_t devices;
        struct proc_cgroups_sub_t freezer;
        struct proc_cgroups_sub_t blkio;
        struct proc_cgroups_sub_t perf_event;
    } proc_cgroups_data;

*/

static LNXPROC_PROC_CGROUPS_T proc_cgroups_data;

static int
proc_cgroups_normalize(LNXPROC_BASE_T *base)
{
    lnxproc_base_print(base, 1, NULL);

    struct map_t {
        size_t idx[2];
        int *addr;
    };

    struct map_t map[] = {
        {{0, 0}, &proc_cgroups_data.cpuset.hierarchy},
        {{0, 1}, &proc_cgroups_data.cpuset.num_cgroups},
        {{0, 2}, &proc_cgroups_data.cpuset.enabled},
        {{1, 0}, &proc_cgroups_data.cpu.hierarchy},
        {{1, 1}, &proc_cgroups_data.cpu.num_cgroups},
        {{1, 2}, &proc_cgroups_data.cpu.enabled},
        {{2, 0}, &proc_cgroups_data.cpuacct.hierarchy},
        {{2, 1}, &proc_cgroups_data.cpuacct.num_cgroups},
        {{2, 2}, &proc_cgroups_data.cpuacct.enabled},
        {{3, 0}, &proc_cgroups_data.memory.hierarchy},
        {{3, 1}, &proc_cgroups_data.memory.num_cgroups},
        {{3, 2}, &proc_cgroups_data.memory.enabled},
        {{4, 0}, &proc_cgroups_data.devices.hierarchy},
        {{4, 1}, &proc_cgroups_data.devices.num_cgroups},
        {{4, 2}, &proc_cgroups_data.devices.enabled},
        {{5, 0}, &proc_cgroups_data.freezer.hierarchy},
        {{5, 1}, &proc_cgroups_data.freezer.num_cgroups},
        {{5, 2}, &proc_cgroups_data.freezer.enabled},
        {{6, 0}, &proc_cgroups_data.blkio.hierarchy},
        {{6, 1}, &proc_cgroups_data.blkio.num_cgroups},
        {{6, 2}, &proc_cgroups_data.blkio.enabled},
        {{7, 0}, &proc_cgroups_data.perf_event.hierarchy},
        {{7, 1}, &proc_cgroups_data.perf_event.num_cgroups},
        {{7, 2}, &proc_cgroups_data.perf_event.enabled},
    };

    size_t dims = sizeof(map) / sizeof(map[0]);

    int i;

    for (i = 0; i < dims; i++) {
        *map[i].addr = atoi(lnxproc_array_get(base->array, map[i].idx, 2));
    }

    return 0;
}

LNXPROC_BASE_T *
lnxproc_proc_cgroups_init(void)
{

    LNXPROC_LIMITS_T limits[] = {
        {9, "\n", 1},           /* row delimiters */
        {4, "\t", 1}            /* column delimiters */
    };

    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return lnxproc_base_init("/proc/cgroups",
                             NULL,
                             proc_cgroups_normalize,
                             NULL,
                             lnxproc_error_print_callback,
                             256, limits, dim, &proc_cgroups_data);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
