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

#ifndef LIBLNXPROC_ARRAY_PRIVATE_H
#define LIBLNXPROC_ARRAY_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"
#include "vector_private.h"
#include "limits.h"
#include "array.h"

    struct lnxproc_array_t {
        LNXPROC_ERROR_CALLBACK callback;
        LNXPROC_LIMITS_T *limits;
        size_t dim;
        void **saved;
        LNXPROC_VECTOR_T *data;
    };

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_ARRAY_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
