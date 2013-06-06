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

typical contents of /proc/net/snmp file::

Ip: Forwarding DefaultTTL InReceives InHdrErrors InAddrErrors ForwDatagrams InUnknownProtos InDiscards InDelivers OutRequests OutDiscards OutNoRoutes ReasmTimeout ReasmReqds ReasmOKs ReasmFails FragOKs FragFails FragCreates
Ip: 1 64 377637 0 0 0 0 0 377547 375230 60 1 0 0 0 0 0 0 0
Icmp: InMsgs InErrors InDestUnreachs InTimeExcds InParmProbs InSrcQuenchs InRedirects InEchos InEchoReps InTimestamps InTimestampReps InAddrMasks InAddrMaskReps OutMsgs OutErrors OutDestUnreachs OutTimeExcds OutParmProbs OutSrcQuenchs OutRedirects OutEchos OutEchoReps OutTimestamps OutTimestampReps OutAddrMasks OutAddrMaskReps
Icmp: 120 0 120 0 0 0 0 0 0 0 0 0 0 120 0 120 0 0 0 0 0 0 0 0 0 0
IcmpMsg: InType3 OutType3
IcmpMsg: 120 120
Tcp: RtoAlgorithm RtoMin RtoMax MaxConn ActiveOpens PassiveOpens AttemptFails EstabResets CurrEstab InSegs OutSegs RetransSegs InErrs OutRsts
Tcp: 1 200 120000 -1 1172 487 30 50 21 262227 261044 5 3 110
Udp: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors
Udp: 103170 120 12373 114370 7732 0
UdpLite: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors
UdpLite: 0 0 0 0 0 0

*/

#include <stdlib.h>
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
proc_net_snmp_normalize(_LNXPROC_BASE_T * base)
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
    char *key;
    char *val;

    char *rowkey1;
    char *rowkey2;

    int i, j;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _lnxproc_results_init(results, nrows);

    for (i = 0; i < nrows; i += 2) {
        char **value1 = (char **) values[i];
        char **value2 = (char **) values[i + 1];
        size_t ncols1 = vector->children[i]->length;
        size_t ncols2 = vector->children[i + 1]->length;

        _LNXPROC_DEBUG("%d:Ncols1 %zd\n", i, ncols1);
        _LNXPROC_DEBUG("%d:Ncols2 %zd\n", i, ncols2);

        rowkey1 = value1[0];
        if (!rowkey1)
            continue;

        _LNXPROC_DEBUG("%d:rowkey1 '%s'\n", i, rowkey1);

        rowkey2 = value2[0];
        if (!rowkey2)
            continue;

        _LNXPROC_DEBUG("%d:rowkey2 '%s'\n", i, rowkey2);

        if (strcmp(rowkey1, rowkey2))
            continue;

        int n2 = n1;

        STRLCAT(buf, rowkey1, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 1; j < ((ncols1 > ncols2) ? ncols2 : ncols1); j++) {
            key = value1[j];
            if (!key)
                continue;
            _LNXPROC_DEBUG("%d,%d:Key '%s'\n", i, j, key);

            val = value2[j];
            if (!val)
                continue;
            _LNXPROC_DEBUG("%d,%d:Val '%s'\n", i, j, val);

            int n3 = n2;

            STRLCAT(buf, key, n3, sizeof(buf));
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
                STRLCAT(buf, "-s", n3, sizeof(buf));
                _LNXPROC_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
                _lnxproc_results_add_float(results, buf, rate);
            }
        }
    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_net_snmp_new(_LNXPROC_BASE_T ** base, LNXPROC_OPT_T * optional)
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
    ret = _lnxproc_limits_set(limits, 1, 4, ": ", 2);   /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/snmp" };
    ret = _lnxproc_base_new(base, "proc_net_snmp", _LNXPROC_BASE_TYPE_PREVIOUS,
                            NULL, proc_net_snmp_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _lnxproc_base_set_filenames(*base, filenames, 1);
    }
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
