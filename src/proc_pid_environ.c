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
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_pid_environ_normalize(_LNXPROC_BASE_T * base)
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

    int i, j, k;

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
            _LNXPROC_VECTOR_T *grandchild = child->children[j];
            size_t ncols = grandchild->length;
            char **value2 = (char **) value1[j];

            _LNXPROC_DEBUG("%d,%d:ncols = %zd\n", i, j, ncols);

            char *key = value2[0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);

            int n3 = n2;

            STRLCAT(buf, key, n3, sizeof(buf));

            _LNXPROC_DEBUG("%d,%d:hash key '%s'\n", i, j, buf);

            if (ncols < 3) {
                char *val = value2[1];

                if (val) {
                    _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, val);
                    _lnxproc_results_add_stringref(results, buf, val);
                }
                else {
                    _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, "");
                    _lnxproc_results_add_stringref(results, buf, "");
                }
            }
            else {
                _LNXPROC_DEBUG("%d,%d:ncols = %zd\n", i, j, ncols);
                char *val1 = value2[1];

                if (!val1)
                    continue;

                char *val = NULL;

                for (k = 2; k < ncols; k++) {
                    val = value2[k];

                    if (!val)
                        continue;

                    *(--val) = '=';
                }
                _LNXPROC_DEBUG("%d,%d:value1 '%s'\n", i, j, val1);
                _lnxproc_results_add_stringref(results, buf, val1);
            }
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_pid_environ_new(_LNXPROC_BASE_T ** base, LNXPROC_OPT_T * optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 3);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters (per pid) */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 2, "", 1);     /* column delimiter is \0 */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 2, 2, "=", 1);    /* PARAM=VALUE */
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

    char *filesuffix = "environ";

    ret =
        _lnxproc_base_new(base, "proc_pid_environ", _LNXPROC_BASE_TYPE_VANILLA,
                          NULL, proc_pid_environ_normalize, NULL, 256, limits);
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
