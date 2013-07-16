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
   13 ms_weighted This field is incremented at each I/O start, I/O completion, 
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

#include "config.h"

#include <stdio.h>
#include "topiary_stdlib.h"
#include <string.h>

#include "reference.h"
#include "strlcpy.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "base_private.h"
#include "interface_private.h"
#include "modules.h"

static void
derived_values(int i, int j, _TOPIARY_RESULTS_T *results,
               _TOPIARY_RESULTS_T *presults, char *pkey,
               int offset, size_t plen, float out, float tdiff)
{
    _TOPIARY_DEBUG("pKey %1$p '%1$s'\n", pkey);
    _TOPIARY_DEBUG("offset %d\n", offset);
    _TOPIARY_DEBUG("plen %zd\n", plen);
    _TOPIARY_DEBUG("out %f\n", out);
    _TOPIARY_DEBUG("tdiff %f\n", tdiff);
    if (tdiff > 0.0) {
        _TOPIARY_RESULTS_TABLE_T *pentry = NULL;

        int ret = _topiary_results_fetch(presults, pkey, &pentry);

        if (ret)
            return;

#ifdef DEBUG
        char buf[64];
        char *pbuf;

        _topiary_results_table_valuestr(pentry, buf, sizeof buf, &pbuf);

        _TOPIARY_DEBUG("%d,%d:Prev %s = %s\n", i, j, pkey, pbuf);
#endif
        STRLCAT(pkey, "-s", offset, plen);
        _TOPIARY_DEBUG("pKey %1$p '%1$s'\n", pkey);
        float value = (out - pentry->value.f) / tdiff;

        _topiary_results_add_fixed(results, pkey, value, 0, 1);
#ifdef DEBUG
        _TOPIARY_DEBUG("%d,%d:Rate %s = %f\n", i, j, pkey, value);
        if (value < 0.0) {
            _TOPIARY_DEBUG("WARN: diff < 0 (=%f)\n", value);
        }
#endif
    }
}

static int
proc_diskstats_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_BASE_DATA_T *data = base->current;
    _TOPIARY_RESULTS_T *results = data->results;
    _TOPIARY_ARRAY_T *current = data->array;

    _TOPIARY_DEBUG("Current data is %d at %p\n", data->id, data);
    _TOPIARY_DEBUG("Current results is at %p\n", results);

    float tdiff = 0.0;
    _TOPIARY_BASE_DATA_T *pdata = base->previous;
    _TOPIARY_RESULTS_T *presults = NULL;

    if (pdata) {
        _topiary_base_timeval_diff(base, &tdiff);
        presults = pdata->results;
        _TOPIARY_DEBUG("Previous data is %d at %p\n", pdata->id, pdata);
        _TOPIARY_DEBUG("Previous results is at %p\n", presults);
        _TOPIARY_DEBUG("Time difference = %f secs\n", tdiff);
    }

    const size_t nrows = current->vector->length;

    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) current->vector->values;

#define MAJORCOL 0
#define MINORCOL 1
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
    static const int const precols[] =
        { MS_READCOL, MS_WRITECOL, MS_IOCOL, MS_WEIGHTEDCOL, };
    static const int nprecols = sizeof(precols) / sizeof(precols[0]);

