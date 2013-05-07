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

This is the order of metrics in /proc/diskstats::

   0 major        Major number
   1 minor        Minor number
   2 name         Name
   3 reads        This is the total number of reads completed successfully.
   4 merge_read   Reads and writes which are adjacent to each other may be
                  merged for efficiency. Thus two 4K reads may become one 8K 
                  read before it is ultimately handed to the disk, and so it 
                  will be counted (and queued) as only one I/O. This field lets
                  you know how often this was done.
   5 s_read       This is the total number of sectors read successfully.
   6 ms_read      This is the total number of milliseconds spent by all reads.
   7 writes       This is the total number of writes completed successfully.
   8 merge_write  Reads and writes which are adjacent to each other may be
                  merged for efficiency. Thus two 4K reads may become one 8K 
                  read before it is ultimately handed to the disk, and so it
                  will be counted (and queued) as only one I/O. This field lets
                  you know how often this was done.
   9 s_write      This is the total number of sectors written successfully.
   10 ms_write    This is the total number of milliseconds spent by all writes.
   11 ios         The only field that should go to zero. Incremented as requests
                  are given to appropriate request_queue_t and decremented as
                  they finish.
   12 ms_io       This field is increases so long as field 9 is nonzero.
   13 ms_weighted This field is incremented at each I/O start, I/O completion, #
                  I/O

Typical contents of /proc/diskstats::

      1    0 ram0 0 0 0 0 0 0 0 0 0 0 0
      1    1 ram1 0 0 0 0 0 0 0 0 0 0 0
      1    2 ram2 0 0 0 0 0 0 0 0 0 0 0
      1    3 ram3 0 0 0 0 0 0 0 0 0 0 0
      1    4 ram4 0 0 0 0 0 0 0 0 0 0 0
      1    5 ram5 0 0 0 0 0 0 0 0 0 0 0
      1    6 ram6 0 0 0 0 0 0 0 0 0 0 0
      1    7 ram7 0 0 0 0 0 0 0 0 0 0 0
      1    8 ram8 0 0 0 0 0 0 0 0 0 0 0
      1    9 ram9 0 0 0 0 0 0 0 0 0 0 0
      1   10 ram10 0 0 0 0 0 0 0 0 0 0 0
      1   11 ram11 0 0 0 0 0 0 0 0 0 0 0
      1   12 ram12 0 0 0 0 0 0 0 0 0 0 0
      1   13 ram13 0 0 0 0 0 0 0 0 0 0 0
      1   14 ram14 0 0 0 0 0 0 0 0 0 0 0
      1   15 ram15 0 0 0 0 0 0 0 0 0 0 0
      8    0 sda 1958502 875995 119654702 27720992 10620346 8610695 154141942
                 2172968844 0 30623307 2201458835
      8    1 sda1 152 1853 2287 1849 89 27 230 7932 0 9737 9781
      8    2 sda2 227184 610491 6699952 3069044 138618 740443 7256536 32485911 0
                  2201739 36166789
      8    3 sda3 1731147 263634 112952175 24649802 10481639 7870225 146885176
                  2140475001 0 29931840 2165282323
    253    0 dm-0 791292 0 18866082 11314325 10345209 0 82761672 2133498386 0
                  26344873 2144813575
    253    1 dm-1 1188117 0 93835442 17701655 3557810 0 28462480 1190191767 0
                  10791972 1207900801
    253    2 dm-2 15848 0 247890 163041 4457625 0 35661000 373930544 0 6793367
                  374095145
    253    3 dm-3 307 0 2450 1354 3 0 24 1 0 295 1355
     11    0 sr0 0 0 0 0 0 0 0 0 0 0 0
      9    0 md0 0 0 0 0 0 0 0 0 0 0 0

*/

#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_diskstats.h"

LNXPROC_ERROR_T
lnxproc_strings_append(char ***strings, size_t * n, char *val)
{
    char **p = realloc(*strings, (*n + 1) * sizeof(char *));

    if (!p) {
        return LNXPROC_ERROR_SYSTEM;
    }

    p[*n] = strdup(val);
    (*n)++;
    *strings = p;
    return LNXPROC_OK;
}

void
lnxproc_strings_free(char **strings, size_t nkeys)
{
    if (strings) {
        int i;

        for (i = 0; i < nkeys; i++) {
            free(strings[i]);
        }
        free(strings);
    }
}

