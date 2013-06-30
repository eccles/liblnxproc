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

#include "config.h"

#include <ctype.h>              // isupper
#include <stdio.h>
#include "topiary_stdlib.h"
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
proc_pid_smaps_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_BASE_DATA_T *data = base->current;
    _TOPIARY_RESULTS_T *results = data->results;
    _TOPIARY_ARRAY_T *array = data->array;
    _TOPIARY_VECTOR_T *vector = array->vector;

    size_t npids = vector->length;

    _TOPIARY_DEBUG("Current data is %d at %p\n", data->id, data);
    _TOPIARY_DEBUG("Current results is at %p\n", results);
    _TOPIARY_DEBUG("Npids %zd\n", npids);
    char ****values = (char ****) vector->values;

    int i, j;

    char hash[64];

    int n1 = 0;

    STRLCAT(hash, "/", n1, sizeof(hash));

    _topiary_results_init(results, npids);

    for (i = 0; i < npids; i++) {
        char ***value1 = (char ***) values[i];
        char *pidkey = value1[0][0];

        if (!pidkey)
            continue;

        _TOPIARY_VECTOR_T *child = vector->children[i];
        size_t nrows = child->length;

        _TOPIARY_DEBUG("%d:pidkey '%s'\n", i, pidkey);
        _TOPIARY_DEBUG("%d:nrows = %zd\n", i, nrows);
        int first = -1;
        int last = -1;

/*
 * get first parameter positon
 */
        for (j = 1; j < nrows; j++) {

            char *key = value1[j][0];

            if (!key)
                continue;
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _TOPIARY_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (!isupper(key[0])) {
                j++;
                first = j;
                _TOPIARY_DEBUG("%d,%d:first %d\n", i, j, first);
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
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _TOPIARY_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (!isupper(key[0])) {
                last = j;
                _TOPIARY_DEBUG("%d,%d:last %d\n", i, j, last);
                break;
            }
        }
        if (last < 0)
            continue;

        int nparams = last - first;

        _TOPIARY_DEBUG("%d:nparams %d\n", i, nparams);
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
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);
            _TOPIARY_DEBUG("%d,%d:key[0] '%c'\n", i, j, key[0]);
            if (n < nparams) {
                fields[n] = key;
                _TOPIARY_DEBUG("%d,%d:fields[%d] '%s'\n", i, j, n, fields[n]);
                n++;
            }
        }

/*
 * sum all fields
 */
        unsigned long sums[nparams];

        memset(sums, 0, nparams * sizeof(long));

        for (j = 1; j < nrows; j++) {
            char **value2 = (char **) value1[j];

            char *key = value2[0];

            if (!key)
                continue;
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);

            if (!isupper(key[0])) {
                n = -1;
                continue;
            }
            else {
                n++;
            }
            _TOPIARY_DEBUG("%d,%d:n = %d\n", i, j, n);
            _TOPIARY_DEBUG("%d,%d:fields[%d] '%s'\n", i, j, n, fields[n]);

            if (!strcmp("VmFlags", key))
                continue;

            char *val = value2[1];

            if (!val)
                continue;
            _TOPIARY_DEBUG("%d,%d,%d:val = '%s'\n", i, j, 1, val);

            if (n < nparams) {
                sums[n] += strtoul(val, NULL, 0);

                _TOPIARY_DEBUG("%d,%d:sums[%d] %lu\n", i, j, n, sums[n]);
            }
        }
/*
 *  write out results
 */
        int n2 = n1;

        STRLCAT(hash, pidkey, n2, sizeof(hash));
        STRLCAT(hash, "/", n2, sizeof(hash));

        for (j = 0; j < nparams; j++) {
            char *key = fields[j];

            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);
            if (!strcmp("VmFlags", key))
                continue;

            unsigned long val = sums[j];

            _TOPIARY_DEBUG("%d,%d:val %lu\n", i, j, val);

            int n3 = n2;

            STRLCAT(hash, key, n3, sizeof(hash));
            _TOPIARY_DEBUG("%d,%d:hash '%s'\n", i, j, hash);

            _topiary_results_add_unsigned_long(results, hash, val);
        }
    }

    return TOPIARY_OK;
}

int
_topiary_proc_pid_smaps_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 3);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f", 1);   /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 9, "\n", 1);   /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 2, 4, ": ", 2);   /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
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

    char *filesuffix = "smaps";

    ret = _topiary_base_new(base, "proc_pid_smaps", _TOPIARY_BASE_TYPE_PREVIOUS,
                            NULL, proc_pid_smaps_normalize, NULL, 256, limits);
    if (!ret) {
        _topiary_base_set_fileprefix(*base, fileprefix);
        _topiary_base_set_fileglob(*base, fileglob);
        _topiary_base_set_filesuffix(*base, filesuffix);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
