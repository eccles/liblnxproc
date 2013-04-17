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

Typical contents of file /proc/sys/kernel/osrelease::

    3.5.0-19-generic

.. note::
   This class is **cached**. The file is only read **once**. Subsequent reads
   return the first value read

'''
    '''
    Measures the osrelease file from /proc/sys/kernel filesystem
    '''
    def __init__(self, **kwds):
        '''
        Initialises object fields
        '''

        kwds['filename'] = 'proc/sys/kernel/osrelease'
        kwds['fields'] = 'osrelease'

        super(ProcSysKernelOsrelease, self).__init__(**kwds)

    def normalize(self, timestamp, lines):
        '''
        The sys/kernel/osrelease file is a single value
        '''
        data = {}
        data[self.fields] = lines[0].rstrip()
        return data

if __name__ == "__main__":
    from Test import Test
    Test(ProcSysKernelOsrelease)

*/

#include <stdlib.h>
#include <string.h>

#include "base_private.h"
#include <lnxproc/proc_osrelease.h>

static LNXPROC_ERROR_T
proc_osrelease_normalize(LNXPROC_BASE_T *base)
{
    lnxproc_results_store(base->results, base->lines, "/value");

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_osrelease_new(LNXPROC_BASE_T **base)
{

    return lnxproc_base_new(base,
                            "/proc/sys/kernel/osrelease",
                            NULL, proc_osrelease_normalize, NULL, 32, NULL, 0);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
