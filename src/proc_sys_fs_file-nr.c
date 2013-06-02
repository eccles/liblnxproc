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

Typical contents of file /proc/sys_fs_file-nr::

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
proc_sys_fs_file_nr_normalize(_LNXPROC_BASE_T * base)
{
    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;
    char **values = (char **) array->vector->values;

    static const char *fields[] = { "allocated", "unused", "max", "used", };

    char buf[64];
    char *val;
    int n1 = 0;
    int n2 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    size_t ncols = array->vector->length;

    _LNXPROC_DEBUG("Ncols %zd\n", ncols);

    _lnxproc_results_init(results, ncols);
    int i, v;

    for (i = 0; i < 3; i++) {
        val = values[i];

        if (!val)
            continue;

        n2 = n1;

        STRLCAT(buf, fields[i], n2, sizeof(buf));
        _LNXPROC_DEBUG("Key %s, Value %s\n", buf, val);
        v = atoi(val);
        _lnxproc_results_add_int(results, buf, v);

    }
    n2 = n1;
    STRLCAT(buf, fields[3], n2, sizeof(buf));
    _LNXPROC_DEBUG("Key %s, Value %s\n", buf, val);
    v = atoi(values[0]) - atoi(values[1]);
    _lnxproc_results_add_int(results, buf, v);

    return LNXPROC_OK;
}

int
_lnxproc_proc_sys_fs_file_nr_new(_LNXPROC_BASE_T ** base, void *optional)
{

    char *filenames[] = { "/proc/sys/fs/file-nr", };
    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 1);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\t\f\n", 3);       /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret =
        _lnxproc_base_new(base, "proc_sys_fs_file-nr",
                          _LNXPROC_BASE_TYPE_VANILLA, filenames, 1, NULL, NULL,
                          NULL, NULL, proc_sys_fs_file_nr_normalize, NULL, 64,
                          limits);
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
