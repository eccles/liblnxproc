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

   Reads the file /sys/block/<disk>/queue/hw_sector_size' 

*/

#include <stdlib.h>
#include <string.h>

#include "error_private.h"
#include "array_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "base_private.h"
#include "sys_disksectors.h"

struct env_t {
    LNXPROC_RESULTS_T *results;
    char *key;
};

static LNXPROC_ERROR_T
iter_func(char *val, void *data, size_t idx[], size_t dim)
{
    LNXPROC_DEBUG("Val %s, Data %p, idx[%d]=%d idx[%d]=%d\n", val, data, 0,
                  idx[0], 1, idx[1]);
    struct env_t *env = data;

    if (idx[1] == 0) {
        env->key = val;
//        lnxproc_results_store(env->results, val, "/key%02d", idx[0]);
    }
    else {
        lnxproc_results_store(env->results, val, "/%s", env->key);
    }
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
sys_disksectors_normalize(LNXPROC_BASE_T *base)
{

    LNXPROC_RESULTS_T *results = base->results;
    _LNXPROC_ARRAY_T *array = base->array;

    struct env_t env = {
        .results = results,
        .key = NULL,
    };

    _lnxproc_array_iterate(array, &env, iter_func);

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_sys_disksectors_new(LNXPROC_BASE_T **base)
{

    char *fileprefix = "/sys/block";
    char *filesuffix = "queue/hw_sector_size";

    _LNXPROC_LIMITS_T limits[] = {
        {.expected = 9,.chars = "\n",.len = 1}, /* row delimiters */
        {.expected = 2,.chars = "\t",.len = 1}  /* column delimiters */
    };

    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return _lnxproc_base_new(base,
                             NULL, 0, fileprefix, filesuffix,
                             NULL, sys_disksectors_normalize, NULL,
                             256, limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
