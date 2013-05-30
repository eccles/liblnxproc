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

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strlcpy.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"

static int
proc_cpuinfo_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_BASE_DATA_T *data = base->current;
    _LNXPROC_RESULTS_T *results = data->results;
    _LNXPROC_ARRAY_T *array = data->array;
    _LNXPROC_VECTOR_T *vector = array->vector;

    size_t nrows = vector->length;

    _LNXPROC_DEBUG("Current data is %d at %p\n", data->id, data);
    _LNXPROC_DEBUG("Current results is at %p\n", results);
    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) vector->values;

    int i, m, n;
    char prockey[32] = "";
    char key[64] = "";

    _lnxproc_results_init(results, nrows);
    int first = 2;

    for (i = 0; i < nrows; i++) {
        char *rowkey = values[i][0];

        if (!rowkey)
            continue;
        _LNXPROC_DEBUG("%d:rowkey '%s'\n", i, rowkey);

        char *val = values[i][1];

        if (!val)
            continue;
        while (*val == ' ')
            val++;

        if (!strcmp(rowkey, "processor")) {
            m = 0;
            STRLCAT(prockey, "/", m, sizeof(prockey));
            STRLCAT(prockey, val, m, sizeof(prockey));
            STRLCAT(prockey, "/", m, sizeof(prockey));
            first--;
            _LNXPROC_DEBUG("%d:first %d prockey '%s'\n", i, first, prockey);
        }

        else {
            if (!strcmp(rowkey, "address sizes") ||
                !strcmp(rowkey, "bogomips") ||
                !strcmp(rowkey, "cache_alignment") ||
                !strcmp(rowkey, "cache size") ||
                !strcmp(rowkey, "clflush size") ||
                !strcmp(rowkey, "cpu cores") ||
                !strcmp(rowkey, "cpu family") ||
                !strcmp(rowkey, "cpu MHz") ||
                !strcmp(rowkey, "cpuid level") ||
                !strcmp(rowkey, "flags") ||
                !strcmp(rowkey, "fpu") ||
                !strcmp(rowkey, "fpu_exception") ||
                !strcmp(rowkey, "model") ||
                !strcmp(rowkey, "model name") ||
                !strcmp(rowkey, "microcode") ||
                !strcmp(rowkey, "power management") ||
                !strcmp(rowkey, "siblings") ||
                !strcmp(rowkey, "stepping") ||
                !strcmp(rowkey, "vendor_id") || !strcmp(rowkey, "wp")) {
                _LNXPROC_DEBUG("%d:first %d\n", i, first);
                if (first > 0) {
                    n = 0;
                    STRLCAT(key, "/", n, sizeof(key));
                    STRLCAT(key, rowkey, n, sizeof(key));
                    _LNXPROC_DEBUG("%d:key '%s'\n", i, key);
                    _LNXPROC_DEBUG("%d:val '%s'\n", i, val);
                    _lnxproc_results_add_stringref(results, key, val);
                }
            }
            else {
                n = 0;

                STRLCAT(key, prockey, n, sizeof(key));
                STRLCAT(key, rowkey, n, sizeof(key));
                _LNXPROC_DEBUG("%d:key '%s'\n", i, key);
                _LNXPROC_DEBUG("%d:val '%s'\n", i, val);

                _lnxproc_results_add_stringref(results, key, val);
            }
        }

    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_cpuinfo_new(_LNXPROC_BASE_T ** base, void *optional)
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
    ret = _lnxproc_limits_set(limits, 1, 2, ":\t", 2);  /* column delimiter */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/cpuinfo", };
    ret =
        _lnxproc_base_new(base, "proc_cpuinfo", _LNXPROC_BASE_TYPE_MEMOIZE,
                          filenames, 1, NULL, NULL, NULL, NULL,
                          proc_cpuinfo_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