#define sectorscale 1.0/1024.0
    struct par_t {
        const char *name;
        const float scale;
    };

    static const struct par_t pars[] = {
        {.name = "major",},
        {.name = "minor",},
        {.name = "name",},
        {.name = "reads",.scale = 1.0,},
        {.name = "merge_read",.scale = 1.0,},
        {.name = "s_read",.scale = sectorscale,},
        {.name = "ms_read",.scale = 1.e-3,},
        {.name = "writes",.scale = 1.0,},
        {.name = "merge_write",.scale = 1.0,},
        {.name = "s_write",.scale = sectorscale,},
        {.name = "ms_write",.scale = 1.e-3,},
        {.name = "ios",.scale = 1.0,},
        {.name = "ms_io",.scale = 1.e-3,},
        {.name = "ms_weighted",.scale = 1.e-3,},
    };
    static const size_t numcols = sizeof(pars) / sizeof(pars[0]);

    int i;

    char pkey[64];

    int n1 = 0;

    STRLCAT(pkey, "/", n1, sizeof(pkey));
    _TOPIARY_DEBUG("pKey %1$p '%1$s'\n", pkey);

    _topiary_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        char *key = value1[KEYCOL];

        if (!key)
            continue;

        _TOPIARY_DEBUG("%1$d:Key %2$p '%2$s'\n", i, key);

        int n2 = n1;

        STRLCAT(pkey, key, n2, sizeof(pkey));
        STRLCAT(pkey, "/", n2, sizeof(pkey));
        _TOPIARY_DEBUG("%2$d:pKey %1$p '%1$s'\n", pkey, i);

/* sort out sector size */
        int n3 = n2;

        int sectorsize = 0;

        STRLCAT(pkey, "sector", n3, sizeof(pkey));
        _TOPIARY_DEBUG("%2$d:pKey %1$p '%1$s'\n", pkey, i);
        if (presults) {
            _TOPIARY_RESULTS_TABLE_T *pentry = NULL;
            int ret = _topiary_results_fetch(presults, pkey, &pentry);

            if (!ret) {
                sectorsize = pentry->value.i;
                _TOPIARY_DEBUG("%d:previous sectorsize %d\n", i, sectorsize);
                _topiary_results_add_int(results, pkey, sectorsize);
            }

        }
        if (sectorsize == 0) {
            _TOPIARY_RESULTS_T *disksectors = base->optional->results;

            if (disksectors) {
                char dkey[64];
                int m1 = 0;

                STRLCAT(dkey, "/", m1, sizeof(dkey));
                if (!strncmp(key, "sd", 2)) {
                    strlcpy(dkey + 1, key, 4);
                }
                else {
                    STRLCAT(dkey, key, m1, sizeof(dkey));
                }
                _TOPIARY_DEBUG("%2$d:dKey %1$p '%1$s'\n", dkey, i);
                _TOPIARY_RESULTS_TABLE_T *pentry = NULL;
                int ret = _topiary_results_fetch(disksectors, dkey, &pentry);

                if (ret) {
                    sectorsize = 512;
                    _TOPIARY_DEBUG("%d:default sectorsize %d\n", i, sectorsize);
                }
                else {
                    sectorsize = pentry->value.i;
                    _TOPIARY_DEBUG("%d:disksector sectorsize %d\n", i,
                                   sectorsize);
                }
            }
            else {
                sectorsize = 512;
                _TOPIARY_DEBUG("%d:default (no disksectors) sectorsize %d\n", i,
                               sectorsize);
            }
            _topiary_results_add_int(results, pkey, sectorsize);
        }
        _TOPIARY_DEBUG("%d:sectorsize %d\n", i, sectorsize);

