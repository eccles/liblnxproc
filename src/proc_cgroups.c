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

typical contents of /proc/cgroups file::

   subsys_name hierarchy num_cgroups enabled
   cpuset     1     4 1
   cpu        2     4 1
   cpuacct    3     4 1
   memory     4     4 1
   devices    5     4 1
   freezer    6     4 1
   blkio      7     4 1
   perf_event 8     1 1

*/

#include "array.h"
#include "proc_cgroups.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int proccgroups_func(struct lnxproc_base_t *base, int i, int j,
                            char *val)
{

    printf(" proccgroups_func:%d,%d: %p '%s'\n", i, j, val, val);

    void *map = lnxproc_base_map(base);
    /* map should only be null when both i && j are 0 */
    if (!map) {
        map = lnxproc_array_new(2, 1);
        if (!map) {
            printf("Map allocation failure\n");
            return 1;
        }
        lnxproc_base_map_set(base, map);
    }

    void *f = lnxproc_array_get(map, i);
    /* f should only be null when j is 0 */
    if (!f) {
        f = lnxproc_array_new(2, 0);
        if (!f) {
            printf("Field allocation failure\n");
            return 1;
        }
        lnxproc_array_set(map, i, f);
    }
    else {
        lnxproc_array_set_length(map, i);
    }
    lnxproc_array_set_last(f, j, val);
    lnxproc_array_print(map, NULL);

    return 0;
}

int lines_split(struct lnxproc_base_t *base,
                char *line_limit,
                char *field_limit,
                int (*func) (struct lnxproc_base_t *, int, int, char *))
{

    int n = lnxproc_base_nbytes(base);
    if (n > 0) {
        int i = 0;
        int j = 0;

        char *c = lnxproc_base_lines(base);
        char *d = c + n;

        char *saveptr = c;
        while (c < d) {
            if (strchr(line_limit, *c)) {
                *c = '\0';
                printf
                    ("lines_split:1:Line %d Field %d String %s\n",
                     i, j, saveptr);

                func(base, i, j, saveptr);
                while ((++c < d) && strchr(line_limit, *c));
                saveptr = c;
                i++;
                j = 0;
            }
            else if (strchr(field_limit, *c)) {
                *c = '\0';
                printf
                    ("lines_split:2:Line %d Field %d String %s\n",
                     i, j, saveptr);
                func(base, i, j, saveptr);
                while ((++c < d) && strchr(field_limit, *c));
                saveptr = c;
                j++;
            }
            else {
                c++;
            }
        }
    }

    return 0;
}

static int proccgroups_normalize(struct lnxproc_base_t *base)
{
    lines_split(base, "\n", "\t", proccgroups_func);
    return 0;
}

struct lnxproc_base_t *proccgroups_init(void)
{
    return lnxproc_base_init("/proc/cgroups",
                             NULL,
                             proccgroups_normalize,
                             NULL, 256, &proccgroups_data);
}

/* 
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab 
 */
