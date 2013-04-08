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
#include "base.h"
#include "array.h"

static LNXPROC_ARRAY_T *
map_split_add_array(LNXPROC_BASE_T *base,
                    LNXPROC_ARRAY_T *wmap, int arraydim, int idx, int recursive)
{
    LNXPROC_ARRAY_T *f = lnxproc_array_get(wmap, idx);

    if (!f) {
        f = lnxproc_array_new(arraydim, recursive, lnxproc_base_callback(base));

        if (!f) {
            return NULL;
        }

        lnxproc_array_set_last(wmap, idx, f);
    }

    return f;
}

static int
map_split_func(LNXPROC_BASE_T *base, int *arraydims, int *idx, size_t idxlen,
               char *val)
{

    int map_recursive = idxlen > 1 ? 1 : 0;

    LNXPROC_ARRAY_T *map = lnxproc_base_map(base);

    if (!map) {
        map =
            lnxproc_array_new(arraydims[0], map_recursive,
                              lnxproc_base_callback(base));

        if (!map) {
            return 1;
        }

        lnxproc_base_map_set(base, map);
    }

    if (!map_recursive) {
        lnxproc_array_set_last(map, idx[0], val);
        return 0;
    }

    LNXPROC_ARRAY_T *wmap = map;
    int i;

    for (i = 0; i < idxlen - 2; i++) {
        wmap = map_split_add_array(base, wmap, arraydims[i + 1], idx[i], 1);

        if (!wmap) {
            return 1;
        }
    }

    wmap = map_split_add_array(base, wmap, arraydims[i + 1], idx[i], 0);
    i++;
    lnxproc_array_set_last(wmap, idx[i], val);

    return LNXPROC_OK;
}

int
lnxproc_map_split(LNXPROC_BASE_T *base,
                  int *arraydims, LNXPROC_MAP_LIMITS_T limit[], size_t limitlen)
{

    int n = lnxproc_base_nbytes(base);

    if (n > 0) {
        int idx[limitlen];

        memset(idx, 0, limitlen * sizeof(int));

        char *c = lnxproc_base_lines(base);
        char *d = c + n;

        char *saveptr = c;

        while (c < d) {
            if (lnxproc_map_chr(limit + 0, *c)) {
                *c = '\0';

                map_split_func(base, arraydims, idx, limitlen, saveptr);

                while ((++c < d) && lnxproc_map_chr(limit + 0, *c));

                saveptr = c;
                idx[0]++;
                idx[1] = 0;

            }

            else if (lnxproc_map_chr(limit + 1, *c)) {
                *c = '\0';

                map_split_func(base, arraydims, idx, limitlen, saveptr);

                while ((++c < d) && lnxproc_map_chr(limit + 1, *c));

                saveptr = c;
                idx[1]++;

            }

            else {
                c++;
            }
        }
    }

    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
