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

Typical contents of file /proc/sys/kernel/hostname::

    Feynmann

*/

#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "results_private.h"
#include "base_private.h"
#include "proc_hostname.h"

static int
proc_hostname_normalize(_LNXPROC_BASE_T * base)
{
    _lnxproc_results_init(base->current->results, 1);
    _lnxproc_results_add_string(base->current->results, "/value",
                                base->current->lines,
                                strlen(base->current->lines));

    return LNXPROC_OK;
}

int
_lnxproc_proc_hostname_new(_LNXPROC_BASE_T ** base, void *optional)
{

    char *filenames[] = { "/proc/sys/kernel/hostname", };
    return _lnxproc_base_new(base, "proc_hostname", _LNXPROC_BASE_TYPE_MEMOIZE,
                             filenames, 1, NULL, NULL, NULL,
                             NULL, proc_hostname_normalize, NULL, 64, NULL);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
