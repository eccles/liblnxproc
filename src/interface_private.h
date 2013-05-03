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

#ifndef LIBLNXPROC_INTERFACE_PRIVATE_H
#define LIBLNXPROC_INTERFACE_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "util_private.h"
#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "base_private.h"
#include <lnxproc/interface.h>

    LNXPROC_ERROR_T _lnxproc_interface_new(LNXPROC_INTERFACE_T ** interface,
                                           char **filenames,
                                           size_t nfiles,
                                           char *fileprefix,
                                           char *filesuffix,
                                           LNXPROC_BASE_METHOD rawread,
                                           LNXPROC_BASE_METHOD normalize,
                                           LNXPROC_READ_METHOD read,
                                           size_t buflen,
                                           LNXPROC_LIMITS_T limits[],
                                           size_t dim);

    typedef LNXPROC_ERROR_T (*_LNXPROC_INTERFACE_METHOD) (LNXPROC_INTERFACE_T **
                                                          interface);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_INTERFACE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
