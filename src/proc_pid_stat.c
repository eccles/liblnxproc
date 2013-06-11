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

*/

#include <stdio.h>
#include <stdlib.h>             //strtoul
#include <string.h>

#include "allocate.h"
#include "strlcpy.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"
#include "uthash.h"

/*-----------------------------------------------------------------------------
 * list handlers - used to store lists of pid records
 * that correspond to a particular parent or process group
 */
struct my_list_t {
    struct my_list_t *next;
    int value;
};

static struct my_list_t *
mylist_clear(struct my_list_t *mylist)
{
    _LNXPROC_DEBUG("FREE %p\n", mylist);
    while (mylist) {
        struct my_list_t *tmp = mylist->next;

        _LNXPROC_DEBUG("FREE %p PID %d\n", mylist, mylist->value);
        free(mylist);
        mylist = tmp;
    }
    return NULL;
}

static struct my_list_t *
mylist_add(struct my_list_t *mylist, int value)
{
    struct my_list_t *mlist = Allocate(NULL, sizeof(struct my_list_t));

    if (!mlist)
        return mylist;
    mlist->value = value;
    mlist->next = mylist;
    mylist = mlist;
    _LNXPROC_DEBUG("ADD %p VAL %d\n", mlist, mlist->value);
    return mylist;
}

struct my_pidlist_t {
    struct my_pidlist_t *next;
    char *value;
};

static struct my_pidlist_t *
mypidlist_clear(struct my_pidlist_t *mypidlist)
{
    _LNXPROC_DEBUG("FREE %p\n", mypidlist);
    while (mypidlist) {
        struct my_pidlist_t *tmp = mypidlist->next;

        _LNXPROC_DEBUG("FREE %p PID %s\n", mypidlist, mypidlist->value);
        free(mypidlist);
        mypidlist = tmp;
    }
    return NULL;
}

static struct my_pidlist_t *
mypidlist_add(struct my_pidlist_t *mypidlist, char *value)
{
    struct my_pidlist_t *mpidlist = Allocate(NULL, sizeof(struct my_pidlist_t));

    if (!mpidlist)
        return mypidlist;
    mpidlist->value = value;
    mpidlist->next = mypidlist;
    mypidlist = mpidlist;
    _LNXPROC_DEBUG("ADD %p VAL %s\n", mpidlist, mpidlist->value);
    return mypidlist;
}

/*-----------------------------------------------------------------------------
 * A hash of lists. The key to the hash is a parent pid or process group.
 * Each entry has a list of pid records corresponding to the children of a 
 * pid or member of a particular process group.
 */
struct my_hash_t {
    int key;
    struct my_list_t *head;
    UT_hash_handle hh;
};

static struct my_hash_t *
myhash_clear(struct my_hash_t *myhash)
{
    _LNXPROC_DEBUG("FREE %p\n", myhash);
    if (myhash) {
        struct my_hash_t *mhash, *tmp;

        HASH_ITER(hh, myhash, mhash, tmp) {
            _LNXPROC_DEBUG("FREE %p PGRP %d\n", mhash, mhash->key);
            mhash->head = mylist_clear(mhash->head);
            HASH_DEL(myhash, mhash);
            free(mhash);
        }
    }
    return NULL;
}

static struct my_hash_t *
myhash_add(struct my_hash_t *myhash, int key, int value)
{
    struct my_hash_t *mhash;

    HASH_FIND_INT(myhash, &key, mhash);
    if (!mhash) {
        mhash = Allocate(NULL, sizeof(struct my_hash_t));
        if (!mhash)
            return myhash;
        mhash->key = key;
        _LNXPROC_DEBUG("ADD %p KEY %d\n", mhash, mhash->key);
        HASH_ADD(hh, myhash, key, sizeof(mhash->key), mhash);
    }
    if (mhash) {
        mhash->head = mylist_add(mhash->head, value);
    }
    return myhash;
}

/*-----------------------------------------------------------------------------
 * Column names for each file read corresponding to a particular pid
 */
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

#define NCOLKEYS (sizeof(colkey) / sizeof(colkey[0]))

