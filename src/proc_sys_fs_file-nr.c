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

Typical contents of file /proc/sys_fs_file-nr::

BOOT_IMAGE=/vmlinuz-3.8.0-21-generic root=/dev/mapper/ubuntu-root ro quiet splash vt.handoff=7

*/

#include <stdlib.h>
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_sys_fs_file_nr_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;
    char **values = (char **) array->vector->values;

    static const char *fields[] = { "allocated", "unused", "max", "used", };

    char buf[64];
    char *val;
    int n1 = 0;
    int n2 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    size_t ncols = array->vector->length;

    _TOPIARY_DEBUG("Ncols %zd\n", ncols);

    _topiary_results_init(results, ncols);
    int i;
    unsigned long v;

    for (i = 0; i < 3; i++) {
        val = values[i];

        if (!val)
            continue;

        n2 = n1;

        STRLCAT(buf, fields[i], n2, sizeof(buf));
        _TOPIARY_DEBUG("Key %s, Value %s\n", buf, val);
        v = strtoul(val, NULL, 0);
        _topiary_results_add_unsigned_long(results, buf, v);

    }
    n2 = n1;
    STRLCAT(buf, fields[3], n2, sizeof(buf));
    _TOPIARY_DEBUG("Key %s, Value %s\n", buf, val);
    v = strtoul(values[0], NULL, 0) - strtoul(values[1], NULL, 0);
    _topiary_results_add_unsigned_long(results, buf, v);

    return TOPIARY_OK;
}

int
_topiary_proc_sys_fs_file_nr_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional)
{

    char *filenames[] = { "/proc/sys/fs/file-nr", };
    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 1);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\t\f\n", 3);       /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret =
        _topiary_base_new(base, "proc_sys_fs_file-nr",
                          _TOPIARY_BASE_TYPE_VANILLA,
                          NULL, proc_sys_fs_file_nr_normalize, NULL, 64,
                          limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
