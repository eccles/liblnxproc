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
    _LNXPROC_RESULTS_T *results;
    char *key;
};

static LNXPROC_ERROR_T
iter_func(char *val, void *data, size_t idx[], size_t dim)
{
    _LNXPROC_DEBUG("Val %s, Data %p, idx[%d]=%zd idx[%d]=%zd\n", val, data, 0,
                   idx[0], 1, idx[1]);
    struct env_t *env = data;

    if (idx[1] == 0) {
        env->key = val;
    }
    else {
        _lnxproc_results_add_int(env->results, env->key, atoi(val));
    }
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
sys_disksectors_normalize(_LNXPROC_BASE_T * base)
{

    _LNXPROC_RESULTS_T *results = base->current->results;
    _LNXPROC_ARRAY_T *array = base->current->array;

    struct env_t env = {
        .results = results,
        .key = NULL,
    };

    _lnxproc_results_init(results, 2);
    _lnxproc_array_iterate(array, &env, 0, iter_func);

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_sys_disksectors_new(_LNXPROC_BASE_T ** base)
{

    char *fileprefix = "/sys/block";
    char *fileglob = "*";
    char *filesuffix = "queue/hw_sector_size";

    _LNXPROC_LIMITS_T limits[] = {
        {.expected = 9,.chars = "\n",.len = 1}, /* row delimiters */
        {.expected = 1,.chars = "\t",.len = 1}  /* column delimiters */
    };

    size_t dim = sizeof(limits) / sizeof(limits[0]);

    return _lnxproc_base_new(base, "sys_disksectors",
                             _LNXPROC_BASE_TYPE_MEMOIZE, NULL, 0, fileprefix,
                             fileglob, filesuffix, NULL,
                             sys_disksectors_normalize, NULL, 256, limits, dim);
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
