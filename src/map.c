/*
This file is part of liblnxproc.

 liblnxproc is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 liblnxproc is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Paul Hewlett, phewlett76@gmail.com

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "limits.h"
#include "vector.h"

LNXPROC_VECTOR_T *
lnxproc_create(LNXPROC_ERROR_CALLBACK callback,
               LNXPROC_LIMITS_T limits[], size_t dim, int depth)
{
    LNXPROC_DEBUG("callback %p\n", callback);
    LNXPROC_DEBUG("limits %p\n", limits);
    LNXPROC_DEBUG("dim %d\n", dim);
    LNXPROC_DEBUG("depth %d\n", depth);

    LNXPROC_LIMITS_T *limit = limits + depth;

    LNXPROC_DEBUG("limit %p\n", limit);
#ifdef DEBUG
    char buf[64];

    LNXPROC_DEBUG("limit %s\n", lnxproc_limit_print(limit, buf, sizeof buf));
#endif

    int recursive = depth < dim - 1 ? 1 : 0;

    LNXPROC_DEBUG("recursive %d\n", recursive);

    LNXPROC_VECTOR_T *wvec =
        lnxproc_vector_new(limit->expected, recursive, callback);

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < limit->expected; i++) {
            LNXPROC_VECTOR_T *f = lnxproc_create(callback, limits, dim, depth);

            if (!f) {
                return NULL;
            }
            lnxproc_vector_set_last(wvec, i, f);
        }
    }
    return wvec;

}

static int
map_split_func(LNXPROC_ERROR_CALLBACK callback,
               LNXPROC_VECTOR_T * map,
               LNXPROC_LIMITS_T limits[], size_t dim,
               int idx[], int depth, char *saveptr)
{

    void *savedvector[dim];

    savedvector[0] = map;
    int i;

    for (i = 1; i < dim; i++) {
        LNXPROC_VECTOR_T *f =
            lnxproc_vector_get(savedvector[i - 1], idx[i - 1]);

        if (!f) {
            LNXPROC_LIMITS_T *limit = limits + i;
            int recursive = i < dim - 1 ? 1 : 0;

            f = lnxproc_vector_new(limit->expected, recursive, callback);
            if (!f) {
                return 1;
            }
            if (recursive)
                lnxproc_vector_set_last(savedvector[i - 1], idx[i - 1], f);
        }
        savedvector[i] = f;
    }
    lnxproc_vector_set_last(savedvector[i - 1], idx[i - 1], saveptr);

    return LNXPROC_OK;
}

int
lnxproc_split(LNXPROC_VECTOR_T * map,
              LNXPROC_ERROR_CALLBACK callback,
              LNXPROC_LIMITS_T limits[], size_t dim, char *lines, int nbytes)
{

    if (nbytes > 0) {
        char *c = lines;
        char *d = c + nbytes;

        char *saveptr = c;

        if (dim > 0) {
            int idx[dim];

            memset(idx, 0, dim * sizeof(int));

            while (c < d) {
                int i;
                int increment = 1;

                for (i = 0; i < dim; i++) {
                    if (lnxproc_chr(limits + i, *c)) {
                        *c = '\0';

                        int ret = map_split_func(callback, map, limits, dim,
                                                 idx, i, saveptr);

                        if (ret) {
                            return ret;
                        }

                        while ((++c < d) && lnxproc_chr(limits + i, *c));

                        saveptr = c;
                        idx[i]++;
                        int j = i + 1;

                        if (dim > j) {
                            memset(idx + j, 0, (dim - j) * sizeof(int));
                        }

                        increment = 0;
                    }
                }
                if (increment) {
                    c++;
                }
            }
        }
        else {
            while (c < d) {
                if (strchr("\n", *c)) {
                    *c = '\0';
                    break;
                }
                c++;
            }
        }
    }

    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