#define COMMCOL  2
#define PPIDCOL  4
#define PGRPCOL  5
#define TPGIDCOL  8
#define MINFLTCOL  10
#define CMINFLTCOL  11
#define MAJFLTCOL  12
#define CMAJFLTCOL  13
#define UTIMECOL  14
#define STIMECOL  15
#define CUTIMECOL 16
#define CSTIMECOL 17
#define ITREALVALUECOL 21
#define STARTTIMECOL 22
#define VSIZECOL 23
#define RSSCOL  24
#define RLIMCOL 25
#define NSWAPCOL 36
#define CNSWAPCOL 37
#define GUEST_TIMECOL  42
#define CGUEST_TIMECOL 43
#define DELAY_BLKIO_TICKSCOL 44

/*-----------------------------------------------------------------------------
 * structure containing all objects needed for process the pid data. 
 * This structure is a portable stack that is passed as an argument to all
 * functions that need it.
 */
struct process_t {
    _LNXPROC_VECTOR_T *vector;
    char key[64];
    int n1;
    float tdiff;
    _LNXPROC_RESULTS_T *presults;
    _LNXPROC_RESULTS_T *results;
    _LNXPROC_RESULTS_TABLE_T *hash;
    struct my_hash_t *ppidhash;
    struct my_hash_t *pgrphash;
    struct my_list_t *pidlist;
    struct my_pidlist_t *allpidlist;
};

/*-----------------------------------------------------------------------------
 * Process a pid record at record 'i'. Converts values found into appropriate
 * forms and stores them in the results table. Also stores values that are
 * needed for later post processing.
 */
static void
process_pid(int i, struct process_t *process)
{
    _LNXPROC_VECTOR_T *vector = process->vector;
    char ***values = (char ***) vector->values;
    char *key = process->key;
    size_t keysize = sizeof(process->key);
    int n1 = process->n1;

    _LNXPROC_RESULTS_T *results = process->results;
    _LNXPROC_RESULTS_TABLE_T *hash = process->hash;

    char **value1 = (char **) values[i];
    char *pidkey = value1[0];

    if (!pidkey)
        return;

    process->allpidlist = mypidlist_add(process->allpidlist, pidkey);

    size_t ncols = vector->children[i]->length;
    size_t ncols1 = ncols > NCOLKEYS ? NCOLKEYS : ncols;

    _LNXPROC_DEBUG("%d:first Rowkey value %s\n", i, pidkey);
    int n2 = n1;

    STRLCAT(key, pidkey, n2, keysize);
    STRLCAT(key, "/", n2, keysize);
/*
 * first 2 columns are pid which is the hashkey
 */
    int j;

    for (j = 2; j < ncols1; j++) {
        char *val = value1[j];

        if (!val)
            continue;

        int n3 = n2;

        STRLCAT(key, colkey[j], n3, keysize);

        if ((j == VSIZECOL) || (j == RLIMCOL)) {
            unsigned long value = strtoul(val, 0, 10) / 1024;

            _LNXPROC_DEBUG("%d,%d:%s value %s ul %lu\n", i, j, key, val, value);
            _lnxproc_results_add_unsigned_long(results, key, value);
        }
        else if ((j == RSSCOL) || (j == NSWAPCOL) || (j == CNSWAPCOL)) {
            long value = atoi(val) * results->page_size;

            _LNXPROC_DEBUG("%d,%d:%s value %s long %ld\n", i, j, key,
                           val, value);
            _lnxproc_results_add_long(results, key, value);
        }
        else if ((j == UTIMECOL) ||
                 (j == STIMECOL) || (j == CUTIMECOL) || (j == CSTIMECOL)
                 || (j == STARTTIMECOL)) {
            float value = strtoul(val, 0, 10) * results->secs_per_jiffy;

            _LNXPROC_DEBUG("%d,%d:%s value %s float %f\n", i, j, key,
                           val, value);
            _lnxproc_results_add_float(results, key, value);

/*
 * Store values so that usage (%) can be calculated.
 */
            _LNXPROC_RESULTS_TABLE_T *hentry =
                Allocate(NULL, sizeof(_LNXPROC_RESULTS_TABLE_T));
            if (!hentry)
                continue;
            strlcpy(hentry->key, key, sizeof hentry->key);
            hentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT;
            hentry->value.f = value;
            _LNXPROC_DEBUG("%d,%d:Store %s = %f\n", i, j, hentry->key, value);
            HASH_ADD(hh, hash, key, n3, hentry);
        }
        else if ((j == ITREALVALUECOL) ||
                 (j == GUEST_TIMECOL) ||
                 (j == CGUEST_TIMECOL) || (j == DELAY_BLKIO_TICKSCOL)) {
            float value = strtoul(val, 0, 10) * results->secs_per_jiffy;

            _LNXPROC_DEBUG("%d,%d:%s value %s float %f\n", i, j, key,
                           val, value);
            _lnxproc_results_add_float(results, key, value);

        }
        else if ((j == MINFLTCOL) ||
                 (j == CMINFLTCOL) || (j == MAJFLTCOL) || (j == CMAJFLTCOL)) {
            unsigned long value = strtoul(val, 0, 10);

            _lnxproc_results_add_unsigned_long(results, key, value);
        }
        else if ((j == TPGIDCOL) || (j == PPIDCOL) || (j == PPIDCOL)) {
            _lnxproc_results_add_int(results, key, atoi(val));
        }
        else {
            _lnxproc_results_add_stringref(results, key, val);
        }
    }
    process->hash = hash;
}

