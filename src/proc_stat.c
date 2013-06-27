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

typical contents of /proc/stat file::

cpu  4707250 3894 1143360 197492176 1250162 8 18846 0 836808 0
cpu0 914342 1228 218053 23804312 281075 3 5880 0 140876 0
cpu1 759828 571 215720 24004906 256728 1 5021 0 138154 0
cpu2 721924 581 216588 24047212 256886 0 3242 0 145614 0
cpu3 814522 1270 217624 23853656 349317 2 4246 0 144459 0
cpu4 225346 22 67786 25597542 23599 0 101 0 50869 0
cpu5 437286 10 66696 25380254 27614 0 142 0 67572 0
cpu6 389070 198 76362 25394403 26056 0 107 0 83969 0
cpu7 444929 11 64528 25409889 28883 0 105 0 65293 0
intr 933015237 16 3 0 0 0 0 0 0 1 0 0 0 0 0 0 0 1022861 895 0 0 0 0 0 539355 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 29 9531687 1167850 12 464945 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
ctxt 2438742500
btime 1369575822
processes 224347
procs_running 6
procs_blocked 0
softirq 90796022 0 21051565 44290 15033485 9523416 0 1316048 21055421 165439 22606358

       /proc/stat
              kernel/system statistics.  Varies with architecture.  Common entries include:

              cpu  3357 0 4313 1362393
                     The  amount  of  time,  measured in units of USER_HZ (1/100ths of a second on most architectures, use sysconf(_SC_CLK_TCK) to
                     obtain the right value), that the system spent in user mode, user mode with low priority (nice), system mode,  and  the  idle
                     task, respectively.  The last value should be USER_HZ times the second entry in the uptime pseudo-file.

                     In  Linux  2.6  this  line includes three additional columns: iowait - time waiting for I/O to complete (since 2.5.41); irq -
                     time servicing interrupts (since 2.6.0-test4); softirq - time servicing softirqs (since 2.6.0-test4).

                     Since Linux 2.6.11, there is an eighth column, steal - stolen time, which is the time spent in other operating  systems  when
                     running in a virtualized environment

                     Since Linux 2.6.24, there is a ninth column, guest, which is the time spent running a virtual CPU for guest operating systems
                     under the control of the Linux kernel.

              page 5741 1808
                     The number of pages the system paged in and the number that were paged out (from disk).

              swap 1 0
                     The number of swap pages that have been brought in and out.

              intr 1462898
                     This line shows counts of interrupts serviced since boot time, for each of the possible system interrupts.  The first  column
                     is the total of all interrupts serviced; each subsequent column is the total for a particular interrupt.

              disk_io: (2,0):(31,30,5764,1,2) (3,0):...
                     (major,disk_idx):(noinfo, read_io_ops, blks_read, write_io_ops, blks_written)
                     (Linux 2.4 only)

              ctxt 115315
                     The number of context switches that the system underwent.

              btime 769041601
                     boot time, in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).

              processes 86031
                     Number of forks since boot.

              procs_running 6
                     Number of processes in runnable state.  (Linux 2.5.45 onward.)

              procs_blocked 2
                     Number of processes blocked waiting for I/O to complete.  (Linux 2.5.45 onward.)


*/

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
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
proc_stat_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *key;
    char *val;

    int i, j;

    static const char *cputitles[] =
        { "user", "nice", "system", "idle", "iowait", "irq", "softirq", "steal",
        "guest", "unknown",
    };
    static const size_t ncputitles = sizeof(cputitles) / sizeof(cputitles[0]);
    static const char *swaptitles[] = { "swapin", "swapout", };
    static const size_t nswaptitles =
        sizeof(swaptitles) / sizeof(swaptitles[0]);
    static const char *pagetitles[] = { "pagein", "pageout", };
    static const size_t npagetitles =
        sizeof(pagetitles) / sizeof(pagetitles[0]);

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _topiary_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        size_t ncols = array->vector->children[i]->length;

        _TOPIARY_DEBUG("%d:Ncols %zd\n", i, ncols);

        key = value1[0];
        if (!key)
            continue;
        _TOPIARY_DEBUG("%d:Key '%s'\n", i, key);

        int n2 = n1;

        STRLCAT(buf, key, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 1; j < ncols; j++) {
            val = value1[j];

            if (!val)
                continue;

            _TOPIARY_DEBUG("%d,%d:Val '%s'\n", i, j, val);

            int n3 = n2;

            if (!strncmp(key, "cpu", 3)) {
                if (j > ncputitles) {
                    INTCAT(buf, j - 1, n3, sizeof(buf));
                }
                else {
                    STRLCAT(buf, cputitles[j - 1], n3, sizeof(buf));
                }
                _TOPIARY_DEBUG("%d,%d:hashKey %s\n", i, j, buf);
                float t = strtoul(val, NULL, 0) * results->secs_per_jiffy;

                _TOPIARY_DEBUG("%d,%d:value %f\n", i, j, t);
                _topiary_results_add_fixed(results, buf, t, 0, 2);
            }
            else if (!strcmp(key, "intr")) {
                INTCAT(buf, j - 1, n3, sizeof(buf));
                _TOPIARY_DEBUG("%d,%d:hashKey %s\n", i, j, buf);
                _topiary_results_add_unsigned_long(results, buf,
                                                   strtoul(val, NULL, 0));
            }
            else if (!strcmp(key, "page")) {
                if (j > npagetitles) {
                    INTCAT(buf, j - 1, n3, sizeof(buf));
                }
                else {
                    STRLCAT(buf, pagetitles[j - 1], n3, sizeof(buf));
                }
                _TOPIARY_DEBUG("%d,%d:hashKey %s\n", i, j, buf);
                _topiary_results_add_unsigned_long(results, buf,
                                                   strtoul(val, NULL, 0));
            }
            else if (!strcmp(key, "swap")) {
                if (j > nswaptitles) {
                    INTCAT(buf, j - 1, n3, sizeof(buf));
                }
                else {
                    STRLCAT(buf, swaptitles[j - 1], n3, sizeof(buf));
                }
                _TOPIARY_DEBUG("%d,%d:hashKey %s\n", i, j, buf);
                _topiary_results_add_unsigned_long(results, buf,
                                                   strtoul(val, NULL, 0));
            }
            else {
                INTCAT(buf, j - 1, n3, sizeof(buf));
                _TOPIARY_DEBUG("%d,%d:hashKey %s\n", i, j, buf);
                _topiary_results_add_unsigned_long(results, buf,
                                                   strtoul(val, NULL, 0));
            }
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_stat_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/stat" };
    ret = _topiary_base_new(base, "proc_stat", _TOPIARY_BASE_TYPE_VANILLA,
                            NULL, proc_stat_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
