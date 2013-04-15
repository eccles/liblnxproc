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

#include "error.h"
#include "results.h"
#include "array_private.h"
#include "base.h"

    struct lnxproc_base_t {
        LNXPROC_BASE_METHOD rawread;
        LNXPROC_BASE_METHOD normalize;
        LNXPROC_BASE_METHOD read;
        LNXPROC_ERROR_CALLBACK callback;
        const char *filename;
        char *lines;
        size_t buflen;
        int nbytes;
        LNXPROC_ARRAY_T *array;
        LNXPROC_RESULTS_T *results;
        LNXPROC_RESULTS_T *prev;
    };

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_BASE_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
