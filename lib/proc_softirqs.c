/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

#include "config.h"

#include <stdio.h>
#include "topiary_stdlib.h"
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_softirqs_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_BASE_DATA_T *data = base->current;
    _TOPIARY_RESULTS_T *results = data->results;
    _TOPIARY_ARRAY_T *array = data->array;
    _TOPIARY_VECTOR_T *vector = array->vector;

    size_t nrows = vector->length;

    _TOPIARY_DEBUG("Current data is %d at %p\n", data->id, data);
    _TOPIARY_DEBUG("Current results is at %p\n", results);
    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) vector->values;

    float tdiff = 0.0;
    _TOPIARY_BASE_DATA_T *pdata = base->previous;
    _TOPIARY_RESULTS_T *presults = NULL;

    if (pdata) {
        _topiary_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _TOPIARY_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _TOPIARY_DEBUG("Previous results is at %p\n", presults);
        _TOPIARY_DEBUG("Time difference = %f secs\n", tdiff);
    }

    int i, j;

    int n1 = 0;
    char hashkey[64];

    STRLCAT(hashkey, "/", n1, sizeof(hashkey));

    _topiary_results_init(results, nrows);

    size_t ncpus = vector->children[0]->length;

    char **titles = (char **) values[0];

    _TOPIARY_DEBUG("Ncpus %zd\n", ncpus);

    for (i = 1; i < nrows; i++) {
        char **value1 = (char **) values[i];

        char *key = value1[0];

        if (!key)
            continue;

        _TOPIARY_DEBUG("%d:key %s\n", i, key);
        int n2 = n1;

        STRLCAT(hashkey, key, n2, sizeof(hashkey));
        STRLCAT(hashkey, "/", n2, sizeof(hashkey));

        for (j = 1; j < ncpus + 1; j++) {
            char *val = value1[j];

            if (!val)
                continue;

            _TOPIARY_DEBUG("%d,%d:title %s\n", i, j - 1, titles[j - 1]);
            int n3 = n2;

            STRLCAT(hashkey, titles[j - 1], n3, sizeof(hashkey));
            _TOPIARY_DEBUG("%d,%d:hashkey %s\n", i, j, hashkey);

            unsigned long v = strtoul(val, NULL, 0);

            _TOPIARY_DEBUG("%d,%d:val %lu\n", i, j, v);
            _topiary_results_add_unsigned_long(results, hashkey, v);
            if (tdiff > 0.0) {
                _TOPIARY_RESULTS_TABLE_T *pentry = NULL;

                int ret = _topiary_results_fetch(presults, hashkey, &pentry);

                if (ret)
                    continue;

                STRLCAT(hashkey, "-s", n3, sizeof(hashkey));
                float value = (v - pentry->value.ul) / tdiff;

                _topiary_results_add_fixed(results, hashkey, value, 0, 2);
#ifdef DEBUG
                _TOPIARY_DEBUG("%d,%d:Curr %s = %f\n", i, j, hashkey, value);
                if (value < 0.0) {
                    _TOPIARY_DEBUG("WARN: diff < 0 (=%f)\n", value);
                }
#endif
            }
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_softirqs_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
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
    ret = _topiary_limits_set(limits, 1, 2, ": ", 2);   /* column delimiter */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/softirqs", };
    ret =
        _topiary_base_new(base, "proc_softirqs", _TOPIARY_BASE_TYPE_PREVIOUS,
                          NULL, proc_softirqs_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
