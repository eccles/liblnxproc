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

Typical contents of file /proc/sys/kernel/hostname::

    Feynmann

*/

#include "config.h"

#include "topiary_stdlib.h"
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
