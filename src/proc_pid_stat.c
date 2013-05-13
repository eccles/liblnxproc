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

   3405 (httpd) S 1 3405 3405 0 -1 4202816 14491 0 762 0 15 4 0 0 18 0 2 0 7048
   441008128 15350 18446744073709551615 1 1 0 0 0 0 0 16781440 134235755
   18446744073709551615 0 0 17 3 0 0 304

Description::
 
    0 pid %d The process ID.
    1 comm %s The filename of the executable, in parentheses.  This is visible
      whether or not the executable is swapped out.
    2 state %c One character from the string "RSDZTW" where R is running, S is 
      sleeping in an interruptible wait,  D  is  waiting  in  uninterruptible
      disk sleep, Z is zombie, T is traced or stopped (on a signal), and W is
      paging.
    3 ppid %d The PID of the parent.
    4 pgrp %d The process group ID of the process.
    5 session %d The session ID of the process.
    6 tty_nr %d The tty the process uses.
    7 tpgid %d The process group ID of the process which currently owns the tty
      that the process is connected to.
    8 flags %lu The kernel flags word of the process. For bit meanings, see the
      PF_* defines in <linux/sched.h>.  Details depend on the kernel version.
    9 minflt %lu The number of minor faults the process has made which have not
      required loading a memory page from disk.
   10 cminflt %lu The number of minor faults that the process's waited-for 
      children have made.
   11 majflt %lu The number of major faults the process has made which have 
      required loading a memory page from disk.
   12 cmajflt %lu The number of major faults that the process's waited-for 
      children have made.
   13 utime %lu The number of jiffies that this process has been scheduled in
      user mode.
   14 stime %lu The number of jiffies that this process has been scheduled in
      kernel mode.
   15 cutime %ld The number of jiffies that this process's waited-for children
      have been scheduled in user mode. (See also times(2).)
   16 cstime %ld The number of jiffies that this process's waited-for children
      have been scheduled in kernel mode.
   17 priority %ld (Explanation  for  Linux  2.6)  
   18 nice %ld The nice value ranges from 19 (nicest) to -19 (not nice to
      others).
   19 num_threads %ld Number of threads in this process (since Linux 2.6).
      Before kernel 2.6, this field was hard coded to 0 as a placeholder for an
      earlier removed field.
   20 itrealvalue %ld The time in jiffies before the next SIGALRM is sent to the
      process due to an interval timer.
   21 starttime %lu The time in jiffies the process started after system boot.
   22 vsize %lu Virtual memory size in bytes.
   23 rss %ld Resident  Set  Size:  number of pages the process has in real
      memory, minus 3 for administrative purposes. This is just the pages which
      count towards text, data, or stack space. This does not include pages
      which have not been demand-loaded in, or which are swapped out.
   24 rlim %lu Current limit in bytes on the rss of the process
      (usually 4294967295 on i386).
   25 startcode %lu The address above which program text can run.
   26 endcode %lu The address below which program text can run.
   27 startstack %lu The address of the start of the stack.
   28 kstkesp %lu The current value of esp (stack pointer), as found in the
      kernel stack page for the process.
   29 kstkeip %lu The current EIP (instruction pointer).
   30 signal %lu The bitmap of pending signals.
   31 blocked %lu The bitmap of blocked signals.
   32 sigignore %lu The bitmap of ignored signals.
   33 sigcatch %lu The bitmap of caught signals.
   34 wchan %lu This is the "channel" in which the process is waiting. It is the
      address of a system call, and can be looked up in a namelist if you need a
      textual name.  (If you have an up-to-date /etc/psdatabase, then try ps -l
      to see the WCHAN field in action.)
   35 nswap %lu Number of pages swapped (not maintained).
   36 cnswap %lu Cumulative nswap for child processes (not maintained).
   37 exit_signal %d Signal to be sent to parent when we die.
   38 processor %d CPU number last executed on.
   39 rt_priority %lu (since kernel 2.5.19) Real-time scheduling priority (see 
      sched_setscheduler(2)).
   40 policy %lu (since kernel 2.5.19) Scheduling policy (see 
      sched_setscheduler(2)).
   41 delayacct_blkio_ticks %llu (since Linux 2.6.18) Aggregated block I/O 
      delays, measured in clock ticks (centiseconds).
   42 guest_time %lu (since Linux 2.6.24) Guest time of the process (time spent
      running a virtual CPU for a guest operating system), measured in clock 
      ticks (divide by sysconf(_SC_CLK_TCK).
   43 cguest_time %ld (since Linux 2.6.24) Guest time of the process's children,
      measured in clock ticks (divide by sysconf(_SC_CLK_TCK).

.. note::
   The no. of fields varies with kernel version. This code will read all fields 
   to 'cguest_time' as documented in the man pages. However the kernel used for
   testing contained a number of unknown extra fields. Older kernels will only
   return a dictionary with fewer fields. The assumption is that newer kernels
   will never delete any fields from previous versions.

'''
############################################################################

import util
from pidtype import PidType
from procpid import ProcPid

class ProcPidStat(ProcPid):
    '''
    Measures the stat file from /proc/<pid> filesystem
    '''
    def __init__(self, **kwds):
        '''
        Initialises object fields
        '''

        kwds['filename'] = 'stat'
        kwds['fields'] = [ 'pid', 'comm', 'state', 'ppid',
                           'pgrp', 'session', 'tty_nr', 'tpgid', 
                           'flags', 'minflt', 'cminflt', 'majflt', 
                           'cmajflt', 'utime', 'stime', 'cutime', 
                           'cstime', 'priority', 'nice', 'num_threads', 
                           'itrealvalue', 'starttime', 'vsize', 'rss', 
                           'rlim', 'startcode', 'endcode', 'startstack', 
                           'kstkesp', 'kstkeip', 'signal', 'blocked', 
                           'sigignore', 'sigcatch', 'wchan', 'nswap', 
                           'cnswap', 'exit_signal', 'processor', 
                           'rt_priority', 'policy', 'delay_blkio_ticks', 
                           'guest_time', 'cguest_time' ]

        super(ProcPidStat, self).__init__(**kwds)

        self.prev = None

    def normalize1(self, lines):
        '''
        Pre-normalize function. Used by the ProcAllpid() class for
        efficiency
        '''
        data = {}
        if lines:
            vals = lines[0].split()

            data = dict(zip(self.fields[:3], vals[:3] ))
            data.update(dict(zip(self.fields[3:], 
                                 [ int(val) for val in vals[3:] ])))

            for pidfield in ('pid', 'ppid', 'pgrp', 'tpgid'):
                data[pidfield] = PidType(data[pidfield])
    
            if 'comm' in data:
                data['comm'] = data['comm'][1:-1]

        return data

    def normalize2(self, timestamp, data):
        '''
        Post-normalize function. Used by the ProcAllpid() class for
        efficiency 
        '''
        for timefield in ('utime', 'stime', 'cutime', 'cstime', 'starttime',
                          'itrealvalue', 'guest_time', 'cguest_time'):
            util.jiffy_to_secs(data, timefield)

        util.divide(data, 'vsize', 1024)
        util.page_to_kib(data, 'rss')
        util.page_to_kib(data, 'nswap')
        util.page_to_kib(data, 'cnswap')
        util.divide(data, 'rlim', 1024)
        util.multiply(data, 'delay_blkio_ticks', 1000000)

        if self.prev:
            tdiff = timestamp - self.prev[0]
            prev = self.prev[1]
            util.percent(data, prev, 'utime', tdiff)
            util.percent(data, prev, 'stime', tdiff)
            util.percent(data, prev, 'cutime', tdiff)
            util.percent(data, prev, 'cstime', tdiff)

        self.prev = (timestamp, data)

        return self.filter(data)

    def normalize(self, timestamp, lines):
        '''
        Normalize is split into 2 halves for efficiency when  called from
        the ProcAllpid() class. Normal operation uses this method 
        '''
        data = self.normalize1(lines)
        return self.normalize2(timestamp, data)

if __name__ == "__main__":
    from Test import Test
#    Test(ProcPidStat, pid=getpid(),
#                      include=r'^comm|^[a-z]*pid|^utime|^stime|pgrp|rss|vsize')
    Test(ProcPidStat)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_pid_stat.h"

static LNXPROC_ERROR_T
proc_pid_stat_normalize(LNXPROC_BASE_T *base)
{
    _LNXPROC_RESULTS_T *results = base->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;
    char *rowkey;

    int i, j;

    static const char *colkey[] = {
        "pid", "pid", "comm", "state", "ppid",
        "pgrp", "session", "tty_nr", "tpgid",
        "flags", "minflt", "cminflt", "majflt",
        "cmajflt", "utime", "stime", "cutime",
        "cstime", "priority", "nice", "num_threads",
        "itrealvalue", "starttime", "vsize", "rss",
        "rlim", "startcode", "endcode", "startstack",
        "kstkesp", "kstkeip", "signal", "blocked",
        "sigignore", "sigcatch", "wchan", "nswap",
        "cnswap", "exit_signal", "processor",
        "rt_priority", "policy", "delay_blkio_ticks",
        "guest_time", "cguest_time"
    };
    size_t ncolkeys = sizeof(colkey) / sizeof(colkey[0]);

    _lnxproc_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        size_t ncols = array->vector->children[i]->length;
        size_t ncols1 = ncols > ncolkeys ? ncolkeys : ncols;

        rowkey = values[i][0];
        for (j = 1; j < ncols1; j++) {
            char buf[32];

            val = values[i][j];
            if ((j == 23) ||    // vsize
                (j == 25)) {    // rlim
                snprintf(buf, sizeof buf, "%d", atoi(val) / 1024);
                _lnxproc_results_add(results, buf, "/%s/%s", rowkey, colkey[j]);
            }
            else if ((j == 24) ||       // rss
                     (j == 36) ||       // nswap
                     (j == 37)) {       // cnswap
                snprintf(buf, sizeof buf, "%ld",
                         atoi(val) * results->page_size);
                _lnxproc_results_add(results, buf, "/%s/%s", rowkey, colkey[j]);
            }
            else if ((j == 14) ||       // utime
                     (j == 15) ||       // stime
                     (j == 16) ||       // cutime
                     (j == 17) ||       // cstime
                     (j == 21) ||       // itrealvalue
                     (j == 22) ||       // starttime
                     (j == 42) ||       // guest_time
                     (j == 43) ||       // cguest_time
                     (j == 44)) {       // delay_blkio_ticks
                snprintf(buf, sizeof buf, "%f",
                         atoi(val) * results->secs_per_jiffy);
                _lnxproc_results_add(results, buf, "/%s/%s", rowkey, colkey[j]);
            }
            else {
                _lnxproc_results_add(results, val, "/%s/%s", rowkey, colkey[j]);
            }
        }
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_pid_stat_new(LNXPROC_BASE_T **base)
{

    _LNXPROC_LIMITS_T limits[] = {
        {.expected = 9,
         .chars = "\n",
         .len = 1},             /* row delimiters */
        {.expected = 4,
         .chars = " ",
         .len = 1}              /* column delimiters */
    };

    char *fileprefix = { "/proc" };
    char *filesuffix = { "stat" };
    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return _lnxproc_base_new(base,
                             NULL, 0, fileprefix, filesuffix,
                             NULL, proc_pid_stat_normalize, NULL, 256,
                             limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
