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

Typical contents of file /proc/cmdline::

BOOT_IMAGE=/vmlinuz-3.8.0-21-generic root=/dev/mapper/ubuntu-root ro quiet splash vt.handoff=7

*/

#include "config.h"

#include "topiary_stdlib.h"
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_cmdline_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;
    char **values = (char **) array->vector->values;

    size_t ncols = array->vector->length;

    _topiary_results_init(results, ncols);
    int i;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    for (i = 0; i < ncols; i++) {
        char *val = values[i];

        if (!val)
            continue;

        int n2 = n1;

        INTCAT(buf, i, n2, sizeof(buf));
        _TOPIARY_DEBUG("Key %s, Value %s\n", buf, val);
        _topiary_results_add_stringref(results, buf, val);

    }
    return TOPIARY_OK;
}

int
_topiary_proc_cmdline_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    char *filenames[] = { "/proc/cmdline", };
    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 1);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, " \f\n", 3);        /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_base_new(base, "proc_cmdline", _TOPIARY_BASE_TYPE_MEMOIZE,
                            NULL, proc_cmdline_normalize, NULL, 64, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
