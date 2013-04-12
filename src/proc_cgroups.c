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

#include "base_private.h"
#include "proc_cgroups.h"

static int
proc_cgroups_normalize(LNXPROC_BASE_T *base)
{
    lnxproc_base_print(base, 1, NULL);
    return 0;
}

LNXPROC_BASE_T *
proc_cgroups_init(void)
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
