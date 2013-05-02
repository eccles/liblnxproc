/*
 * This file is part of liblnxproc.
 *
 *  liblnxproc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  liblnxproc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#include <stdlib.h>
#include "error_private.h"
#include "base_private.h"
#include "interface_private.h"

LNXPROC_ERROR_T
_lnxproc_interface_new(LNXPROC_INTERFACE_T ** interface,
                      char **filenames,
                      size_t nfiles,
                      char *fileprefix,
                      char *filesuffix,
                      LNXPROC_BASE_METHOD rawread,
                      LNXPROC_BASE_METHOD normalize,
                      LNXPROC_READ_METHOD read,
                      size_t buflen, LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_DEBUG("rawread %p, normalize %p, read %p\n", rawread,
                  normalize, read);
    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof LNXPROC_INTERFACE_T %d\n",
                  sizeof(LNXPROC_INTERFACE_T));

    LNXPROC_INTERFACE_T *p = calloc(1, sizeof(LNXPROC_INTERFACE_T));

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE,
                            "Malloc interface\n");
        return LNXPROC_ERROR_INTERFACE_MALLOC_INTERFACE;
    }

    LNXPROC_ERROR_T ret = lnxproc_base_new(&p->base,
                                           filenames,
                                           nfiles,
                                           fileprefix,
                                           filesuffix,
                                           rawread,
                                           normalize,
                                           read,
                                           buflen, limits, dim);

    if (ret) {
        LNXPROC_INTERFACE_FREE(p);
        return ret;
    }
    if (read) {
        p->read = read;
    }
    else {
        p->read = lnxproc_base_read;
    }
    *interface = p;
    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_INTERFACE_T *
lnxproc_interface_free(LNXPROC_INTERFACE_T * interface)
{
    LNXPROC_DEBUG("Base %p\n", interface);

    if (interface) {
        if (interface->base) {
            LNXPROC_DEBUG("Free base \n");
            LNXPROC_BASE_FREE(interface->base);
        }

        LNXPROC_DEBUG("Free Interface\n");
        free(interface);
        interface = NULL;
    }

    return interface;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
