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

    struct lnxproc_base_t;

    enum lnxproc_base_error_t {
        LNXPROC_BASE_OK = 0,
        LNXPROC_BASE_ERROR_NULL,
        LNXPROC_BASE_ERROR_SYSTEM,
        LNXPROC_BASE_ERROR_MALLOC_BASE,
        LNXPROC_BASE_ERROR_MALLOC_BUFFER,
    };

    void lnxproc_base_error_print_callback(const char *func,
                                           enum lnxproc_base_error_t err);
    void (*lnxproc_base_error_callback) (const char *func,
                                         enum lnxproc_base_error_t err);

    typedef int (*LNXPROC_BASE_METHOD) (struct lnxproc_base_t * base);

    const char *lnxproc_base_filename(struct lnxproc_base_t *base);
    char *lnxproc_base_lines(struct lnxproc_base_t *base);
    void *lnxproc_base_map(struct lnxproc_base_t *base);
    int lnxproc_base_map_set(struct lnxproc_base_t *base, void *map);
    int lnxproc_base_nbytes(struct lnxproc_base_t *base);
    int lnxproc_base_rawread(struct lnxproc_base_t *base);
    int lnxproc_base_read(struct lnxproc_base_t *base);
    int lnxproc_base_normalize(struct lnxproc_base_t *base);
    struct lnxproc_base_t *lnxproc_base_init(const char *filename,
                                             LNXPROC_BASE_METHOD rawread,
                                             LNXPROC_BASE_METHOD normalize,
                                             LNXPROC_BASE_METHOD read,
                                             size_t buflen, void *data);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    struct lnxproc_base_t *lnxproc_base_free(struct lnxproc_base_t *base)
     WARN_UNUSED;

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab 
 */
#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif
