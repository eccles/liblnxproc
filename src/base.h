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

#ifndef LIBLNXPROC_BASE_H
#define LIBLNXPROC_BASE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "error.h"
#include "limits.h"
#include "array.h"

    struct lnxproc_base_t;
    typedef struct lnxproc_base_t LNXPROC_BASE_T;

    typedef int (*LNXPROC_BASE_METHOD) (LNXPROC_BASE_T *base);
    typedef int (*LNXPROC_NORMALIZE_METHOD) (LNXPROC_BASE_T *base);

    LNXPROC_LIMITS_T *lnxproc_base_map_limits(LNXPROC_BASE_T *base);
    int lnxproc_base_map_dim(LNXPROC_BASE_T *base);

    LNXPROC_ARRAY_T *lnxproc_base_map(LNXPROC_BASE_T *base);

    const char *lnxproc_base_filename(LNXPROC_BASE_T *base);
    char *lnxproc_base_lines(LNXPROC_BASE_T *base);
    LNXPROC_ERROR_CALLBACK lnxproc_base_callback(LNXPROC_BASE_T *base);

    int lnxproc_base_rawread(LNXPROC_BASE_T *base);
    int lnxproc_base_read(LNXPROC_BASE_T *base);
    int lnxproc_base_normalize(LNXPROC_BASE_T *base);

    int lnxproc_base_nbytes(LNXPROC_BASE_T *base);

    LNXPROC_BASE_T *lnxproc_base_init(const char *filename,
                                      LNXPROC_BASE_METHOD rawread,
                                      LNXPROC_NORMALIZE_METHOD normalize,
                                      LNXPROC_BASE_METHOD read,
                                      LNXPROC_ERROR_CALLBACK callback,
                                      size_t buflen,
                                      LNXPROC_LIMITS_T maplimits[],
                                      size_t mapdim, void *data);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_BASE_T *lnxproc_base_free(LNXPROC_BASE_T *base) WARN_UNUSED;

    int lnxproc_base_print(LNXPROC_BASE_T *base, int allocated, void *data);
    int lnxproc_base_map_print(LNXPROC_BASE_T *base, void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
