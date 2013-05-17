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
#include <stdio.h>
#include <string.h>

#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_cgroups.h"

static LNXPROC_ERROR_T
proc_cgroups_normalize(LNXPROC_BASE_T *base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;
    char *colkey;
    char *rowkey;
    _LNXPROC_RESULTS_TABLE_T entry;

    int i, j;

    _lnxproc_results_init(results, nrows);
    for (i = 1; i < nrows; i++) {
        size_t ncols = array->vector->children[i]->length;

        rowkey = values[i][0];
        if (!rowkey)
            continue;

        for (j = 1; j < ncols; j++) {
            colkey = values[0][j];
            if (!colkey)
                continue;
            val = values[i][j];
            if (!val)
                continue;
            snprintf(entry.key, sizeof entry.key, "/%s/%s", rowkey, colkey);
            entry.valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_INT;
            entry.value.i = atoi(val);
            _lnxproc_results_add(results, &entry);
        }
    }
    _lnxproc_results_hash(results);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_cgroups_new(LNXPROC_BASE_T **base)
{

    _LNXPROC_LIMITS_T limits[] = {
        {.expected = 9,
         .chars = "\n",
         .len = 1},             /* row delimiters */
        {.expected = 4,
         .chars = "\t",
         .len = 1}              /* column delimiters */
    };

    char *filenames[] = { "/proc/cgroups" };
    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return _lnxproc_base_new(base, LNXPROC_BASE_TYPE_VANILLA,
                             filenames, 1, NULL, NULL, NULL,
                             NULL, proc_cgroups_normalize, NULL, 256,
                             limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
