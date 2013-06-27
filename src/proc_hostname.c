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

Typical contents of file /proc/sys/kernel/hostname::

    Feynmann

*/

#include "config.h"

#include "stdlib.h"
#include <string.h>

#include "error_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_hostname_normalize(_TOPIARY_BASE_T *base)
{
    _topiary_results_init(base->current->results, 1);
    _topiary_results_add_stringref(base->current->results, "/value",
                                   base->current->lines);

    return TOPIARY_OK;
}

int
_topiary_proc_hostname_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    char *filenames[] = { "/proc/sys/kernel/hostname", };
    int ret =
        _topiary_base_new(base, "proc_hostname", _TOPIARY_BASE_TYPE_MEMOIZE,
                          NULL, proc_hostname_normalize, NULL, 64, NULL);

    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
