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

from basecache import BaseCache

class ProcSysKernelDomainname(BaseCache):
    '''
    Measures the domainname file from /proc/sys/kernel filesystem
    '''
    def __init__(self, **kwds):
        '''
        Initialises object fields
        '''

        kwds['filename'] = 'proc/sys/kernel/domainname'
        kwds['fields'] = 'domainname'

        super(ProcSysKernelDomainname, self).__init__(**kwds)

    def normalize(self, timestamp, lines):
        '''
        The sys/kernel/domainname file is a single value
        '''
        data = {}
        data[self.fields] = lines[0].rstrip()
        return data

if __name__ == "__main__":
    from Test import Test
    Test(ProcSysKernelDomainname)

*/

#include <stdlib.h>
#include <string.h>

#include "base_private.h"
#include <lnxproc/proc_domainname.h>

static LNXPROC_ERROR_T
proc_domainname_normalize(LNXPROC_BASE_T *base)
{
    lnxproc_results_store(base->results, base->lines, "/value");

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_domainname_new(LNXPROC_BASE_T **base)
{

    return lnxproc_base_new(base,
                            "/proc/sys/kernel/domainname",
                            NULL, proc_domainname_normalize, NULL, 64, NULL, 0);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
