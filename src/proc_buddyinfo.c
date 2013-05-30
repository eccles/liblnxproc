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

typical contents of /proc/buddyinfo file::

Node 0, zone      DMA      3      1      2      1      3      2      0      0      1      1      3 
Node 0, zone    DMA32  30698  18510  10904  10038   4225   1538    485    101     21      1      0 
Node 0, zone   Normal  85168  42417  24240  18985   8333   1863    343    136     11      0      0 

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"

static int
proc_buddyinfo_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;

    int i, j;

    _lnxproc_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        size_t ncols = array->vector->children[i]->length;

        _LNXPROC_DEBUG("%d:Ncols %zd\n", i, ncols);

        char *rowkey = values[i][0];

        if (!rowkey)
            continue;
        _LNXPROC_DEBUG("%d:Rowkey %s\n", i, rowkey);
        char *rownum = values[i][1];

        if (!rownum)
            continue;
        _LNXPROC_DEBUG("%d:Rownum %s\n", i, rownum);
        char *zone = values[i][3];

        if (!rownum)
            continue;
        _LNXPROC_DEBUG("%d:Zone %s\n", i, zone);

        char buf[64];
        int n = 0;

        STRLCAT(buf, "/", n, sizeof(buf));
        STRLCAT(buf, rowkey, n, sizeof(buf));
        STRLCAT(buf, rownum, n, sizeof(buf));
        STRLCAT(buf, "/", n, sizeof(buf));
        STRLCAT(buf, zone, n, sizeof(buf));
        STRLCAT(buf, "/", n, sizeof(buf));
        _LNXPROC_DEBUG("%d:base key %s\n", i, buf);

        for (j = 4; j < ncols; j++) {

            int k = j - 4;

            val = values[i][j];
            if (!val || !val[0])
                continue;

            char key[64];
            int m = 0;

            STRLCAT(key, buf, m, sizeof(key));
            INTCAT(key, k, m, sizeof(key));
            _LNXPROC_DEBUG("%d:key %s\n", i, key);

            _LNXPROC_DEBUG("%d,%d:value %s\n", i, j, val);
            int myval = (1 << k) * results->page_size * atoi(val);

            _lnxproc_results_add_int(results, key, myval);
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_buddyinfo_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 15, "\f\n", 2);        /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 4, ", ", 2);   /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/buddyinfo" };
    ret = _lnxproc_base_new(base, "proc_buddyinfo", _LNXPROC_BASE_TYPE_VANILLA,
                            filenames, 1, NULL, NULL, NULL,
                            NULL, proc_buddyinfo_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
