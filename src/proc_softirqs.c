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
#include "val2str.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"

static int
proc_softirqs_normalize(_LNXPROC_BASE_T * base)
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

    float tdiff = 0.0;
    _LNXPROC_BASE_DATA_T *pdata = base->previous;
    _LNXPROC_RESULTS_T *presults = NULL;

    if (pdata) {
        _lnxproc_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _LNXPROC_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _LNXPROC_DEBUG("Previous results is at %p\n", presults);
        _LNXPROC_DEBUG("Time difference = %f secs\n", tdiff);
    }

    int i, j;

    int n1 = 0;
    char hashkey[64];

    STRLCAT(hashkey, "/", n1, sizeof(hashkey));

    _lnxproc_results_init(results, nrows);

    size_t ncpus = vector->children[0]->length;

    char **titles = (char **) values[0];

    _LNXPROC_DEBUG("Ncpus %zd\n", ncpus);

    for (i = 1; i < nrows; i++) {
        char **value1 = (char **) values[i];

        char *key = value1[0];

        if (!key)
            continue;

        _LNXPROC_DEBUG("%d:key %s\n", i, key);
        int n2 = n1;

        STRLCAT(hashkey, key, n2, sizeof(hashkey));
        STRLCAT(hashkey, "/", n2, sizeof(hashkey));

        for (j = 1; j < ncpus + 1; j++) {
            char *val = value1[j];

            if (!val)
                continue;

            _LNXPROC_DEBUG("%d,%d:title %s\n", i, j - 1, titles[j - 1]);
            int n3 = n2;

            STRLCAT(hashkey, titles[j - 1], n3, sizeof(hashkey));
            _LNXPROC_DEBUG("%d,%d:hashkey %s\n", i, j, hashkey);

            int v = atoi(val);

            _LNXPROC_DEBUG("%d,%d:val %d\n", i, j, v);
            _lnxproc_results_add_int(results, hashkey, v);
            if (tdiff > 0.0) {
                _LNXPROC_RESULTS_TABLE_T *pentry = NULL;

                int ret = _lnxproc_results_fetch(presults, hashkey, &pentry);

                if (ret)
                    continue;

                STRLCAT(hashkey, "-s", n3, sizeof(hashkey));
                float value = (v - pentry->value.i) / tdiff;

                _lnxproc_results_add_float(results, hashkey, value);
#ifdef DEBUG
                _LNXPROC_DEBUG("%d,%d:Curr %s = %f\n", i, j, hashkey, value);
                if (value < 0.0) {
                    _LNXPROC_DEBUG("WARN: diff < 0 (=%f)\n", value);
                }
#endif
            }
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_softirqs_new(_LNXPROC_BASE_T ** base, void *optional)
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

    char *filenames[] = { "/proc/softirqs", };
    ret =
        _lnxproc_base_new(base, "proc_softirqs", _LNXPROC_BASE_TYPE_PREVIOUS,
                          filenames, 1, NULL, NULL, NULL, NULL,
                          proc_softirqs_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