/*-----------------------------------------------------------------------------
 * Calculate rates (per second) and usage (%)
 */
static void
process_rates(int i, struct process_t *process)
{
    _LNXPROC_VECTOR_T *vector = process->vector;
    char ***values = (char ***) vector->values;
    char *key = process->key;
    size_t keysize = sizeof(process->key);
    int n1 = process->n1;
    float tdiff = process->tdiff;
    _LNXPROC_RESULTS_T *presults = process->presults;
    _LNXPROC_RESULTS_T *results = process->results;
    _LNXPROC_RESULTS_TABLE_T *hash = process->hash;

    char **value1 = (char **) values[i];
    char *pidkey = value1[0];

    if (!pidkey)
        return;

    _LNXPROC_DEBUG("%d:second Rowkey value %s\n", i, pidkey);

    int n2 = n1;

    STRLCAT(key, pidkey, n2, keysize);
    STRLCAT(key, "/", n2, keysize);

    _LNXPROC_RESULTS_TABLE_T *startentry = NULL;

    HASH_FIND_STR(hash, key, startentry);

/*
 * Ignore previous entries with a different starttime
 */
    if (startentry) {
        _LNXPROC_DEBUG("%d:current starttime for %s is %f\n", i, pidkey,
                       startentry->value.f);
        _LNXPROC_RESULTS_TABLE_T *pentry = NULL;

        int n3 = n2;

        STRLCAT(key, colkey[STARTTIMECOL], n3, keysize);
        int ret = _lnxproc_results_fetch(presults, key, &pentry);

        if (ret)
            return;

        _LNXPROC_DEBUG("%d:previous starttime for %s is %f\n", i,
                       pidkey, pentry->value.f);

        if (pentry->value.f != startentry->value.f)
            return;
    }

    int j;

    for (j = UTIMECOL; j <= CSTIMECOL; j++) {
        int n3 = n2;

        STRLCAT(key, colkey[j], n3, keysize);

        _LNXPROC_RESULTS_TABLE_T *pentry = NULL;

        int ret = _lnxproc_results_fetch(presults, key, &pentry);

        if (ret)
            continue;

        _LNXPROC_DEBUG("%d,%d:Prev value %s = %f\n", i, j, pentry->key,
                       pentry->value.f);

        _LNXPROC_RESULTS_TABLE_T *hentry = NULL;

        HASH_FIND_STR(hash, key, hentry);
        if (!hentry)
            continue;

        STRLCAT(key, "%", n3, keysize);

        float value = ((hentry->value.f - pentry->value.f) * 100.0)
            / tdiff;

        if (value < 0.0) {
            _LNXPROC_DEBUG("%d,%d:WARN Usage %s = %f\n", i, j, key, value);
        }
        else {
            _LNXPROC_DEBUG("%d,%d:Usage %s = %f\n", i, j, key, value);
        }

        _lnxproc_results_add_float(results, key, value);
    }
}

/*-----------------------------------------------------------------------------
 * Process one pid record - this function is recursive so that the complete
 * tree of processes that are children and lower generations are found.
 * Also visit all processes that belong to the same process group.
 * The second column of the raw data is tagged so that multiple occurrences of
 * the same pid in the hash tables are avoided.
 */
