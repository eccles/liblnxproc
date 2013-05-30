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

Name:	bash
State:	S (sleeping)
Tgid:	4114
Pid:	4114
PPid:	4024
TracerPid:	0
Uid:	1000	1000	1000	1000
Gid:	1000	1000	1000	1000
FDSize:	256
Groups:	4 24 27 29 30 46 107 124 133 134 1000 
VmPeak:	   23652 kB
VmSize:	   23588 kB
VmLck:	       0 kB
VmPin:	       0 kB
VmHWM:	    4932 kB
VmRSS:	    4928 kB
VmData:	    2900 kB
VmStk:	     136 kB
VmExe:	     900 kB
VmLib:	    2296 kB
VmPTE:	      68 kB
VmSwap:	       0 kB
Threads:	1
SigQ:	0/127852
SigPnd:	0000000000000000
ShdPnd:	0000000000000000
SigBlk:	0000000000010000
SigIgn:	0000000000384004
SigCgt:	000000004b813efb
CapInh:	0000000000001000
CapPrm:	0000000000000000
CapEff:	0000000000000000
CapBnd:	0000001fffffffff
Seccomp:	0
Cpus_allowed:	ff
Cpus_allowed_list:	0-7
Mems_allowed:	00000000,00000001
Mems_allowed_list:	0
voluntary_ctxt_switches:	7876
nonvoluntary_ctxt_switches:	399
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
proc_pid_status_normalize(_LNXPROC_BASE_T * base)
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

    _lnxproc_results_init(results, npids);
    for (i = 0; i < npids; i++) {
        char *pidkey = values[i][0][0];

        if (!pidkey)
            continue;

        _LNXPROC_VECTOR_T *child = vector->children[i];
        size_t nrows = child->length;

        _LNXPROC_DEBUG("%d:pidkey '%s'\n", i, pidkey);
        _LNXPROC_DEBUG("%d:nrows = %zd\n", i, nrows);
        for (j = 1; j < nrows; j++) {

            char *key = values[i][j][0];

            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:key '%s'\n", i, j, key);

            _LNXPROC_VECTOR_T *grandchild = child->children[j];
            size_t ncols = grandchild->length;

            _LNXPROC_DEBUG("%d,%d:ncols = %zd\n", i, j, ncols);

            char basehash[64];

            int n = 0;

            STRLCAT(basehash, "/", n, sizeof(basehash));
            STRLCAT(basehash, pidkey, n, sizeof(basehash));

            char basehash1[64];

            n = 0;
            STRLCAT(basehash1, basehash, n, sizeof(basehash1));
            STRLCAT(basehash1, "/", n, sizeof(basehash1));
            STRLCAT(basehash1, key, n, sizeof(basehash1));

            if (!strcmp(key, "Name") ||
                !strcmp(key, "State") ||
                !strncmp(key, "Cpus_allowed", 12) ||
                !strncmp(key, "Mems_allowed", 12) ||
                !strncmp(key, "Cap", 3) ||
                !strncmp(key, "Shd", 3) || !strncmp(key, "Sig", 3)) {
                _LNXPROC_DEBUG("%d,%d:hash key '%s'\n", i, j, basehash1);
                char *val = values[i][j][1];

                if (!val)
                    continue;
                _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, val);
                _lnxproc_results_add_stringref(results, basehash1, val);
            }
            else if (!strcmp(key, "Tgid") ||
                     !strcmp(key, "TracerPid") ||
                     !strcmp(key, "FDSize") ||
                     !strcmp(key, "Pid") ||
                     !strcmp(key, "PPid") ||
                     !strcmp(key, "Threads") ||
                     !strcmp(key, "Seccomp") ||
                     !strcmp(key, "voluntary_ctxt_switches") ||
                     !strcmp(key, "nonvoluntary_ctxt_switches") ||
                     !strncmp(key, "Vm", 2)) {
                _LNXPROC_DEBUG("%d,%d:hash key '%s'\n", i, j, basehash1);
                char *val = values[i][j][1];

                if (!val)
                    continue;
                _LNXPROC_DEBUG("%d,%d:val '%s'\n", i, j, val);
                _lnxproc_results_add_int(results, basehash1, atoi(val));
            }
            else if (!strcmp(key, "Uid")) {
                static const char *uidtitle[] =
                    { "Uid", "Euid", "Suid", "Fuid", };
                for (k = 1; k < ncols; k++) {
                    char buf[64];

                    n = 0;

                    STRLCAT(buf, basehash, n, sizeof(buf));
                    STRLCAT(buf, "/", n, sizeof(buf));
                    STRLCAT(buf, uidtitle[k - 1], n, sizeof(buf));
                    _LNXPROC_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, buf);
                    char *val = values[i][j][k];

                    if (!val)
                        continue;
                    _LNXPROC_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _lnxproc_results_add_int(results, buf, atoi(val));
                }

            }
            else if (!strcmp(key, "Gid")) {
                static const char *gidtitle[] =
                    { "Gid", "Egid", "Sgid", "Fgid", };
                for (k = 1; k < ncols; k++) {
                    char buf[64];

                    n = 0;

                    STRLCAT(buf, basehash, n, sizeof(buf));
                    STRLCAT(buf, "/", n, sizeof(buf));
                    STRLCAT(buf, gidtitle[k - 1], n, sizeof(buf));
                    _LNXPROC_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, buf);
                    char *val = values[i][j][k];

                    if (!val)
                        continue;
                    _LNXPROC_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _lnxproc_results_add_int(results, buf, atoi(val));
                }

            }
            else if (!strcmp(key, "Groups")) {
                for (k = 1; k < ncols; k++) {
                    char buf[64];

                    n = 0;

                    STRLCAT(buf, basehash1, n, sizeof(buf));
                    STRLCAT(buf, "/", n, sizeof(buf));
                    INTCAT(buf, k - 1, n, sizeof(buf));
                    _LNXPROC_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, buf);
                    char *val = values[i][j][k];

                    if (!val)
                        continue;
                    _LNXPROC_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _lnxproc_results_add_int(results, buf, atoi(val));
                }

            }
            else {
                /* unknown fields - should never execute this section */
                for (k = 1; k < ncols; k++) {
                    char buf[64];

                    n = 0;

                    STRLCAT(buf, basehash1, n, sizeof(buf));
                    STRLCAT(buf, "/", n, sizeof(buf));
                    INTCAT(buf, k - 1, n, sizeof(buf));

                    _LNXPROC_DEBUG("%d,%d,%d:WARN hash key '%s'\n", i, j, k,
                                   buf);

                    char *val = values[i][j][k];

                    if (!val)
                        continue;
                    _LNXPROC_DEBUG("%d,%d,%d:WARN val '%s'\n", i, j, k, val);
                    //_lnxproc_results_add_stringref(results, buf, val);
                }
            }
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_pid_status_new(_LNXPROC_BASE_T ** base, void *optional)
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
    ret = _lnxproc_limits_set(limits, 2, 2, " \t:", 3);
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

    char *filesuffix = "status";

    ret =
        _lnxproc_base_new(base, "proc_pid_status", _LNXPROC_BASE_TYPE_VANILLA,
                          NULL, 0, fileprefix, fileglob, filesuffix, NULL,
                          proc_pid_status_normalize, NULL, 256, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
