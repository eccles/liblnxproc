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

   Reads the file /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq

.. note::
   This class is **cached**. The file is only read **once**. Subsequent reads
   return the first value read

'''
*/

#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "base_private.h"
#include "results_private.h"
#include "sys_cpufreq.h"

static LNXPROC_ERROR_T
sys_cpufreq_normalize(LNXPROC_BASE_T *base)
{
    _lnxproc_results_init(base->current->results, 1);
    _LNXPROC_RESULTS_TABLE_T entry;

    strcpy(entry.key, "/value");
    strcpy(entry.value, base->current->lines);
    _lnxproc_results_add(base->current->results, &entry);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_sys_cpufreq_new(LNXPROC_BASE_T **base)
{

    char *filenames[] =
        { "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", };
    return _lnxproc_base_new(base, LNXPROC_BASE_TYPE_MEMOIZE, filenames, 1,
                             NULL, NULL, NULL, NULL, sys_cpufreq_normalize,
                             NULL, 64, NULL, 0);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
