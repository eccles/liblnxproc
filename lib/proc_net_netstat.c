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

typical contents of /proc/net/netstat file::

TcpExt: SyncookiesSent SyncookiesRecv SyncookiesFailed EmbryonicRsts PruneCalled RcvPruned OfoPruned OutOfWindowIcmps LockDroppedIcmps ArpFilter TW TWRecycled TWKilled PAWSPassive PAWSActive PAWSEstab DelayedACKs DelayedACKLocked DelayedACKLost ListenOverflows ListenDrops TCPPrequeued TCPDirectCopyFromBacklog TCPDirectCopyFromPrequeue TCPPrequeueDropped TCPHPHits TCPHPHitsToUser TCPPureAcks TCPHPAcks TCPRenoRecovery TCPSackRecovery TCPSACKReneging TCPFACKReorder TCPSACKReorder TCPRenoReorder TCPTSReorder TCPFullUndo TCPPartialUndo TCPDSACKUndo TCPLossUndo TCPLostRetransmit TCPRenoFailures TCPSackFailures TCPLossFailures TCPFastRetrans TCPForwardRetrans TCPSlowStartRetrans TCPTimeouts TCPRenoRecoveryFail TCPSackRecoveryFail TCPSchedulerFailed TCPRcvCollapsed TCPDSACKOldSent TCPDSACKOfoSent TCPDSACKRecv TCPDSACKOfoRecv TCPAbortOnData TCPAbortOnClose TCPAbortOnMemory TCPAbortOnTimeout TCPAbortOnLinger TCPAbortFailed TCPMemoryPressures TCPSACKDiscard TCPDSACKIgnoredOld TCPDSACKIgnoredNoUndo TCPSpuriousRTOs TCPMD5NotFound TCPMD5Unexpected TCPSackShifted TCPSackMerged TCPSackShiftFallback TCPBacklogDrop TCPMinTTLDrop TCPDeferAcceptDrop IPReversePathFilter TCPTimeWaitOverflow TCPReqQFullDoCookies TCPReqQFullDrop TCPRetransFail TCPRcvCoalesce TCPOFOQueue TCPOFODrop TCPOFOMerge TCPChallengeACK TCPSYNChallenge TCPFastOpenActive TCPFastOpenPassive TCPFastOpenPassiveFail TCPFastOpenListenOverflow TCPFastOpenCookieReqd
TcpExt: 0 0 0 0 0 0 0 0 0 0 766 0 0 0 0 0 1028 0 11 0 0 99 9296 26034 0 176424 173 9975 161316 0 0 0 0 0 0 0 0 0 0 4 0 0 0 0 0 0 0 5 0 0 0 0 11 0 3 0 9 58 0 0 0 0 0 0 0 0 0 0 0 0 0 225 0 0 1 0 0 0 0 0 7759 116 0 0 3 3 0 0 0 0 0
IpExt: InNoRoutes InTruncatedPkts InMcastPkts OutMcastPkts InBcastPkts OutBcastPkts InOctets OutOctets InMcastOctets OutMcastOctets InBcastOctets OutBcastOctets
IpExt: 2 0 237 65 1242 73 117712822 103374286 35236 8641 121278 9694
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
proc_net_netstat_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;
    _TOPIARY_VECTOR_T *vector = array->vector;

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
    char *key;
    char *val;

    char *rowkey1;
    char *rowkey2;

    int i, j;

    _topiary_results_init(results, nrows);

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    for (i = 0; i < nrows; i += 2) {
        char **value1 = (char **) values[i];
        char **value2 = (char **) values[i + 1];
        size_t ncols1 = vector->children[i]->length;
        size_t ncols2 = vector->children[i + 1]->length;

        _TOPIARY_DEBUG("%d:Ncols1 %zd\n", i, ncols1);
        _TOPIARY_DEBUG("%d:Ncols2 %zd\n", i, ncols2);

        rowkey1 = value1[0];
        if (!rowkey1)
            continue;

        _TOPIARY_DEBUG("%d:rowkey1 '%s'\n", i, rowkey1);

        rowkey2 = value2[0];
        if (!rowkey2)
            continue;

        _TOPIARY_DEBUG("%d:rowkey2 '%s'\n", i, rowkey2);

        if (strcmp(rowkey1, rowkey2))
            continue;

        int n2 = n1;

        STRLCAT(buf, rowkey1, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 1; j < ((ncols1 > ncols2) ? ncols2 : ncols1); j++) {
            key = value1[j];
            if (!key)
                continue;
            _TOPIARY_DEBUG("%d,%d:Key '%s'\n", i, j, key);

            val = value2[j];
            if (!val)
                continue;
            _TOPIARY_DEBUG("%d,%d:Val '%s'\n", i, j, val);

            int n3 = n2;

            STRLCAT(buf, key, n3, sizeof(buf));
            _TOPIARY_DEBUG("%d,%d:hashkey '%s'\n", i, j, buf);
            unsigned long current = atoi(val);

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
                _topiary_results_add_fixed(results, buf, rate, 0, 2);
            }
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_net_netstat_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
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
    ret = _topiary_limits_set(limits, 1, 4, ": ", 3);   /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/netstat" };
    ret =
        _topiary_base_new(base, "proc_net_netstat", _TOPIARY_BASE_TYPE_PREVIOUS,
                          NULL, proc_net_netstat_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
