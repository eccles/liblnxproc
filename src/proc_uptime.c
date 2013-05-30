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

Typical contents of file /proc/uptime::

BOOT_IMAGE=/vmlinuz-3.8.0-21-generic root=/dev/mapper/ubuntu-root ro quiet splash vt.handoff=7

*/

#include <stdlib.h>
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"
#include "base_private.h"

static int
proc_uptime_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;
    char **values = (char **) array->vector->values;

    static const char *titles[] = { "uptime", "idle", };

    size_t ncols = array->vector->length;
    size_t n = ncols > 2 ? 2 : ncols;

    _lnxproc_results_init(results, ncols);
    int i;

    for (i = 0; i < n; i++) {
        char *val = values[i];

        if (!val)
            continue;

        char buf[64];

        int n = 0;

        STRLCAT(buf, "/", n, sizeof(buf));
        STRLCAT(buf, titles[i], n, sizeof(buf));
        _LNXPROC_DEBUG("Key %s, Value %s\n", buf, val);
        _lnxproc_results_add_float(results, buf, atof(val));

    }
    return LNXPROC_OK;
}

int
_lnxproc_proc_uptime_new(_LNXPROC_BASE_T ** base, void *optional)
{

    char *filenames[] = { "/proc/uptime", };
    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 1);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 2, " \n", 2);  /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_base_new(base, "proc_uptime", _LNXPROC_BASE_TYPE_VANILLA,
                            filenames, 1, NULL, NULL, NULL,
                            NULL, proc_uptime_normalize, NULL, 64, limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
