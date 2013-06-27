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

typical contents of /proc/net/snmp6 file::

Ip6InReceives                   	1240
Ip6InHdrErrors                  	0
Ip6InTooBigErrors               	0
Ip6InNoRoutes                   	0
Ip6InAddrErrors                 	0
Ip6InUnknownProtos              	0
Ip6InTruncatedPkts              	0
Ip6InDiscards                   	0
Ip6InDelivers                   	268
Ip6OutForwDatagrams             	0
Ip6OutRequests                  	183
Ip6OutDiscards                  	0
Ip6OutNoRoutes                  	25
Ip6ReasmTimeout                 	0
Ip6ReasmReqds                   	0
Ip6ReasmOKs                     	0
Ip6ReasmFails                   	0
Ip6FragOKs                      	0
Ip6FragFails                    	0
Ip6FragCreates                  	0
Ip6InMcastPkts                  	1218
Ip6OutMcastPkts                 	178
Ip6InOctets                     	415606
Ip6OutOctets                    	25974
Ip6InMcastOctets                	414066
Ip6OutMcastOctets               	25786
Ip6InBcastOctets                	0
Ip6OutBcastOctets               	0
Icmp6InMsgs                     	0
Icmp6InErrors                   	0
Icmp6OutMsgs                    	37
Icmp6OutErrors                  	0
Icmp6InDestUnreachs             	0
Icmp6InPktTooBigs               	0
Icmp6InTimeExcds                	0
Icmp6InParmProblems             	0
Icmp6InEchos                    	0
Icmp6InEchoReplies              	0
Icmp6InGroupMembQueries         	0
Icmp6InGroupMembResponses       	0
Icmp6InGroupMembReductions      	0
Icmp6InRouterSolicits           	0
Icmp6InRouterAdvertisements     	0
Icmp6InNeighborSolicits         	0
Icmp6InNeighborAdvertisements   	0
Icmp6InRedirects                	0
Icmp6InMLDv2Reports             	0
Icmp6OutDestUnreachs            	0
Icmp6OutPktTooBigs              	0
Icmp6OutTimeExcds               	0
Icmp6OutParmProblems            	0
Icmp6OutEchos                   	0
Icmp6OutEchoReplies             	0
Icmp6OutGroupMembQueries        	0
Icmp6OutGroupMembResponses      	0
Icmp6OutGroupMembReductions     	0
Icmp6OutRouterSolicits          	15
Icmp6OutRouterAdvertisements    	0
Icmp6OutNeighborSolicits        	5
Icmp6OutNeighborAdvertisements  	0
Icmp6OutRedirects               	0
Icmp6OutMLDv2Reports            	17
Icmp6OutType133                 	15
Icmp6OutType135                 	5
Icmp6OutType143                 	17
Udp6InDatagrams                 	243
Udp6NoPorts                     	0
Udp6InErrors                    	0
Udp6OutDatagrams                	124
Udp6RcvbufErrors                	0
Udp6SndbufErrors                	0
UdpLite6InDatagrams             	0
UdpLite6NoPorts                 	0
UdpLite6InErrors                	0
UdpLite6OutDatagrams            	0
UdpLite6RcvbufErrors            	0
UdpLite6SndbufErrors            	0
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
proc_net_snmp6_normalize(_TOPIARY_BASE_T *base)
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

    int i;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _topiary_results_init(results, nrows);

    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];

        key = value1[0];
        if (!key)
            continue;

        _TOPIARY_DEBUG("%d:key '%s'\n", i, key);

        val = value1[1];
        if (!val)
            continue;

        _TOPIARY_DEBUG("%d:val '%s'\n", i, val);

        int n2 = n1;

        STRLCAT(buf, key, n2, sizeof(buf));
        _TOPIARY_DEBUG("%d:hashkey '%s'\n", i, buf);
        unsigned long current = strtoul(val, NULL, 0);

        _topiary_results_add_unsigned_long(results, buf, current);
        if (tdiff > 0.0) {
            _TOPIARY_RESULTS_TABLE_T *pentry = NULL;

            int ret = _topiary_results_fetch(presults, buf, &pentry);

            if (ret)
                continue;
            _TOPIARY_DEBUG("%d:previous value for %s is %lu\n", i,
                           buf, pentry->value.ul);
            float rate = (current - pentry->value.ul) / tdiff;

            _TOPIARY_DEBUG("%d:rate = %f\n", i, rate);
            STRLCAT(buf, "-s", n2, sizeof(buf));
            _TOPIARY_DEBUG("%d:hashkey '%s'\n", i, buf);
            _topiary_results_add_fixed(results, buf, rate, 0, 2);
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_net_snmp6_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
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
    ret = _topiary_limits_set(limits, 1, 4, "\t ", 2);  /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/net/snmp6" };
    ret = _topiary_base_new(base, "proc_net_snmp6", _TOPIARY_BASE_TYPE_PREVIOUS,
                            NULL, proc_net_snmp6_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
