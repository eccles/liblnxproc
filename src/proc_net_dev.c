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

typical contents of /proc/net/dev file::

Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
   br0: 442449873 1127330    0    0    0     0          0         0 163183773  698831    0    0    0     0       0          0
 vnet1:  795289    8474    0    0    0     0          0         0 36660141  432868    0    0    0     0       0          0
 vnet3:  814495   11790    0    0    0     0          0         0 54014913  448248    0    0    0     0       0          0
  eth0: 478834149 1172419    0    0    0     0          0         0 182352206  801221    0    0    0     0       0          0
    lo: 3463381385 17945369    0    0    0     0          0         0 3463381385 17945369    0    0    0     0       0          0
virbr0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
 vnet0: 1174021   13420    0    0    0     0          0         0 38314754  436463    0    0    0     0       0          0
 wlan0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
 vnet2: 25128174   82211    0    0    0     0          0         0 29883316  390887    0    0    0     0       0          0
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
proc_net_dev_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;
    _LNXPROC_VECTOR_T *vector = array->vector;

    _LNXPROC_BASE_DATA_T *pdata = base->previous;
    _LNXPROC_RESULTS_T *presults = NULL;

    float tdiff = 0.0;

    if (pdata && pdata->array) {
        _lnxproc_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _LNXPROC_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _LNXPROC_DEBUG("Previous results is at %p\n", presults);
    }

    _LNXPROC_DEBUG("Time difference = %f secs\n", tdiff);

    size_t nrows = vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) vector->values;
    char *val;

    //char *colkey;
    char *rowkey;

    int i, j;

    _lnxproc_results_init(results, nrows);

    char **maintitles = values[0];
#ifdef DEBUG
    size_t nmain = vector->children[0]->length;

    _LNXPROC_DEBUG("Nmain %zd\n", nmain);
    for (i = 0; i < nmain; i++) {
        _LNXPROC_DEBUG("maintitle[%d] '%s'\n", i, maintitles[i]);
    }
#endif

    char **coltitles = values[1];

    size_t ncol = vector->children[1]->length;
#ifdef DEBUG
    _LNXPROC_DEBUG("Ncol %zd\n", ncol);
    for (i = 0; i < ncol; i++) {
        _LNXPROC_DEBUG("coltitle[%d] '%s'\n", i, coltitles[i]);
    }
#endif

    int transmit = 0;
    char *firsttitle = coltitles[1];

    _LNXPROC_DEBUG("firsttitle '%s'\n", firsttitle);
    for (i = 2; i < ncol; i++) {
        if (!strcmp(firsttitle, coltitles[i])) {
            transmit = i;
            break;
        }
    }
    _LNXPROC_DEBUG("Transmit %d\n", transmit);

    for (i = 2; i < nrows; i++) {
        size_t ncols = vector->children[i]->length;

        _LNXPROC_DEBUG("%d:Ncols %zd\n", i, ncols);

        rowkey = values[i][0];
        if (!rowkey)
            continue;

        _LNXPROC_DEBUG("%d:rowkey '%s'\n", i, rowkey);
        for (j = 1; j < ncols; j++) {
            val = values[i][j];
            if (!val)
                continue;
            _LNXPROC_DEBUG("%d,%d:Val '%s'\n", i, j, val);

            char buf[64];

            int n = 0;

            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, rowkey, n, sizeof(buf));
            STRLCAT(buf, "/", n, sizeof(buf));
            if (j < transmit) {
                STRLCAT(buf, maintitles[1], n, sizeof(buf));
            }
            else {
                STRLCAT(buf, maintitles[2], n, sizeof(buf));
            }
            STRLCAT(buf, "/", n, sizeof(buf));
            STRLCAT(buf, coltitles[j], n, sizeof(buf));
            _LNXPROC_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
            int current = atoi(val);

            _lnxproc_results_add_int(results, buf, current);
            if (tdiff > 0.0) {
                _LNXPROC_RESULTS_TABLE_T *pentry = NULL;

                int ret = _lnxproc_results_fetch(presults, buf, &pentry);

                if (ret)
                    continue;
                _LNXPROC_DEBUG("%d,%d:previous value for %s is %d\n", i, j,
                               buf, pentry->value.i);
                float rate = (current - pentry->value.i) / tdiff;

                _LNXPROC_DEBUG("%d,%d:rate = %f\n", i, j, rate);
                STRLCAT(buf, "-s", n, sizeof(buf));
                _LNXPROC_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
                _lnxproc_results_add_float(results, buf, rate);
            }
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_net_dev_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 5, "\f\n", 2); /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 4, "|: ", 3);  /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/dev" };
    ret = _lnxproc_base_new(base, "proc_net_dev", _LNXPROC_BASE_TYPE_PREVIOUS,
                            filenames, 1, NULL, NULL, NULL,
                            NULL, proc_net_dev_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
