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

/* This file is a re-implementation of the 'list' type from Python
 */

#ifndef LIBLNXPROC_ARRAY_H
#define LIBLNXPROC_ARRAY_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "limits.h"

    struct lnxproc_array_t;
    typedef struct lnxproc_array_t LNXPROC_ARRAY_T;

    LNXPROC_ERROR_T lnxproc_array_new(LNXPROC_ARRAY_T **array,
                                      LNXPROC_LIMITS_T limits[], size_t dim);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_ARRAY_T *lnxproc_array_free(LNXPROC_ARRAY_T *array) WARN_UNUSED;

#define LNXPROC_ARRAY_FREE(a) {\
    a = lnxproc_array_free(a);\
}

    LNXPROC_ERROR_T lnxproc_array_set(LNXPROC_ARRAY_T *array, size_t idx[],
                                      size_t dim, char *val);
    LNXPROC_ERROR_T lnxproc_array_set_last(LNXPROC_ARRAY_T *array, size_t idx[],
                                           size_t dim, char *val);
    LNXPROC_ERROR_T lnxproc_array_get(LNXPROC_ARRAY_T *array, size_t idx[],
                                      size_t dim, char **value);

    typedef LNXPROC_ERROR_T (*LNXPROC_ARRAY_ITERATE_FUNC) (char *val,
                                                           void *data,
                                                           size_t idx[],
                                                           size_t dim);

    LNXPROC_ERROR_T lnxproc_array_iterate(LNXPROC_ARRAY_T *array,
                                          void *data,
                                          LNXPROC_ARRAY_ITERATE_FUNC func);

    LNXPROC_ERROR_T lnxproc_array_print(LNXPROC_ARRAY_T *array, int allocated,
                                        void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ARRAY_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
