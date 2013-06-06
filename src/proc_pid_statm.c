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

/proc/[pid]/statm
              Provides information about memory usage, measured in pages.  The columns are:

                  size       total program size
                             (same as VmSize in /proc/[pid]/status)
                  resident   resident set size
                             (same as VmRSS in /proc/[pid]/status)
                  share      shared pages (from shared mappings)
                  text       text (code)
                  lib        library (unused in Linux 2.6)
                  data       data + stack
                  dt         dirty pages (unused in Linux 2.6)

/proc/pid/statm:

5899 1235 471 225 0 761 0
*/

#include <stdio.h>
#include <stdlib.h>
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
proc_pid_statm_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_BASE_DATA_T *data = base->current;
    _LNXPROC_RESULTS_T *results = data->results;
    _LNXPROC_ARRAY_T *array = data->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Current data is %d at %p\n", data->id, data);
    _LNXPROC_DEBUG("Current results is at %p\n", results);
    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;

    int i, j;

    static const char *colkey[] = {
        "size", "resident", "share", "test", "lib",
        "data", "dt",
    };
    char key[64];

    int n1 = 0;

    STRLCAT(key, "/", n1, sizeof(key));

    _lnxproc_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        char *rowkey = value1[0];

        if (!rowkey)
            continue;

        size_t ncols = array->vector->children[i]->length;

        _LNXPROC_DEBUG("%d:ncols %zd\n", i, ncols);

        _LNXPROC_DEBUG("%d:Rowkey value %s\n", i, rowkey);
        int n2 = n1;

        STRLCAT(key, rowkey, n2, sizeof(key));
        STRLCAT(key, "/", n2, sizeof(key));

        for (j = 1; j < ncols; j++) {
            char *val = value1[j];

            if (!val)
                continue;

            int n3 = n2;

            STRLCAT(key, colkey[j - 1], n3, sizeof(key));

            int value = atoi(val) * results->page_size;

            _LNXPROC_DEBUG("%d,%d:%s value %s int %d\n", i, j, key, val, value);
            _lnxproc_results_add_int(results, key, value);
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_proc_pid_statm_new(_LNXPROC_BASE_T ** base, LNXPROC_OPT_T * optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *fileprefix = "/proc";
    char *fileglob;

    if (optional && optional->fileglob) {
        fileglob = optional->fileglob;
    }
    else {
        fileglob = "[1-9]*";
    }

    char *filesuffix = "statm";

    ret = _lnxproc_base_new(base, "proc_pid_statm", _LNXPROC_BASE_TYPE_PREVIOUS,
                            NULL, proc_pid_statm_normalize, NULL, 256, limits);
    if (!ret) {
        _lnxproc_base_set_fileprefix(*base, fileprefix);
        _lnxproc_base_set_fileglob(*base, fileglob);
        _lnxproc_base_set_filesuffix(*base, filesuffix);
    }
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
