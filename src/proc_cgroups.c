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

#include "strlcpy.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_cgroups.h"

static int
proc_cgroups_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;
    char *colkey;
    char *rowkey;

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
            char buf[64];

            int n = 0;

            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, rowkey, n, sizeof(buf));
            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, colkey, n, sizeof(buf));
            _lnxproc_results_add_int(results, buf, atoi(val));
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_cgroups_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\n", 1);   /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 4, "\t", 1);   /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/cgroups" };
    ret = _lnxproc_base_new(base, "proc_cgroups", _LNXPROC_BASE_TYPE_VANILLA,
                            filenames, 1, NULL, NULL, NULL,
                            NULL, proc_cgroups_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
