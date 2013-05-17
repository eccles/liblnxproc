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

static void
derived_values(int i, int j, _LNXPROC_RESULTS_T * results,
               _LNXPROC_RESULTS_T * presults, _LNXPROC_RESULTS_TABLE_T * entry,
               float out, float tdiff)
{
    if (tdiff > 0.0) {
        _LNXPROC_RESULTS_TABLE_T dentry;
        _LNXPROC_RESULTS_TABLE_T pentry;

        strcpy(pentry.key, entry->key);
        LNXPROC_ERROR_T ret = _lnxproc_results_fetch(presults, &pentry);

        _LNXPROC_DEBUG("%d,%d:Prev %s = %s\n", i, j, pentry.key, pentry.value);
        if (ret)
            return;

        snprintf(dentry.key, sizeof dentry.key, "%s-s", entry->key);
        snprintf(dentry.value, sizeof dentry.value, "%f",
                 (out - atof(pentry.value)) / tdiff);
        _lnxproc_results_add(results, &dentry);
        _LNXPROC_DEBUG("%d,%d:Curr %s-s = %s\n", i, j, dentry.key,
                       dentry.value);
#ifdef DEBUG
        if (atof(dentry.value) < 0.0) {
            _LNXPROC_DEBUG("WARN: diff < 0 (=%s)\n", dentry.value);
        }
#endif
    }
}

static LNXPROC_ERROR_T
proc_diskstats_normalize(LNXPROC_BASE_T *base)
{
    LNXPROC_BASE_DATA_T *data = base->current;
    _LNXPROC_RESULTS_T *results = data->results;
    _LNXPROC_ARRAY_T *current = data->array;

    _LNXPROC_DEBUG("Current data is %d at %p\n", data->id, data);
    _LNXPROC_DEBUG("Current results is at %p\n", results);

    float tdiff = 0.0;
    LNXPROC_BASE_DATA_T *pdata = base->previous;
    _LNXPROC_RESULTS_T *presults = NULL;

    if (pdata) {
        _lnxproc_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _LNXPROC_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _LNXPROC_DEBUG("Previous results is at %p\n", presults);
        _LNXPROC_DEBUG("Time difference = %f secs\n", tdiff);
    }

    const size_t nrows = current->vector->length;

    _LNXPROC_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) current->vector->values;

#define MAJORKEYCOL 0
#define MINORKEYCOL 1
#define NAMECOL 2
#define KEYCOL NAMECOL
#define READSCOL 3
#define MERGE_READCOL 4
#define S_READCOL 5
#define MS_READCOL 6
#define WRITESCOL 7
#define MERGE_WRITECOL 8
#define S_WRITECOL 9
#define MS_WRITECOL 10
#define IOSCOL 11
#define MS_IOCOL 12
#define MS_WEIGHTEDCOL 13
    static const int const precols[] = { MS_READCOL, MS_WRITECOL, };
    static const int nprecols = sizeof(precols) / sizeof(precols[0]);

/*
 * TODO - get sector size from other LNXPROC module.
 * For now fix it at 512 bytes.
 */
#define sectorsize 512
#define sectorscale sectorsize/1024.0
    struct par_t {
        const char *name;
        const float scale;
    };
    static const struct par_t pars[] = { {.name = "major",.scale = 0.0,},
    {.name = "minor",.scale = 0.0,},
    {.name = "name ",.scale = 0.0,},
    {.name = "reads",.scale = 1.0,},
    {.name = "merge_read",.scale = 1.0,},
    {.name = "s_read",.scale = sectorscale,},
    {.name = "ms_read",.scale = 1.e-3,},
    {.name = "writes",.scale = 1.0,},
    {.name = "merge_write",.scale = 1.0,},
    {.name = "s_write",.scale = sectorscale,},
    {.name = "ms_write",.scale = 1.e-3,},
    {.name = "ios",.scale = 0.0,},
    {.name = "ms_io",.scale = 1.e-1,},
    {.name = "ms_weighted",.scale = 1.e-1,},
    };
    static const size_t numcols = sizeof(pars) / sizeof(pars[0]);

    int i;

    _lnxproc_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        char *key = values[i][KEYCOL];

        if (!key)
            continue;

        _LNXPROC_DEBUG("%1$d:Key %2$p '%2$s'\n", i, key);

        float iodiff[nprecols];

        memset(iodiff, 0, sizeof iodiff);

        int j;

        for (j = 0; j < nprecols; j++) {        // ms_read, ms_write
            int k = precols[j];

            char *val = values[i][k];

            if (val) {
                float secs = pars[k].scale * atoi(val);
                _LNXPROC_RESULTS_TABLE_T entry;

                snprintf(entry.key, sizeof entry.key, "/%s/%s", key,
                         pars[k].name);
                snprintf(entry.value, sizeof entry.value, "%f", secs);
                _LNXPROC_DEBUG("%d,%d:Curr %s = %s\n", i, k, entry.key,
                               entry.value);
                _lnxproc_results_add(results, &entry);
                if (!presults)
                    continue;
                _LNXPROC_RESULTS_TABLE_T pentry;

                strcpy(pentry.key, entry.key);
                LNXPROC_ERROR_T ret = _lnxproc_results_fetch(presults, &pentry);

                _LNXPROC_DEBUG("%d,%d:Prev %s = %s\n", i, k, pentry.key,
                               pentry.value);
                if (!ret)
                    continue;

                iodiff[j] = secs - atof(pentry.value);
                _LNXPROC_DEBUG("%d:iodiff %s = %f\n", i, entry.key, iodiff[j]);
            }
        }

        size_t ncols = current->vector->children[i]->length;
        int n = ncols > numcols ? numcols : ncols;

        for (j = 0; j < n; j++) {
            if (j == KEYCOL || j == MS_READCOL || j == MS_WRITECOL)
                continue;
            float out = 0.0;

            _LNXPROC_RESULTS_TABLE_T entry;

            snprintf(entry.key, sizeof entry.key, "/%s/%s", key, pars[j].name);

            char *val = values[i][j];

            _LNXPROC_DEBUG("%d,%d:Curr value %s = %s\n", i, j, entry.key, val);
            if (!val)
                continue;

            if (j > NAMECOL) {
                out = pars[j].scale * atoi(val);
                snprintf(entry.value, sizeof entry.value, "%f", out);
                _lnxproc_results_add(results, &entry);
            }
            else {
                strcpy(entry.value, val);
                _lnxproc_results_add(results, &entry);
            }
            _LNXPROC_DEBUG("%d,%d:Curr %s = %s\n", i, j, entry.key,
                           entry.value);

            if (!presults)
                continue;

            if ((j == READSCOL) || (j == MERGE_READCOL) || (j == S_READCOL)) {

                derived_values(i, j, results, presults, &entry, out, iodiff[0]);
            }
            if ((j == WRITESCOL) || (j == MERGE_WRITECOL) || (j == S_WRITECOL)) {
                derived_values(i, j, results, presults, &entry, out, iodiff[1]);
            }
            if ((j == MS_IOCOL) || (j == MS_WEIGHTEDCOL)) {
                derived_values(i, j, results, presults, &entry, out, tdiff);
            }
        }
    }
    _lnxproc_results_hash(results);
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

    return _lnxproc_base_new(base, LNXPROC_BASE_TYPE_PREVIOUS,
                             filenames, 1, NULL, NULL, NULL,
                             NULL,
                             proc_diskstats_normalize, NULL, 256, limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
