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

#include <stdlib.h>
#include <string.h>

#include "base_private.h"
#include <lnxproc/proc_cgroups.h>

struct proc_groups_env_t {
    LNXPROC_RESULTS_T *results;
    int colslen;
    char **cols;
    char *key;
};

static LNXPROC_ERROR_T
proc_groups_func(char *val, void *data, size_t idx[], size_t dim)
{
    LNXPROC_DEBUG("Val %s, Data %p, idx[%d]=%d idx[%d]=%d\n", val, data, 0,
                  idx[0], 1, idx[1]);
    struct proc_groups_env_t *env = data;

    if (idx[0] == 0) {
        lnxproc_results_store(env->results, val, "/col%02d", idx[1]);
        void *p = realloc(env->cols, (env->colslen + 1) * sizeof(char *));

        if (!p) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_SYSTEM, "\n");
            return LNXPROC_ERROR_SYSTEM;
        }
        env->cols = p;
        env->colslen += 1;
        env->cols[idx[1]] = strdup(val);
    }
    else {
        if (idx[1] == 0) {
            env->key = val;
            lnxproc_results_store(env->results, val, "/key%02d", idx[0]);
        }
        else if (idx[1] < env->colslen) {
            lnxproc_results_store(env->results, val, "/%s/%s", env->key,
                                  env->cols[idx[1]]);
        }
    }
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
proc_cgroups_normalize(LNXPROC_BASE_T *base)
{
    LNXPROC_RESULTS_T *results = base->results;
    LNXPROC_ARRAY_T *array = base->array;

    struct proc_groups_env_t env = {
        .results = results,
        .colslen = 0,
        .cols = NULL,
        .key = NULL,
    };

    lnxproc_array_iterate(array, &env, proc_groups_func);
    if (env.cols) {
        int i;

        for (i = 0; i < env.colslen; i++) {
            free(env.cols[i]);
        }
        free(env.cols);
    }

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_proc_cgroups_new(LNXPROC_BASE_T **base)
{

    LNXPROC_LIMITS_T limits[] = {
        {9, "\n", 1},           /* row delimiters */
        {4, "\t", 1}            /* column delimiters */
    };

    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return lnxproc_base_new(base,
                            "/proc/cgroups",
                            NULL, proc_cgroups_normalize, NULL, 256, limits,
                            dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
