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
#include <stdlib.h>             //strtoul()
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
proc_meminfo_normalize(_LNXPROC_BASE_T *base)
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

    int i;
    char hashkey[64];
    int n1 = 0;

    STRLCAT(hashkey, "/", n1, sizeof(hashkey));

    _lnxproc_results_init(results, nrows);

    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        char *key = value1[0];

        if (!key)
            continue;
        _LNXPROC_DEBUG("%d:key '%s'\n", i, key);
        int n2 = n1;

        STRLCAT(hashkey, key, n2, sizeof(hashkey));
        _LNXPROC_DEBUG("%d:hashkey '%s'\n", i, hashkey);

        char *val = value1[1];

        if (!val)
            continue;

        _LNXPROC_DEBUG("%d:val '%s'\n", i, val);
        unsigned long v = strtoul(val, NULL, 0);

        _lnxproc_results_add_unsigned_long(results, hashkey, v);

    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_meminfo_new(_LNXPROC_BASE_T **base, LNXPROC_OPT_T *optional)
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
    ret = _lnxproc_limits_set(limits, 1, 2, ": ", 2);   /* column delimiter */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/meminfo", };
    ret =
        _lnxproc_base_new(base, "proc_meminfo", _LNXPROC_BASE_TYPE_VANILLA,
                          NULL, proc_meminfo_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _lnxproc_base_set_filenames(*base, filenames, 1);
    }
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
