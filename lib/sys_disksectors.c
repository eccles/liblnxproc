/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *

   Reads the file /sys/block/<disk>/queue/hw_sector_size' 

*/

#include "config.h"

#include "topiary_stdlib.h"
#include <string.h>

#include "strlcpy.h"
#include "error_private.h"
#include "array_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "opt_private.h"
#include "base_private.h"
#include "modules.h"

struct env_t {
    _TOPIARY_RESULTS_T *results;
    char key[32];
};

static int
iter_func(char *val, void *data, size_t idx[], size_t dim)
{
    _TOPIARY_DEBUG("Val %s, Data %p, idx[%d]=%zd idx[%d]=%zd\n", val, data, 0,
                   idx[0], 1, idx[1]);
    struct env_t *env = data;

    if (idx[1] == 0) {
        int n = 0;

        STRLCAT(env->key, "/", n, sizeof env->key);
        STRLCAT(env->key, val, n, sizeof env->key);
    }
    else {
        _topiary_results_add_int(env->results, env->key, atoi(val));
    }
    return TOPIARY_OK;
}

static int
sys_disksectors_normalize(_TOPIARY_BASE_T *base)
{

    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;

    struct env_t env = {
        .results = results,
        .key = "",
    };

    _topiary_results_init(results, 2);
    _topiary_array_iterate(array, &env, 0, iter_func);

    return TOPIARY_OK;
}

int
_topiary_sys_disksectors_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 1, "\t", 1);   /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *fileprefix = "/sys/block";
    char *fileglob;

    if (optional && optional->fileglob) {
        fileglob = optional->fileglob;
    }
    else {
        fileglob = "*";
    }
    char *filesuffix = "queue/hw_sector_size";

    ret = _topiary_base_new(base, "sys_disksectors",
                            _TOPIARY_BASE_TYPE_MEMOIZE, NULL,
                            sys_disksectors_normalize, NULL, 256, limits);
    if (!ret) {
        _topiary_base_set_fileprefix(*base, fileprefix);
        _topiary_base_set_fileglob(*base, fileglob);
        _topiary_base_set_filesuffix(*base, filesuffix);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
