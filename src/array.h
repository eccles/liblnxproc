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

    struct lnxproc_array_t;

    typedef int (*LNXPROC_ARRAY_ITERATE_FUNC) (struct lnxproc_array_t * array,
                                               void *data, int idx);

    enum lnxproc_array_error_t {
        LNXPROC_ARRAY_OK = 0,
        LNXPROC_ARRAY_ERROR_MALLOC_HEADER,
        LNXPROC_ARRAY_ERROR_MALLOC_DATA,
        LNXPROC_ARRAY_ERROR_NULL,
        LNXPROC_ARRAY_ERROR_REALLOC_DATA,
        LNXPROC_ARRAY_ERROR_INDEX_OUT_OF_RANGE,
    };

    void lnxproc_array_error_print_callback(const char *func,
                                            enum lnxproc_array_error_t err);
    void (*lnxproc_array_error_callback) (const char *func,
                                          enum lnxproc_array_error_t err);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    struct lnxproc_array_t *lnxproc_array_new(size_t size,
                                              int recursive) WARN_UNUSED;
    struct lnxproc_array_t *lnxproc_array_free(struct lnxproc_array_t *array)
     WARN_UNUSED;
    int lnxproc_array_resize(struct lnxproc_array_t *array, size_t size);
    int lnxproc_array_set(struct lnxproc_array_t *array, size_t idx, void *val);
    int lnxproc_array_set_last(struct lnxproc_array_t *array, size_t idx,
                               void *val);
    int lnxproc_array_append(struct lnxproc_array_t *array, void *val);

    int lnxproc_array_set_length(struct lnxproc_array_t *array, size_t idx);
    void *lnxproc_array_addr(struct lnxproc_array_t *array, size_t idx);
    void *lnxproc_array_get(struct lnxproc_array_t *array, size_t idx);
    size_t lnxproc_array_size(struct lnxproc_array_t *array);

    int lnxproc_array_iterate(struct lnxproc_array_t *array,
                              void *data,
                              int start,
                              int end, LNXPROC_ARRAY_ITERATE_FUNC func);

    int lnxproc_array_print(struct lnxproc_array_t *array, void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ARRAY_H
/* 
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab 
 */
