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

typical contents of /proc/net/rpc/nfs file::

    net 0 0 0 0
    rpc 0 0 0
    proc2 18 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    proc3 22 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    proc4 37 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

*/

#include "config.h"

#include "topiary_stdlib.h"
#include <stdio.h>
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
proc_net_rpc_nfs_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;
    _TOPIARY_VECTOR_T *vector = array->vector;

    static const char *nettitles[] =
        { "", "packets", "udp", "tcp", "tcpconn", };
    static const size_t nnettitles = sizeof(nettitles) / sizeof(nettitles[0]);
    static const char *rpctitles[] = { "", "calls", "retrans", "authrefrsh", };
    static const size_t nrpctitles = sizeof(rpctitles) / sizeof(rpctitles[0]);

    static const char *proc2titles[] =
        { "", "", "null", "getattr", "setattr", "root", "lookup", "readlink",
        "read", "wrcache", "write", "create", "remove", "rename",
        "link", "symlink", "mkdir", "rmdir", "readdir", "fsstat",
    };
    static const size_t nproc2titles =
        sizeof(proc2titles) / sizeof(proc2titles[0]);
    static const char *proc3titles[] =
        { "", "", "null", "getattr", "setattr", "lookup", "access", "readlink",
        "read", "write", "create", "mkdir", "symlink", "mknod",
        "remove", "rmdir", "rename", "link", "readdir", "readdirplus",
        "fsstat", "fsinfo", "pathconf", "commit",
    };
    static const size_t nproc3titles =
        sizeof(proc3titles) / sizeof(proc3titles[0]);
    static const char *proc4titles[] =
        { "", "", "null", "read", "write", "commit", "open", "open_conf",
        "open_noat", "open_dgrd", "close", "setattr", "fsinfo", "renew",
        "setclntid", "confirm", "lock", "lockt", "locku", "access",
        "getattr", "lookup", "lookup_root", "remove", "rename", "link",
        "symlink", "create", "pathconf", "statfs", "readlink", "readdir",
        "server_caps", "delegreturn", "getacl", "setacl", "fs_locations",
        "rel_lkowner",
        "secinfo", "exchange_id", "create_ses", "destroy_ses", "sequence",
        "get_lease_t",
        "reclaim_comp", "layoutget", "getdevinfo", "layoutcommit",
        "layoutreturn", "getdevlist",
    };
    static const size_t nproc4titles =
        sizeof(proc4titles) / sizeof(proc4titles[0]);

    _TOPIARY_BASE_DATA_T *pdata = base->previous;
    _TOPIARY_RESULTS_T *presults = NULL;

    float tdiff = 0.0;

    if (pdata && pdata->array) {
        _topiary_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _TOPIARY_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _TOPIARY_DEBUG("Previous results is at %p\n", presults);
    }

    _TOPIARY_DEBUG("Time difference = %f secs\n", tdiff);

    size_t nrows = vector->length;

    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) vector->values;
    char *val;

    char *rowkey;

    int i, j;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _topiary_results_init(results, nrows);

    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        size_t ncols = vector->children[i]->length;

        _TOPIARY_DEBUG("%d:Ncols %zd\n", i, ncols);

        rowkey = value1[0];
        if (!rowkey)
            continue;

        _TOPIARY_DEBUG("%d:rowkey '%s'\n", i, rowkey);

        int n2 = n1;

        STRLCAT(buf, rowkey, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 1; j < ncols; j++) {
            val = value1[j];
            if (!val)
                continue;
            _TOPIARY_DEBUG("%d,%d:Val '%s'\n", i, j, val);

            int n3 = n2;

            if (!strcmp(rowkey, "net")) {
                if (j >= nnettitles)
                    continue;
                STRLCAT(buf, nettitles[j], n3, sizeof(buf));
            }
            else if (!strcmp(rowkey, "rpc")) {
                if (j >= nrpctitles)
                    continue;
                STRLCAT(buf, rpctitles[j], n3, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc2")) {
                if (j >= nproc2titles || j < 2)
                    continue;
                STRLCAT(buf, proc2titles[j], n3, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc3")) {
                if (j >= nproc3titles || j < 2)
                    continue;
                STRLCAT(buf, proc3titles[j], n3, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc4")) {
                if (j >= nproc4titles || j < 2)
                    continue;
                STRLCAT(buf, proc4titles[j], n3, sizeof(buf));
            }
            else {
                continue;
            }
            _TOPIARY_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
            unsigned long current = strtoul(val, NULL, 0);

            _topiary_results_add_unsigned_long(results, buf, current);

            if (tdiff > 0.0) {
                _TOPIARY_RESULTS_TABLE_T *pentry = NULL;

                int ret = _topiary_results_fetch(presults, buf, &pentry);

                if (ret)
                    continue;
                _TOPIARY_DEBUG("%d,%d:previous value for %s is %lu\n", i, j,
                               buf, pentry->value.ul);
                float rate = (current - pentry->value.ul) / tdiff;

                _TOPIARY_DEBUG("%d,%d:rate = %f\n", i, j, rate);
                STRLCAT(buf, "-s", n3, sizeof(buf));
                _TOPIARY_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
                _topiary_results_add_fixed(results, buf, rate, 0, 1);
            }

        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_net_rpc_nfs_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 5, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/rpc/nfs" };
    ret =
        _topiary_base_new(base, "proc_net_rpc_nfs", _TOPIARY_BASE_TYPE_PREVIOUS,
                          NULL, proc_net_rpc_nfs_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
