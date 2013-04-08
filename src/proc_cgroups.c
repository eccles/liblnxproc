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

#include "error.h"
#include "map_limits.h"
#include "base.h"
#include "array.h"
#include "map.h"
#include "proc_cgroups.h"

static int
proccgroups_normalize(LNXPROC_BASE_T *base,
                      int *arraydims,
                      LNXPROC_MAP_LIMITS_T * maplimits, int mapdim)
{
    lnxproc_map_split(base, arraydims, maplimits, mapdim);
    lnxproc_base_print(base, NULL);
    return 0;
}

LNXPROC_BASE_T *
proccgroups_init(void)
{

    LNXPROC_MAP_LIMITS_T maplimits[] = {
        {"\n", 1},              /* row delimiters */
        {"\t", 1}               /* column delimiters */
    };

    int arraydims[] = { 9, 4 }; /* expected dimensions of matrix */
    int dim = 2;

    return lnxproc_base_init("/proc/cgroups",
                             NULL,
                             proccgroups_normalize,
                             NULL,
                             lnxproc_error_print_callback,
                             256, arraydims, maplimits, dim, &proccgroups_data);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