/* sort out millisecond fields */
        float iodiff[nprecols];

        memset(iodiff, 0, sizeof iodiff);

        int j;

        for (j = 0; j < nprecols; j++) {        // ms_read, ms_write
            int k = precols[j];

            char *val = value1[k];

            if (val) {
                float secs = pars[k].scale * atoi(val);

                int n3 = n2;

                STRLCAT(pkey, pars[k].name, n3, sizeof(pkey));
                _TOPIARY_DEBUG("%2$d,%3$d:pKey %1$p '%1$s'\n", pkey, i, k);

                _TOPIARY_DEBUG("%d,%d:Curr %s = %f\n", i, k, pkey, secs);
                _topiary_results_add_fixed(results, pkey, secs, 0, 1);
                if (!presults)
                    continue;
                _TOPIARY_RESULTS_TABLE_T *pentry = NULL;

                int ret = _topiary_results_fetch(presults, pkey, &pentry);

                if (ret)
                    continue;

                _TOPIARY_DEBUG("%d,%d:Prev %s = %f\n", i, k, pentry->key,
                               pentry->value.f);
                iodiff[j] = secs - pentry->value.f;
                _TOPIARY_DEBUG("%d,%d:iodiff %s = %f\n", i, j, pkey, iodiff[j]);
            }
        }

        size_t ncols = current->vector->children[i]->length;
        int mincols = ncols > numcols ? numcols : ncols;

        _TOPIARY_DEBUG("%d:mincols %d\n", i, mincols);

        for (j = 0; j < mincols; j++) {
            _TOPIARY_DEBUG("%d,%d:'%s'\n", i, j, pars[j].name);
            if (j == KEYCOL) {
                _TOPIARY_DEBUG("%d,%d:Ignore '%s'\n", i, j, pars[j].name);
                continue;
            }

            int k;

            for (k = 0; k < nprecols; k++) {
                if (j == precols[k]) {
                    _TOPIARY_DEBUG("%d,%d:Ignore '%s'\n", i, j, pars[j].name);
                    break;
                }
            }
            if (k < nprecols) {
                _TOPIARY_DEBUG("%d,%d:Ignore '%s'\n", i, j,
                               pars[precols[k]].name);
                continue;
            }

            _TOPIARY_DEBUG("%d,%d:Process '%s'\n", i, j, pars[j].name);
            float out = 0.0;

            int n3 = n2;

            STRLCAT(pkey, pars[j].name, n3, sizeof(pkey));
            _TOPIARY_DEBUG("%2$d,%3$d:pKey %1$p '%1$s'\n", pkey, i, j);

            char *val = value1[j];

            _TOPIARY_DEBUG("%d,%d:Curr value %s = %s\n", i, j, pkey, val);
            if (!val)
                continue;

            if (j > NAMECOL) {
                out = pars[j].scale * atoi(val);
                if ((j == S_WRITECOL) || (j == S_READCOL)) {
                    out *= sectorsize;
                }
                _topiary_results_add_fixed(results, pkey, out, 0, 1);
                _TOPIARY_DEBUG("%d,%d:Curr %s = %f\n", i, j, pkey, out);
            }
            else {
                _topiary_results_add_int(results, pkey, atoi(val));
                _TOPIARY_DEBUG("%d,%d:Curr %s = %s\n", i, j, pkey, val);
            }

            if (!presults)
                continue;

            if ((j == READSCOL) || (j == MERGE_READCOL) || (j == S_READCOL)) {
                derived_values(i, j, results, presults, pkey, n3, sizeof(pkey),
                               out, iodiff[0]);
            }
            else if ((j == WRITESCOL) || (j == MERGE_WRITECOL)
                     || (j == S_WRITECOL)) {
                derived_values(i, j, results, presults, pkey, n3, sizeof(pkey),
                               out, iodiff[1]);
            }
            else if ((j == IOSCOL)) {
                derived_values(i, j, results, presults, pkey, n3, sizeof(pkey),
                               out, iodiff[2]);
            }
        }

    }
    return TOPIARY_OK;
}

int
_topiary_proc_diskstats_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{
    int ret;

    _TOPIARY_BASE_T *disksectors = NULL;

    ret = _topiary_sys_disksectors_new(&disksectors, NULL);
    if (ret) {
        return ret;
    }
    ret = _topiary_base_read(disksectors);
    if (ret) {
        return ret;
    }

    _TOPIARY_LIMITS_T *limits = NULL;

    ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 14, " ", 1);   /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/diskstats" };

    ret = _topiary_base_new(base, "proc_diskstats",
                            _TOPIARY_BASE_TYPE_PREVIOUS, NULL,
                            proc_diskstats_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
        TOPIARY_OPT_T *opt = NULL;

        topiary_opt_new(&opt);
        ret = _topiary_opt_set_results(opt, disksectors->current->results);
        ret = _topiary_base_set_optional(*base, opt);
        topiary_opt_free(&opt);
    }
    _TOPIARY_BASE_FREE(disksectors);
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
