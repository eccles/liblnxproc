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

Typical contents of file /proc/uptime::


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
proc_uptime_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;
    char **values = (char **) array->vector->values;

    static const char *titles[] = { "uptime", "idle", };

    size_t ncols = array->vector->length;
    size_t mincol = ncols > 2 ? 2 : ncols;

    _topiary_results_init(results, ncols);
    int i;

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    for (i = 0; i < mincol; i++) {
        char *val = values[i];

        if (!val)
            continue;

        int n2 = n1;

        STRLCAT(buf, titles[i], n2, sizeof(buf));
        _TOPIARY_DEBUG("Key %s, Value %s\n", buf, val);
        _topiary_results_add_fixed(results, buf, atof(val), 0, 1);

    }
    return TOPIARY_OK;
}

int
_topiary_proc_uptime_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    char *filenames[] = { "/proc/uptime", };
    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 1);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 2, " \n\f", 3);        /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_base_new(base, "proc_uptime", _TOPIARY_BASE_TYPE_VANILLA,
                            NULL, proc_uptime_normalize, NULL, 64, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
