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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_diskstats.h"

static LNXPROC_ERROR_T
proc_diskstats_normalize(LNXPROC_BASE_T *base)
{
    _LNXPROC_RESULTS_T *results = base->results;
    _LNXPROC_ARRAY_T *current = base->current->array;
    _LNXPROC_ARRAY_T *previous = NULL;

    if (base->previous) {
        previous = base->previous->array;
    }

    int keycol = 2;
    char *val = NULL;

    size_t nrows = current->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) current->vector->values;
    char ***prev = NULL;

    if (previous) {
        prev = (char ***) previous->vector->values;
    }

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
        sectorscale, 1.e-1, 1.0, 1.0, sectorscale,
        1.e-1, 0.0, 1.e-1, 1.e-1,
    };
    size_t ncols = sizeof(cols) / sizeof(cols[0]);

    float tdiff;

    _lnxproc_base_timeval_diff(base, &tdiff);

    int i;

    for (i = 0; i < nrows; i++) {
        char *key = values[i][keycol];

        _LNXPROC_DEBUG("%1$zd:Key %2$p '%2$s'\n", i, key);

        float readtime = 0;
        float writetime = 0;

        if (previous) {
            readtime = 1.e-6 * (atoi(values[i][6]) - atoi(prev[i][6]));
            writetime = 1.e-6 * (atoi(values[i][10]) - atoi(prev[i][10]));
        }

        int j;

        ncols = current->vector->children[i]->length;
        for (j = 0; j < ncols; j++) {
            if (j == keycol)
                continue;
            int diff;
            float rate;
            char buf[32];

            val = values[i][j];
            _lnxproc_results_store(results, val, "/%s/%s", key, cols[j]);
            if (readtime > 0.0) {
                if ((j == 3) || // 'reads'
                    (j == 4) || // 'merge_read'
                    (j == 5)) { // 's_read'

                    diff = atoi(values[i][j]) - atoi(prev[i][j]);
                    rate = (scale[j] * diff) / readtime;
                    snprintf(buf, sizeof buf, "%f", rate);
                    _lnxproc_results_store(results, buf, "/%s/%s-s",
                                           key, cols[j]);
                }
            }
            if (writetime > 0.0) {
                if ((j == 7) || // 'writes'
                    (j == 8) || // 'merge_write'
                    (j == 9)) { // 's_write'

                    diff = atoi(values[i][j]) - atoi(prev[i][j]);
                    rate = (scale[j] * diff) / writetime;
                    snprintf(buf, sizeof buf, "%f", rate);
                    _lnxproc_results_store(results, buf, "/%s/%s-s",
                                           key, cols[j]);
                }
            }
            if (tdiff > 0.0) {
                if ((j == 6) || // 'ms_read'
                    (j == 10) ||        // 'ms_write'
                    (j == 12) ||        // 'ms_io'
                    (j == 13)) {        // 'ms_weighted'

                    diff = atoi(values[i][j]) - atoi(prev[i][j]);
                    rate = (scale[j] * diff) / tdiff;
                    snprintf(buf, sizeof buf, "%5.1f", rate);
                    _lnxproc_results_store(results, buf, "/%s/%s%%",
                                           key, cols[j]);
                }
            }
        }
    }
    _lnxproc_base_store_previous(base);
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
