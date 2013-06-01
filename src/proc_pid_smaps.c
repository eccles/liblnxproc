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

/proc/[pid]/smaps

00400000-004e1000 r-xp 00000000 fc:00 1310760                            /bin/bash
Size:                900 kB
Rss:                 708 kB
Pss:                 245 kB
Shared_Clean:        704 kB
Shared_Dirty:          0 kB
Private_Clean:         4 kB
Private_Dirty:         0 kB
Referenced:          708 kB
Anonymous:             0 kB
AnonHugePages:         0 kB
Swap:                  0 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Locked:                0 kB
VmFlags: rd ex mr mw me dw 
006e0000-006e1000 r--p 000e0000 fc:00 1310760                            /bin/bash
Size:                  4 kB
Rss:                   4 kB
Pss:                   4 kB
Shared_Clean:          0 kB
Shared_Dirty:          0 kB
Private_Clean:         0 kB
Private_Dirty:         4 kB
Referenced:            4 kB
Anonymous:             4 kB
AnonHugePages:         0 kB
Swap:                  0 kB
KernelPageSize:        4 kB
MMUPageSize:           4 kB
Locked:                0 kB
VmFlags: rd mr mw me dw ac 
......
*/

#include <ctype.h>              // isupper
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
proc_pid_smaps_normalize(_LNXPROC_BASE_T * base)
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

    _lnxproc_results_init(results, npids);
    for (i = 0; i < npids; i++) {
        char ***value1 = (char ***) values[i];
        char *pidkey = value1[0][0];

        if (!pidkey)
            continue;

        _LNXPROC_VECTOR_T *child = vector->children[i];
        size_t nrows = child->length;

        _LNXPROC_DEBUG("%d:pidkey '%s'\n", i, pidkey);
        _LNXPROC_DEBUG("%d:nrows = %zd\n", i, nrows);
        int first = -1;
        int last = -1;

/*
 * get first parameter positon
 */
        for (j = 1; j < nrows; j++) {

            char *key = value1[j][0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _LNXPROC_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (!isupper(key[0])) {
                j++;
                first = j;
                _LNXPROC_DEBUG("%d,%d:first %d\n", i, j, first);
                break;
            }
        }
        if (first < 0)
            continue;

/*
 * get last parameter positon
 */
        for (; j < nrows && first > 0; j++) {

            char *key = value1[j][0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _LNXPROC_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (!isupper(key[0])) {
                last = j;
                _LNXPROC_DEBUG("%d,%d:last %d\n", i, j, last);
                break;
            }
        }
        if (last < 0)
            continue;

        int nparams = last - first;

        _LNXPROC_DEBUG("%d:nparams %d\n", i, nparams);
        if (nparams < 2)
            continue;

/*
 * get array of field names
 */
        char *fields[nparams];

        int n = 0;

        for (j = first; j < last; j++) {

            char *key = value1[j][0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _LNXPROC_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (n < nparams) {
                fields[n] = key;
                _LNXPROC_DEBUG("%d,%d:fields[%d] '%s'\n", i, j, n, fields[n]);
                n++;
            }
        }

/*
 * sum all fields
 */
        long sums[nparams];

        memset(sums, 0, nparams * sizeof(long));

        for (j = 1; j < nrows; j++) {
            char **value2 = (char **) value1[j];

            char *key = value2[0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);

            if (!isupper(key[0])) {
                n = -1;
                continue;
            }
            else {
                n++;
            }
            _LNXPROC_DEBUG("%d,%d:n = %d\n", i, j, n);
            _LNXPROC_DEBUG("%d,%d:fields[%d] '%s'\n", i, j, n, fields[n]);

            if (!strcmp("VmFlags", key))
                continue;

            char *val = value2[1];

            if (!val)
                continue;
            _LNXPROC_DEBUG("%d,%d,%d:val = '%s'\n", i, j, 1, val);

            if (n < nparams) {
                sums[n] += atoi(val);

                _LNXPROC_DEBUG("%d,%d:sums[%d] %ld\n", i, j, n, sums[n]);
            }
        }
/*
 *  write out results
 */
        char hash[64];

        int m = 0;

        STRLCAT(hash, "/", m, sizeof(hash));
        STRLCAT(hash, pidkey, m, sizeof(hash));
        STRLCAT(hash, "/", m, sizeof(hash));

        for (j = 0; j < nparams; j++) {
            char *key = fields[j];

            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);
            if (!strcmp("VmFlags", key))
                continue;

            long val = sums[j];

            _LNXPROC_DEBUG("%d,%d:val %ld\n", i, j, val);

            n = m;
            STRLCAT(hash, key, n, sizeof(hash));
            _LNXPROC_DEBUG("%d,%d:hash '%s'\n", i, j, hash);

            _lnxproc_results_add_long(results, hash, val);
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_proc_pid_smaps_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 3);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f", 1);   /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 9, "\n", 1);   /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 2, 4, ": ", 2);   /* column delimiters */
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

    char *filesuffix = "smaps";

    ret = _lnxproc_base_new(base, "proc_pid_smaps", _LNXPROC_BASE_TYPE_PREVIOUS,
                            NULL, 0, fileprefix, fileglob, filesuffix,
                            NULL, proc_pid_smaps_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
