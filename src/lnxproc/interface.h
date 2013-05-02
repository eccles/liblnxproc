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

#ifndef LIBLNXPROC_INTERFACE_H
#define LIBLNXPROC_INTERFACE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <lnxproc/util.h>
#include <lnxproc/error.h>
#include <lnxproc/limits.h>
#include <lnxproc/results.h>
#include <lnxproc/base.h>

    typedef struct lnxproc_interface_t LNXPROC_INTERFACE_T;

    struct lnxproc_interface_t {
        LNXPROC_READ_METHOD read;
        LNXPROC_BASE_T *base;
    };

    LNXPROC_INTERFACE_T *lnxproc_interface_free(LNXPROC_INTERFACE_T *
                                                interface) WARN_UNUSED;

#define LNXPROC_INTERFACE_FREE(b) {\
    b = lnxproc_interface_free(b);\
}

    LNXPROC_ERROR_T lnxproc_interface_new(LNXPROC_INTERFACE_T ** interface,
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

    typedef LNXPROC_ERROR_T (*LNXPROC_INTERFACE_METHOD) (LNXPROC_INTERFACE_T **
                                                         interface);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
