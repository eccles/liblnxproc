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

struct env_t {
    _LNXPROC_RESULTS_T *results;
    char *key;
};

static int
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

static int
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

int
_lnxproc_sys_disksectors_new(_LNXPROC_BASE_T ** base, void *optional)
{

    _LNXPROC_LIMITS_T *limits = NULL;
    int ret = _lnxproc_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }
    ret = _lnxproc_limits_set(limits, 1, 1, "\t", 1);   /* column delimiters */
    if (ret) {
        _LNXPROC_LIMITS_FREE(limits);
        return ret;
    }

    char *fileprefix = "/sys/block";
    char *fileglob;

    if (optional) {
        fileglob = optional;
    }
    else {
        fileglob = "*";
    }
    char *filesuffix = "queue/hw_sector_size";

    ret = _lnxproc_base_new(base, "sys_disksectors",
                            _LNXPROC_BASE_TYPE_MEMOIZE, NULL,
                            sys_disksectors_normalize, NULL, 256, limits);
    if (!ret) {
        _lnxproc_base_set_fileprefix(*base, fileprefix);
        _lnxproc_base_set_fileglob(*base, fileglob);
        _lnxproc_base_set_filesuffix(*base, filesuffix);
    }
    _LNXPROC_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
