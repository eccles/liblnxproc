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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "error_private.h"
#include "db_private.h"
#include "results_private.h"

static int
internal_print_func(char *key, char *value, void *data)
{
    printf("Key %s = %s\n", key, value);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_print(_LNXPROC_RESULTS_T * results)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    return _lnxproc_results_iterate(results, internal_print_func, NULL);

}

LNXPROC_ERROR_T
_lnxproc_results_new(_LNXPROC_RESULTS_T ** results)
{
    _LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_RESULTS_T %d\n",
                   sizeof(_LNXPROC_RESULTS_T));
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_NULL;
    }

    if (*results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL;
    }

    _LNXPROC_RESULTS_T *newresults = calloc(1, sizeof(_LNXPROC_RESULTS_T));

    if (!newresults) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    newresults->db = DB_OPEN();
    if (!newresults->db) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_OPEN, "Open Db\n");
        _LNXPROC_RESULTS_FREE(newresults);
        return LNXPROC_ERROR_RESULTS_DB_OPEN;
    }

    *results = newresults;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

_LNXPROC_RESULTS_T *
_lnxproc_results_free(_LNXPROC_RESULTS_T * results)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (results) {
        _LNXPROC_DEBUG("Free Results\n");
        if (results->db) {
#ifdef LNXPROC_TDB
            _LNXPROC_DEBUG("Free Db\n");
            int ret = tdb_close(results->db);

            if (ret < 0) {
                _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_CLOSE,
                                     "Close Db\n");
            }
            results->db = NULL;
#endif
        }
        free(results);
        results = NULL;
    }

    return results;
}

LNXPROC_ERROR_T
_lnxproc_results_fetch(_LNXPROC_RESULTS_T * results, char *key, size_t keylen,
                       LNXPROC_RESULTS_DATA_T * val)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    if (!key) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_KEY_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_KEY_NULL;
    }

    if (keylen < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_KEYLEN_ZERO, "\n");
        return LNXPROC_ERROR_RESULTS_KEYLEN_ZERO;
    }

    if (!val) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL;
    }

    if (!results->db) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
                             "Fetch results\n");
        return LNXPROC_ERROR_RESULTS_DB_NOT_OPEN;
    }
#ifdef LNXPROC_TDB
    LNXPROC_RESULTS_DATA_T dbkey = {
        .dsize = keylen,
        .dptr = (unsigned char *) key,
    };

    *val = tdb_fetch(results->db, dbkey);
    if (!val->dptr) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_FETCH,
                             "Fetch results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_FETCH;
    }
#endif

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_store(_LNXPROC_RESULTS_T * results, const char *value,
                       char *fmt, ...)
{
    va_list ap;

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    _LNXPROC_DEBUG("Results %p Value %p '%s'\n", results, value, value);

    if (!results->db) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
                             "Store results\n");
        return LNXPROC_ERROR_RESULTS_DB_NOT_OPEN;
    }
#ifdef LNXPROC_TDB

    char buf[128];

    TDB_DATA key;

    va_start(ap, fmt);
    key.dsize = 1 + vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    key.dptr = (unsigned char *) buf;

    _LNXPROC_DEBUG("Key %d : '%s'\n", key.dsize, key.dptr);

    TDB_DATA data;

    data.dsize = 1 + strlen(value);
    data.dptr = (unsigned char *) value;
    _LNXPROC_DEBUG("Value %d : '%s'\n", data.dsize, data.dptr);

    int flag = TDB_REPLACE;
    int ret = tdb_store(results->db, key, data, flag);

    if (ret < 0) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_STORE,
                             "Store results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_STORE;
    }
#endif
    return LNXPROC_OK;
}

//typedef int (*RESULTS_ITERATE_FUNC)(char *key,char *value,void *env);
//static int db_traverse_func(char *key,char *value,void *env) {
//    return LNXPROC_OK;
//}
#ifdef LNXPROC_TDB
struct db_traverse_env_t {
    _LNXPROC_RESULTS_ITERATE_FUNC func;
    void *data;
};
static int
db_traverse_func(TDB_CONTEXT * tdb, TDB_DATA key, TDB_DATA value, void *state)
{
    struct db_traverse_env_t *env = state;

    env->func((char *) key.dptr, (char *) value.dptr, env->data);
    return 0;
}
#endif

LNXPROC_ERROR_T
_lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                         _LNXPROC_RESULTS_ITERATE_FUNC func, void *data)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results->db) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
                             "Iterate results\n");
        return LNXPROC_ERROR_RESULTS_DB_NOT_OPEN;
    }
#ifdef LNXPROC_TDB
    struct db_traverse_env_t env = {
        .func = func,
        .data = data,
    };
    int ret = tdb_traverse(results->db, db_traverse_func, &env);

    if (ret < 0) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_ITERATE,
                             "Iterate results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_ITERATE;
    }
#endif
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