static LNXPROC_ERROR_T
proc_diskstats_normalize(LNXPROC_BASE_T *base)
{
    _LNXPROC_RESULTS_T *results = base->results;
    _LNXPROC_ARRAY_T *current = base->current.array;
    _LNXPROC_ARRAY_T *previous = base->previous.array;

    char **keys = NULL;
    size_t nkeys = 0;
    int key = 2;
    size_t idx[] = { 0, key };
    char *val = NULL;

    while (!_lnxproc_array_get(current, idx, 2, &val)) {
        _LNXPROC_DEBUG("%1$zd:Val %2$p '%2$s'\n", idx[0], val);
        _lnxproc_results_store(results, val, "/key%02d", idx[0]);
        lnxproc_strings_append(&keys, &nkeys, val);
        idx[0]++;
        val = NULL;
    }
    size_t nrows = idx[0];

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);

    static const char *cols[] =
        { "major", "minor", "name ", "reads", "merge_read",
        "s_read", "ms_read", "writes", "merge_write", "s_write",
        "ms_write", "ios", "ms_io", "ms_weighted",
    };
/*
 * TODO - get sector size from other LNXPROC module.
 * For now fix it at 512 bytes.
 */
#define sectorsize 512
#define sectorscale sectorsize/1024.0
    static const float scale[] = { 0.0, 0.0, 0.0, 1.0, 1.0,
        sectorscale, 1.e-3, 1.0, 1.0, sectorscale,
        1.e-3, 0.0, 1.e-3, 1.e-3,
    };
    size_t ncols = sizeof(cols) / sizeof(cols[0]);

    int i;

    for (i = 0; i < ncols; i++) {
        _lnxproc_results_store(results, cols[i], "/col%02d", i);
    }

    for (i = 0; i < nrows; i++) {
        idx[0] = i;

        idx[1] = 6;             // 'ms_read'
        int readtime;

        _lnxproc_array_diff(previous, current, idx, 2, &readtime);

        idx[1] = 10;            // 'ms_write'
        int writetime;

        _lnxproc_array_diff(previous, current, idx, 2, &writetime);

        idx[1] = 0;
        val = NULL;
        while (!_lnxproc_array_get(current, idx, 2, &val)) {
            if (idx[1] != key) {
                _LNXPROC_DEBUG("%1$zd,%2$zd:Val %3$p '%3$s'\n", idx[0], idx[1],
                               val);
                _lnxproc_results_store(results, val, "/%s/%s", keys[i],
                                       cols[idx[1]]);
                val = NULL;
                if (readtime > 0) {
                    if ((idx[1] == 3) ||        // 'reads'
                        (idx[1] == 4) ||        // 'merge_read'
                        (idx[1] == 5)) {        // 's_read'
                        char buf[32];

                        _lnxproc_base_variable_rate(base, idx, 2, scale[idx[1]],
                                                    readtime, buf, sizeof buf);
                        _lnxproc_results_store(results, val, "/%s/%s-s",
                                               keys[i], cols[idx[1]]);
                        val = NULL;
                    }
                }
                if (writetime > 0) {
                    if ((idx[1] == 7) ||        // 'writes'
                        (idx[1] == 8) ||        // 'merge_write'
                        (idx[1] == 9)) {        // 's_write'
                        char buf[32];

                        _lnxproc_base_variable_rate(base, idx, 2, scale[idx[1]],
                                                    writetime, buf, sizeof buf);
                        _lnxproc_results_store(results, val, "/%s/%s-s",
                                               keys[i], cols[idx[1]]);
                        val = NULL;
                    }
                }
                if ((idx[1] == 6) ||    // 'ms_read'
                    (idx[1] == 10) ||   // 'ms_write'
                    (idx[1] == 12) ||   // 'ms_io'
                    (idx[1] == 13)) {   // 'ms_weighted'
                    char buf[32];

                    _lnxproc_base_variable_usage(base, idx, 2, scale[idx[1]],
                                                 buf, sizeof buf);
                    _lnxproc_results_store(results, val, "/%s/%s%%",
                                           keys[i], cols[idx[1]]);
                    val = NULL;
                }
            }
            idx[1]++;
        }
    }
    lnxproc_strings_free(keys, nkeys);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_diskstats_new(LNXPROC_BASE_T **base)
{

    _LNXPROC_LIMITS_T limits[] = {
        {
         .expected = 9,.chars = "\n",.len = 1}, /* row delimiters */
        {
         .expected = 14,.chars = " ",.len = 1}  /* column delimiters */
    };
    char *filenames[] = {
        "/proc/diskstats"
    };
    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return _lnxproc_base_new(base,
                             filenames, 1, NULL, NULL,
                             NULL,
                             proc_diskstats_normalize, NULL, 256, limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