static void
pid_iterate(int i,
            char donetag,
            struct process_t *process, void (*func) (int, struct process_t *))
{
    char ***values = (char ***) process->vector->values;
    struct my_hash_t *ppidhash = process->ppidhash;
    struct my_hash_t *pgrphash = process->pgrphash;

    char **value1 = (char **) values[i];

/*
 * avoid records that have already been processed
 */
    char *tag = value1[1];

    if (!tag)
        return;

    if (tag[0] == donetag)
        return;
    tag[0] = donetag;

/*
 * Execute processing on this pid
 */
    func(i, process);

    char *pidkey = value1[0];

    if (!pidkey)
        return;

    int pid = atoi(pidkey);
    struct my_hash_t *mhash;

/*
 * Recursively process all children
 */
    HASH_FIND_INT(ppidhash, &pid, mhash);
    if (mhash) {
        struct my_list_t *mlist = mhash->head;

        while (mlist) {
            pid_iterate(mlist->value, donetag, process, func);
            mlist = mlist->next;
        }
    }

/*
 * Recursively process all processes that belong to the same process group
 */
    HASH_FIND_INT(pgrphash, &pid, mhash);
    if (mhash) {
        struct my_list_t *mlist = mhash->head;

        while (mlist) {
            pid_iterate(mlist->value, donetag, process, func);
            mlist = mlist->next;
        }
    }
}

/*-----------------------------------------------------------------------------
 * Process all pids found. IF master is set then only process pids that 
 * have the same process groups and the same ancestors (parents, grandparents
 * etc....).
 * Otherwise process all pids.
 */
static void
pids_iterate(char *master,
             char donetag,
             struct process_t *process, void (*func) (int, struct process_t *))
{

    if (master) {
        struct my_list_t *pidlist = process->pidlist;
        struct my_list_t *pid = NULL;

        for (pid = pidlist; pid; pid = pid->next) {
            pid_iterate(pid->value, donetag, process, func);
        }
    }
    else {
        size_t nrows = process->vector->length;
        int i;

        for (i = 0; i < nrows; i++) {
            func(i, process);
        }
    }
}

/*-----------------------------------------------------------------------------
 * Process the raw data returned by the rawread function for this module
 */
static int
proc_pid_stat_normalize(_LNXPROC_BASE_T *base)
{

    struct process_t process;

    _LNXPROC_BASE_DATA_T *data = base->current;

    process.results = data->results;
    _LNXPROC_ARRAY_T *array = data->array;

    process.vector = array->vector;
    LNXPROC_OPT_T *opt = base->optional;

    _LNXPROC_DEBUG("Optional %p\n", opt);
    char *master = NULL;
    LNXPROC_MODULE_T *sub = NULL;

    if (opt) {
        if (opt->master) {
            master = opt->master;
            if (opt->module)
                sub = opt->module;
        }
    }
    _LNXPROC_DEBUG("Master task '%s'\n", master);
    _LNXPROC_DEBUG("Sub %p\n", sub);

    size_t nrows = process.vector->length;

    _LNXPROC_DEBUG("Current data is %d at %p\n", data->id, data);
    _LNXPROC_DEBUG("Current results is at %p\n", process.results);
    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) process.vector->values;

    _LNXPROC_BASE_DATA_T *pdata = base->previous;

    process.presults = NULL;

    process.tdiff = 0.0;

    int i;

    process.hash = NULL;

    process.pgrphash = NULL;
    process.ppidhash = NULL;
    process.pidlist = NULL;
    process.allpidlist = NULL;

/*
 * If master task is specified then build lists of process groups and trees
 * of ancestors for each pid.
 */
    if (master) {
        for (i = 0; i < nrows; i++) {
            char **value1 = values[i];
            char *pidkey = value1[0];

            if (!pidkey)
                continue;

            char *val = value1[PGRPCOL];

            if (!val)
                continue;

            int pgrp = atoi(val);

            _LNXPROC_DEBUG("%d:PID %d PGRP %d\n", i, atoi(pidkey), pgrp);
            process.pgrphash = myhash_add(process.pgrphash, pgrp, i);

            val = value1[PPIDCOL];

            if (!val)
                continue;
            int ppid = atoi(val);

            _LNXPROC_DEBUG("%d:PID %d PPID %d\n", i, atoi(pidkey), ppid);

            process.ppidhash = myhash_add(process.ppidhash, ppid, i);

            val = value1[COMMCOL];
            if (!val)
                continue;
            if (ppid == 1 && !strcmp(master, val)) {
                process.pidlist = mylist_add(process.pidlist, i);
                _LNXPROC_DEBUG("MASTERPID %d at %d\n", atoi(pidkey), i);
            }
        }
    }

    process.n1 = 0;
    STRLCAT(process.key, "/", process.n1, sizeof(process.key));

