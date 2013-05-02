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

#ifndef LIBLNXPROC_BASE_PRIVATE_H
#define LIBLNXPROC_BASE_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "array_private.h"
#include <lnxproc/base.h>

    typedef LNXPROC_ERROR_T (*LNXPROC_BASE_METHOD) (LNXPROC_BASE_T *base);
    typedef LNXPROC_RESULTS_T *(*LNXPROC_READ_METHOD) (LNXPROC_BASE_T *base);

    struct lnxproc_base_t {
        LNXPROC_BASE_METHOD rawread;
        LNXPROC_BASE_METHOD normalize;
        LNXPROC_READ_METHOD read;
        char **filenames;
        size_t nfiles;
        char *fileprefix;
        char *filesuffix;
        char *lines;
        size_t buflen;
        int nbytes;
        LNXPROC_ARRAY_T *array;
        LNXPROC_RESULTS_T *results;
        LNXPROC_RESULTS_T *prev;
    };

    LNXPROC_ERROR_T lnxproc_base_new(LNXPROC_BASE_T **base,
                                     char **filenames,
                                     size_t nfiles,
                                     char *fileprefix,
                                     char *filesuffix,
                                     LNXPROC_BASE_METHOD rawread,
                                     LNXPROC_BASE_METHOD normalize,
                                     LNXPROC_READ_METHOD read,
                                     size_t buflen,
                                     LNXPROC_LIMITS_T limits[], size_t dim);

    LNXPROC_ERROR_T lnxproc_base_array(LNXPROC_BASE_T *base,
                                       LNXPROC_ARRAY_T **array);

    LNXPROC_ERROR_T lnxproc_base_filename(LNXPROC_BASE_T *base,
                                          char **filename);
    LNXPROC_ERROR_T lnxproc_base_lines(LNXPROC_BASE_T *base, char **lines);

    LNXPROC_ERROR_T lnxproc_base_rawread(LNXPROC_BASE_T *base);
    LNXPROC_ERROR_T lnxproc_base_normalize(LNXPROC_BASE_T *base);

    LNXPROC_ERROR_T lnxproc_base_nbytes(LNXPROC_BASE_T *base, int *nbytes);

    LNXPROC_ERROR_T lnxproc_base_print(LNXPROC_BASE_T *base);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
