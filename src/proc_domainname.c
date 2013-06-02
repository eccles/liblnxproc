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

Typical contents of file /proc/sys/kernel/domainname::

    (none)

.. note::
   This class is **cached**. The file is only read **once**. Subsequent reads
   return the first value read

'''

*/

#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "results_private.h"
#include "base_private.h"

static int
proc_domainname_normalize(_LNXPROC_BASE_T * base)
{
    _lnxproc_results_init(base->current->results, 1);
    _lnxproc_results_add_stringref(base->current->results, "/value",
                                   base->current->lines);

    return LNXPROC_OK;
}

int
_lnxproc_proc_domainname_new(_LNXPROC_BASE_T ** base, void *optional)
{

    char *filenames[] = { "/proc/sys/kernel/domainname" };
    int ret = _lnxproc_base_new(base, "proc_domainname",
                                _LNXPROC_BASE_TYPE_MEMOIZE, NULL,
                                proc_domainname_normalize,
                                NULL,
                                64, NULL);

    if (!ret) {
        ret = _lnxproc_base_set_filenames(*base, filenames, 1);
    }
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
