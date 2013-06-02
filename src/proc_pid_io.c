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
proc_pid_io_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_BASE_DATA_T *data = base->current;
    _LNXPROC_RESULTS_T *results = data->results;
    _LNXPROC_ARRAY_T *array = data->array;
    _LNXPROC_VECTOR_T *vector = array->vector;

    size_t npids = vector->length;

    _LNXPROC_DEBUG("Current data is %d at %p\n", data->id, data);
    _LNXPROC_DEBUG("Current results is at %p\n", results);
    _LNXPROC_DEBUG("Npids %zd\n", npids);
    char ****values = (char ****) vector->values;

    int i, j;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _lnxproc_results_init(results, npids);
    for (i = 0; i < npids; i++) {
        char ***value1 = (char ***) values[i];
        char *pidkey = value1[0][0];

        if (!pidkey)
            continue;

        _LNXPROC_VECTOR_T *child = vector->children[i];
        size_t nrows = child->length;

        _LNXPROC_DEBUG("%d:first pidkey value '%s'\n", i, pidkey);
        _LNXPROC_DEBUG("%d:nrows = %zd\n", i, nrows);

        int n2 = n1;

        STRLCAT(buf, pidkey, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 1; j < nrows; j++) {
            char **value2 = (char **) value1[j];

            char *key = value2[0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);

            char *val = value2[1];

            if (!val)
                continue;
            _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, val);

            int n3 = n2;

            STRLCAT(buf, key, n3, sizeof(buf));

            _LNXPROC_DEBUG("%d,%d:hash key '%s'\n", i, j, buf);

            _lnxproc_results_add_long(results, buf, atol(val));
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_pid_io_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 3);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f", 1);   /* row delimiters (per pid) */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 2, "\n", 1);   /* column delimiter */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 2, 2, " :", 2);
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *fileprefix = "/proc";
    char *fileglob;

    if (optional) {
        fileglob = optional;
    }
    else {
        fileglob = "[1-9]*";
    }

    char *filesuffix = "io";

    ret =
        _lnxproc_base_new(base, "proc_pid_io", _LNXPROC_BASE_TYPE_VANILLA,
                          NULL, 0, fileprefix, fileglob, filesuffix, NULL,
                          proc_pid_io_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
