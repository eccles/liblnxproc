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

typical contents of /proc/net/rpc/nfsd file::

rc 0 4 453
fh 0 0 0 0 0
io 0 0
th 8 0 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000
ra 32 0 0 0 0 0 0 0 0 0 0 0
net 457 1 456 3
rpc 457 0 0 0 0
proc2 18 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
proc3 22 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
proc4 2 1 455
proc4ops 59 0 0 0 1 0 0 0 0 0 18 3 0 0 0 0 2 0 0 0 0 0 0 15 0 3 0 0 0 0 0 435 0 0 0 0 2 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

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
proc_net_rpc_nfsd_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;
    _LNXPROC_VECTOR_T *vector = array->vector;

    static const char *nettitles[] =
        { "", "packets", "udp", "tcp", "tcpconn", };
    static const size_t nnettitles = sizeof(nettitles) / sizeof(nettitles[0]);
    static const char *rpctitles[] =
        { "", "calls", "badcalls", "badclnt", "badauth", "xdrcall", };
    static const size_t nrpctitles = sizeof(rpctitles) / sizeof(rpctitles[0]);

    static const char *ratitles[] = {
        "", "size", "deep10", "deep20", "deep30", "deep40", "deep50", "deep60",
        "deep70", "deep80", "deep90", "deep100", "notfound",
    };

    static const size_t nratitles = sizeof(ratitles) / sizeof(ratitles[0]);

    static const char *rctitles[] = {
        "", "hits", "misses", "nocache",
    };
    static const size_t nrctitles = sizeof(rctitles) / sizeof(rctitles[0]);

    static const char *iotitles[] = {
        "", "null", "compound",
    };
    static const size_t niotitles = sizeof(iotitles) / sizeof(iotitles[0]);

    static const char *thtitles[] = {
        "", "threads", "ntimesmax", "hist00", "hist10", "hist20", "hist30",
        "hist40", "hist50", "hist60", "hist70", "hist80", "hist90",
    };
    static const size_t nthtitles = sizeof(thtitles) / sizeof(thtitles[0]);

    static const char *fhtitles[] = {
        "", "lookup", "anon", "ncachedir", "ncachedir", "stale",
    };
    static const size_t nfhtitles = sizeof(fhtitles) / sizeof(fhtitles[0]);

    static const char *proc2titles[] = {
        "", "", "null", "getattr", "setattr", "root", "lookup", "readlink",
        "read",
        "wrcache", "write", "create", "remove", "rename", "link", "symlink",
        "mkdir", "rmdir", "readdir", "fsstat",
    };
    static const size_t nproc2titles =
        sizeof(proc2titles) / sizeof(proc2titles[0]);

    static const char *proc3titles[] = {
        "", "", "null", "getattr", "setattr", "lookup", "access", "readlink",
        "read",
        "write", "create", "kdir", "symlink", "mknod", "remove", "rmdir",
        "rename", "link", "readdir", "readdirplus", "fsstat", "fsinfo",
        "pathconf", "commit",
    };
    static const size_t nproc3titles =
        sizeof(proc3titles) / sizeof(proc3titles[0]);

    static const char *proc4titles[] = {
        "", "", "null", "compound",
    };
    static const size_t nproc4titles =
        sizeof(proc4titles) / sizeof(proc4titles[0]);

    static const char *proc4opstitles[] = {
        "", "", "op0-unused", "op1-unused", "op2-future", "access", "close",
        "commit",
        "create", "delegpurge", "delegreturn", "getattr", "getfh", "link",
        "lock", "lockt", "locku", "lookup", "lookup_root", "nverify",
        "open", "openattr", "open_conf", "open_dgrd", "putfh", "putpubfh",
        "putrootfh", "read", "readdir", "readlink", "remove", "rename",
        "renew", "restorefh", "savefh", "secinfo", "setattr", "setcltid",
        "setcltidconf", "verify", "write", "rellockowner", "bc_ctl",
        "bind_conn", "exchange_id", "create_ses", "destroy_ses",
        "free_stateid", "getdirdeleg", "getdevinfo", "getdevlist",
        "layoutcommit", "layoutget", "layoutreturn", "secinfononam",
        "sequence", "set_ssv", "test_stateid", "want_deleg", "destroy_clid",
        "reclaim_comp",
    };
    static const size_t nproc4opstitles =
        sizeof(proc4opstitles) / sizeof(proc4opstitles[0]);

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

    char *rowkey;

    int i, j;

    _lnxproc_results_init(results, nrows);

    for (i = 0; i < nrows; i++) {
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
            if (!strcmp(rowkey, "net")) {
                if (j >= nnettitles)
                    continue;
                STRLCAT(buf, nettitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "rpc")) {
                if (j >= nrpctitles)
                    continue;
                STRLCAT(buf, rpctitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "ra")) {
                if (j >= nratitles)
                    continue;
                STRLCAT(buf, ratitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "rc")) {
                if (j >= nrctitles)
                    continue;
                STRLCAT(buf, rctitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "io")) {
                if (j >= niotitles)
                    continue;
                STRLCAT(buf, iotitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "th")) {
                if (j >= nthtitles)
                    continue;
                STRLCAT(buf, thtitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "fh")) {
                if (j >= nfhtitles)
                    continue;
                STRLCAT(buf, fhtitles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc2")) {
                if (j >= nproc2titles || j < 2)
                    continue;
                STRLCAT(buf, proc2titles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc3")) {
                if (j >= nproc3titles || j < 2)
                    continue;
                STRLCAT(buf, proc3titles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc4")) {
                if (j >= nproc4titles || j < 2)
                    continue;
                STRLCAT(buf, proc4titles[j], n, sizeof(buf));
            }
            else if (!strcmp(rowkey, "proc4ops")) {
                if (j >= nproc4opstitles || j < 2)
                    continue;
                STRLCAT(buf, proc4opstitles[j], n, sizeof(buf));
            }
            else {
                _LNXPROC_DEBUG("%d,%d:WARN rowkey '%s'\n", i, j, rowkey);

                continue;
            }
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
_lnxproc_proc_net_rpc_nfsd_new(_LNXPROC_BASE_T ** base, void *optional)
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
    ret = _lnxproc_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/rpc/nfsd" };
    ret =
        _lnxproc_base_new(base, "proc_net_rpc_nfsd",
                          _LNXPROC_BASE_TYPE_PREVIOUS, filenames, 1, NULL, NULL,
                          NULL, NULL, proc_net_rpc_nfsd_normalize, NULL, 256,
                          limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */