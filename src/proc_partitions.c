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

typical contents of /proc/partitions file::

   subsys_name hierarchy num_partitions enabled
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

static int
proc_partitions_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;
    char *rowkey;

    int i, j;

    _lnxproc_results_init(results, nrows);
    char **titles = values[0];

    for (i = 1; i < nrows; i++) {
        rowkey = values[i][3];
        if (!rowkey)
            continue;
        _LNXPROC_DEBUG("%d:Rowkey '%s'\n", i, rowkey);
        for (j = 0; j < 3; j++) {
            val = values[i][j];
            if (!val)
                continue;
            char buf[64];

            int n = 0;

            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, rowkey, n, sizeof(buf));
            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, titles[j], n, sizeof(buf));
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, buf);
            _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, val);
            _lnxproc_results_add_int(results, buf, atoi(val));
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_partitions_new(_LNXPROC_BASE_T ** base, void *optional)
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
    ret = _lnxproc_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/partitions" };
    ret = _lnxproc_base_new(base, "proc_partitions", _LNXPROC_BASE_TYPE_VANILLA,
                            filenames, 1, NULL, NULL, NULL,
                            NULL, proc_partitions_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */