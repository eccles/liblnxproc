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

#ifndef LIBLNXPROC_VECTOR_H
#define LIBLNXPROC_VECTOR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "error.h"

    struct lnxproc_vector_t;
    typedef struct lnxproc_vector_t LNXPROC_VECTOR_T;

    union lnxproc_vector_data_t;
    typedef union lnxproc_vector_data_t LNXPROC_VECTOR_DATA_T;

    LNXPROC_VECTOR_T *lnxproc_data_child(LNXPROC_VECTOR_DATA_T * val);
    char *lnxproc_data_value(LNXPROC_VECTOR_DATA_T * val);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_VECTOR_T *lnxproc_vector_new(size_t size,
                                         int recursive,
                                         LNXPROC_ERROR_CALLBACK callback)
     WARN_UNUSED;

    LNXPROC_VECTOR_T *lnxproc_vector_free(LNXPROC_VECTOR_T *
                                          vector) WARN_UNUSED;
    int lnxproc_vector_resize(LNXPROC_VECTOR_T * vector, size_t size);

    int lnxproc_vector_set_child(LNXPROC_VECTOR_T * vector, size_t idx,
                                 LNXPROC_VECTOR_T * child);
    int lnxproc_vector_set_value(LNXPROC_VECTOR_T * vector, size_t idx,
                                 char *val);

    int lnxproc_vector_set_last_child(LNXPROC_VECTOR_T * vector, size_t idx,
                                      LNXPROC_VECTOR_T * child);
    int lnxproc_vector_set_last_value(LNXPROC_VECTOR_T * vector, size_t idx,
                                      char *val);

    int lnxproc_vector_set_length(LNXPROC_VECTOR_T * vector, size_t idx);
    LNXPROC_VECTOR_T *lnxproc_vector_child(LNXPROC_VECTOR_T * vector,
                                           size_t idx);
    char *lnxproc_vector_value(LNXPROC_VECTOR_T * vector, size_t idx);

    size_t lnxproc_vector_size(LNXPROC_VECTOR_T * vector);

    LNXPROC_ERROR_CALLBACK lnxproc_vector_callback(LNXPROC_VECTOR_T * vector);

    typedef int (*LNXPROC_VECTOR_ITERATE_FUNC) (LNXPROC_VECTOR_DATA_T * entry,
                                                int recursive, void *data,
                                                size_t idx);

    int lnxproc_vector_iterate(LNXPROC_VECTOR_T * vector,
                               void *data,
                               int start,
                               int end, LNXPROC_VECTOR_ITERATE_FUNC func);

    int lnxproc_vector_print(LNXPROC_VECTOR_T * vector, int allocated,
                             void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_VECTOR_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
