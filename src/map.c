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
#include "map_limits.h"
#include "array.h"

LNXPROC_ARRAY_T *
lnxproc_map_create(LNXPROC_ERROR_CALLBACK callback,
                   LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim, int depth)
{
    LNXPROC_DEBUG("callback %p\n", callback);
    LNXPROC_DEBUG("maplimits %p\n", maplimits);
    LNXPROC_DEBUG("mapdim %d\n", mapdim);
    LNXPROC_DEBUG("depth %d\n", depth);

    lnxproc_map_limits_print(maplimits, mapdim);

    LNXPROC_MAP_LIMITS_T *maplimit = maplimits + depth;

    LNXPROC_DEBUG("maplimit %p\n", maplimit);
#ifdef DEBUG
    char buf[64];

    LNXPROC_DEBUG("maplimit %s\n",
                  lnxproc_map_limit_print(maplimit, buf, sizeof buf));
#endif

    int recursive = depth < mapdim - 1 ? 1 : 0;

    LNXPROC_DEBUG("recursive %d\n", recursive);

    LNXPROC_ARRAY_T *wmap =
        lnxproc_array_new(maplimit->expected, recursive, callback);

    int i;

    if (recursive) {
        depth++;
        for (i = 0; i < maplimit->expected; i++) {
            LNXPROC_ARRAY_T *f =
                lnxproc_map_create(callback, maplimits, mapdim, depth);
            lnxproc_array_set_last(wmap, i, f);
        }
    }
    return wmap;

}

static int
map_split_func(LNXPROC_ERROR_CALLBACK callback,
               LNXPROC_ARRAY_T *map,
               LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim,
               int idx[], int depth, char *saveptr)
{

    void *savedarray[mapdim];

    savedarray[0] = map;
    int i;

    for (i = 1; i < mapdim; i++) {
        LNXPROC_ARRAY_T *f = lnxproc_array_get(savedarray[i - 1], idx[i - 1]);

        if (!f) {
            LNXPROC_MAP_LIMITS_T *maplimit = maplimits + i;
            int recursive = i < mapdim - 1 ? 1 : 0;

            f = lnxproc_array_new(maplimit->expected, recursive, callback);
            /* TODO = trap null return valie */
            if (recursive)
                lnxproc_array_set_last(savedarray[i - 1], idx[i - 1], f);
        }
        savedarray[i] = f;
    }
    lnxproc_array_set_last(savedarray[i - 1], idx[i - 1], saveptr);

    return LNXPROC_OK;
}

int
lnxproc_map_split(LNXPROC_ARRAY_T *map,
                  LNXPROC_ERROR_CALLBACK callback,
                  LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim,
                  char *lines, int nbytes)
{

    if (nbytes > 0) {
        char *c = lines;
        char *d = c + nbytes;

        char *saveptr = c;

        if (mapdim > 0) {
            int idx[mapdim];

            memset(idx, 0, mapdim * sizeof(int));

            while (c < d) {
                int i;
                int increment = 1;

                for (i = 0; i < mapdim; i++) {
                    if (lnxproc_map_chr(maplimits + i, *c)) {
                        *c = '\0';

                        map_split_func(callback, map, maplimits, mapdim,
                                       idx, i, saveptr);

                        while ((++c < d) && lnxproc_map_chr(maplimits + i, *c));

                        saveptr = c;
                        idx[i]++;
                        int j = i + 1;

                        if (mapdim > j) {
                            memset(idx + j, 0, (mapdim - j) * sizeof(int));
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
