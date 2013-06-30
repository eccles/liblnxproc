/*
This file is part of topiary.

 topiary is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 topiary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with topiary.  If not, see <http://www.gnu.org/licenses/>.

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
proc_pid_status_normalize(_TOPIARY_BASE_T *base)
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

    int i, j, k;
    char hash[64];

    int n1 = 0;

    STRLCAT(hash, "/", n1, sizeof(hash));

    _topiary_results_init(results, npids);
    for (i = 0; i < npids; i++) {
        char ***value1 = (char ***) values[i];
        char *pidkey = value1[0][0];

        if (!pidkey)
            continue;

        int n2 = n1;

        STRLCAT(hash, pidkey, n2, sizeof(hash));
        STRLCAT(hash, "/", n2, sizeof(hash));

        _TOPIARY_VECTOR_T *child = vector->children[i];
        size_t nrows = child->length;

        _TOPIARY_DEBUG("%d:pidkey '%s'\n", i, pidkey);
        _TOPIARY_DEBUG("%d:nrows = %zd\n", i, nrows);
        for (j = 1; j < nrows; j++) {
            char **value2 = (char **) value1[j];

            char *key = value2[0];

            if (!key)
                continue;
            _TOPIARY_DEBUG("%d,%d:key '%s'\n", i, j, key);

            _TOPIARY_VECTOR_T *grandchild = child->children[j];
            size_t ncols = grandchild->length;

            _TOPIARY_DEBUG("%d,%d:ncols = %zd\n", i, j, ncols);

            int n3 = n2;

            STRLCAT(hash, key, n3, sizeof(hash));

            if (!strcmp(key, "Name") ||
                !strcmp(key, "State") ||
                !strncmp(key, "Cpus_allowed", 12) ||
                !strncmp(key, "Mems_allowed", 12) ||
                !strncmp(key, "Cap", 3) ||
                !strncmp(key, "Shd", 3) || !strncmp(key, "Sig", 3)) {
                _TOPIARY_DEBUG("%d,%d:hash key '%s'\n", i, j, hash);
                char *val = value2[1];

                if (!val)
                    continue;
                _TOPIARY_DEBUG("%d,%d:val '%s'\n", i, j, val);
                _topiary_results_add_stringref(results, hash, val);
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
                _TOPIARY_DEBUG("%d,%d:hash key '%s'\n", i, j, hash);
                char *val = value2[1];

                if (!val)
                    continue;
                _TOPIARY_DEBUG("%d,%d:val '%s'\n", i, j, val);
                _topiary_results_add_int(results, hash, atoi(val));
            }
            else if (!strcmp(key, "Uid")) {
                static const char *uidtitle[] =
                    { "Uid", "Euid", "Suid", "Fuid", };
                for (k = 1; k < ncols; k++) {

                    int n3 = n2;

                    STRLCAT(hash, uidtitle[k - 1], n3, sizeof(hash));
                    _TOPIARY_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, hash);
                    char *val = value2[k];

                    if (!val)
                        continue;
                    _TOPIARY_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _topiary_results_add_int(results, hash, atoi(val));
                }

            }
            else if (!strcmp(key, "Gid")) {
                static const char *gidtitle[] =
                    { "Gid", "Egid", "Sgid", "Fgid", };
                for (k = 1; k < ncols; k++) {

                    int n3 = n2;

                    STRLCAT(hash, gidtitle[k - 1], n3, sizeof(hash));
                    _TOPIARY_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, hash);
                    char *val = value2[k];

                    if (!val)
                        continue;
                    _TOPIARY_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _topiary_results_add_int(results, hash, atoi(val));
                }

            }
            else if (!strcmp(key, "Groups")) {
                for (k = 1; k < ncols; k++) {
                    int n4 = n3;

                    STRLCAT(hash, "/", n4, sizeof(hash));
                    INTCAT(hash, k - 1, n4, sizeof(hash));
                    _TOPIARY_DEBUG("%d,%d,%d:hash key '%s'\n", i, j, k, hash);
                    char *val = value2[k];

                    if (!val)
                        continue;
                    _TOPIARY_DEBUG("%d,%d,%d:val '%s'\n", i, j, k, val);
                    _topiary_results_add_int(results, hash, atoi(val));
                }

            }
            else {
                /* unknown fields - should never execute this section */
                for (k = 1; k < ncols; k++) {
                    int n4 = n3;

                    STRLCAT(hash, "/", n4, sizeof(hash));
                    INTCAT(hash, k - 1, n4, sizeof(hash));

                    _TOPIARY_DEBUG("%d,%d,%d:WARN hash key '%s'\n", i, j, k,
                                   hash);

                    char *val = value2[k];

                    if (!val)
                        continue;
                    _TOPIARY_DEBUG("%d,%d,%d:WARN val '%s'\n", i, j, k, val);
                    //_topiary_results_add_stringref(results, buf, val);
                }
            }
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_pid_status_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 3);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f", 1);   /* row delimiters (per pid) */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 2, "\n", 1);   /* column delimiter */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 2, 2, " \t:", 3);
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

    char *filesuffix = "status";

    ret =
        _topiary_base_new(base, "proc_pid_status", _TOPIARY_BASE_TYPE_VANILLA,
                          NULL, proc_pid_status_normalize, NULL, 256, limits);
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