/*
 * Convert raw values into the appropriate form and store in results table
 */
    _lnxproc_results_init(process.results, nrows);
    pids_iterate(master, '-', &process, process_pid);

/*
 * Construct new glob pattern
 */
    if (sub) {
        char fileglob[1024];
        struct my_pidlist_t *mpidlist = process.allpidlist;

        int n = 0;

        STRLCAT(fileglob, "{", n, sizeof fileglob);
        while (mpidlist) {
            STRLCAT(fileglob, mpidlist->value, n, sizeof fileglob);
            STRLCAT(fileglob, ",", n, sizeof fileglob);
            mpidlist = mpidlist->next;
        }
        n--;
        STRLCAT(fileglob, "}", n, sizeof fileglob);
        _LNXPROC_DEBUG("New fileglob '%s'\n", fileglob);
        _lnxproc_set_fileglob(sub, fileglob);
        lnxproc_read(sub);
    }

/*
 * different pids may appear at different positions in the array - so we have to
 * remember the locations of each pid entry in order to calculate usage etc.
 */
    if (pdata && pdata->array) {
        _lnxproc_base_timeval_diff(base, &process.tdiff);
        process.presults = pdata->results;
        _LNXPROC_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _LNXPROC_DEBUG("Previous results is at %p\n", process.presults);
    }

    _LNXPROC_DEBUG("Time difference = %f secs\n", process.tdiff);
    if (process.tdiff > 0.0) {
        _LNXPROC_DEBUG("Current timestamp is %f\n",
                       lnxproc_timeval_secs(&data->tv));
        _LNXPROC_DEBUG("Previous timestamp is %f\n",
                       lnxproc_timeval_secs(&pdata->tv));

        pids_iterate(master, '+', &process, process_rates);
    }
/*
 * Clean up all memory allocated
 */
    if (process.hash) {
        _LNXPROC_RESULTS_TABLE_T *entry, *tmp;

        HASH_ITER(hh, process.hash, entry, tmp) {
            HASH_DEL(process.hash, entry);
            free(entry);
        }
    }
    process.allpidlist = mypidlist_clear(process.allpidlist);
    process.pidlist = mylist_clear(process.pidlist);
    process.pgrphash = myhash_clear(process.pgrphash);
    process.ppidhash = myhash_clear(process.ppidhash);

    return LNXPROC_OK;
}

/*-----------------------------------------------------------------------------
 * Create this module 
 */
int
_lnxproc_proc_pid_stat_new(_LNXPROC_BASE_T **base, LNXPROC_OPT_T *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 4, "() ", 3);  /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    _LNXPROC_DEBUG("Optional %p\n", optional);
    LNXPROC_MODULE_T *sub = NULL;

    if (optional) {
        if (optional->master && optional->module) {
            sub = optional->module;
            _lnxproc_create(sub);
        }
    }
    _LNXPROC_DEBUG("Sub %p\n", sub);

    char *fileprefix = "/proc";
    char *fileglob;

    if (optional && optional->fileglob) {
        fileglob = optional->fileglob;
    }
    else {
        fileglob = "[1-9]*";
    }

    char *filesuffix = "stat";

    ret =
        _lnxproc_base_new(base, "proc_pid_stat",
                          _LNXPROC_BASE_TYPE_PREVIOUS, NULL,
                          proc_pid_stat_normalize, NULL, 256, limits);
    if (!ret) {
        _lnxproc_base_set_fileprefix(*base, fileprefix);
        _lnxproc_base_set_fileglob(*base, fileglob);
        _lnxproc_base_set_filesuffix(*base, filesuffix);
        _lnxproc_base_set_optional(*base, optional);
    }
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
