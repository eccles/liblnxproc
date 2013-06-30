/*
This file is part of topiary.

 topiary is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 topiary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with topiary.  If not, see <http://www.gnu.org/licenses/>.

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

#include "config.h"

#include "topiary_stdlib.h"
#include <stdio.h>
#include <string.h>

#include "strlcpy.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_partitions_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;
    char *rowkey;

    int i, j;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _topiary_results_init(results, nrows);
    char **titles = values[0];

    for (i = 1; i < nrows; i++) {
        char **value1 = (char **) values[i];

        rowkey = value1[3];
        if (!rowkey)
            continue;
        _TOPIARY_DEBUG("%d:Rowkey '%s'\n", i, rowkey);

        int n2 = n1;

        STRLCAT(buf, rowkey, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 0; j < 3; j++) {
            val = value1[j];
            if (!val)
                continue;

            int n3 = n2;

            STRLCAT(buf, titles[j], n3, sizeof(buf));
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, buf);
            _TOPIARY_DEBUG("%d,%d:val '%s'\n", i, j, val);
            _topiary_results_add_unsigned_long(results, buf,
                                               strtoul(val, NULL, 0));
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_partitions_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/partitions" };
    ret = _topiary_base_new(base, "proc_partitions", _TOPIARY_BASE_TYPE_VANILLA,
                            NULL, proc_partitions_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
