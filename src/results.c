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
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <tdb.h>

#include "error.h"
#include "results.h"

LNXPROC_ERROR_T
lnxproc_results_timeval_str(LNXPROC_RESULTS_T * results, char *buf,
                            size_t buflen)
{
    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }
    if (!buf) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_BUF_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_BUF_NULL;
    }
    if (buflen < 1) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_BUFLEN_ZERO, "\n");
        return LNXPROC_ERROR_RESULTS_BUFLEN_ZERO;
    }

    if (results && buf) {
        snprintf(buf, buflen, "%d.%06d", (int) results->tv.tv_sec,
                 (int) results->tv.tv_usec);
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_results_timeval(LNXPROC_RESULTS_T * results, struct timeval **tv)
{
    LNXPROC_DEBUG("Results %p\n", results);
    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    gettimeofday(&results->tv, NULL);
    if (tv) {
        *tv = &results->tv;
    }
#ifdef DEBUG
    char buf[64];

    lnxproc_results_timeval_str(results, buf, sizeof buf);
    LNXPROC_DEBUG("Timestamp %s\n", buf);
#endif

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_results_tv(LNXPROC_RESULTS_T * results, struct timeval **tv)
{
    LNXPROC_DEBUG("Results %p\n", results);
    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    if (!tv) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_TV_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_TV_NULL;
    }

    *tv = &results->tv;
#ifdef DEBUG
    char buf[64];

    lnxproc_results_timeval_str(results, buf, sizeof buf);
    LNXPROC_DEBUG("Timestamp %s\n", buf);
#endif

    return LNXPROC_OK;
}

static int
internal_print_func(char *key, char *value, void *data)
{
    printf("Key %s = %s\n", key, value);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_results_print(LNXPROC_RESULTS_T * results)
{
    LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    char buf[64];

    lnxproc_results_timeval_str(results, buf, sizeof buf);
    printf("Timestamp %s\n", buf);

    return lnxproc_results_iterate(results, internal_print_func, NULL);

}

LNXPROC_ERROR_T
lnxproc_results_new(LNXPROC_RESULTS_T ** results)
{
    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof LNXPROC_RESULTS_T %d\n", sizeof(LNXPROC_RESULTS_T));
    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_NULL;
    }

    if (*results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL,
                            "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL;
    }

    LNXPROC_RESULTS_T *newresults = calloc(1, sizeof(LNXPROC_RESULTS_T));

    if (!newresults) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    gettimeofday(&newresults->tv, NULL);

    newresults->db = DB_OPEN();
    if (!newresults->db) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_OPEN, "Open Db\n");
        LNXPROC_RESULTS_FREE(newresults);
        return LNXPROC_ERROR_RESULTS_DB_OPEN;
    }

    *results = newresults;
    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_RESULTS_T *
lnxproc_results_free(LNXPROC_RESULTS_T * results)
{
    LNXPROC_DEBUG("Results %p\n", results);

    if (results) {
        LNXPROC_DEBUG("Free Results\n");
        if (results->db) {
#ifdef LNXPROC_TDB
            LNXPROC_DEBUG("Free Db\n");
            int ret = tdb_close(results->db);

            if (ret < 0) {
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_CLOSE,
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
lnxproc_results_fetch(LNXPROC_RESULTS_T * results, char *key, size_t keylen,
                      LNXPROC_RESULTS_DATA_T * val)
{
    LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    if (!key) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_KEY_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_KEY_NULL;
    }

    if (keylen < 1) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_KEYLEN_ZERO, "\n");
        return LNXPROC_ERROR_RESULTS_KEYLEN_ZERO;
    }

    if (!val) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL;
    }

    if (!results->db) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
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
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_FETCH,
                            "Fetch results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_FETCH;
    }
#endif

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_results_store(LNXPROC_RESULTS_T * results, char *value, char *fmt, ...)
{
    va_list ap;

    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    LNXPROC_DEBUG("Results %p\n", results);

    if (!results->db) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
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

    TDB_DATA data;

    data.dsize = 1 + strlen(value);
    data.dptr = (unsigned char *) value;
    int flag = TDB_REPLACE;
    int ret = tdb_store(results->db, key, data, flag);

    if (ret < 0) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_STORE,
                            "Store results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_STORE;
    }
#endif
    return LNXPROC_OK;
}

/*
TDB_TRAVERSE(3)            Linux Programmer's Manual           TDB_TRAVERSE(3)

NAME
       tdb_traverse - visit every element in a tdb database

SYNOPSIS
       #include <tdb.h>

       int tdb_traverse(TDB_CONTEXT *tdb,
               int (*fn)(TDB_CONTEXT *,TDB_DATA,TDB_DATA,void *),
               void *state);

DESCRIPTION
       tdb_traverse is the only sure way to visit all the items within a given
       database, tdb Because this  function  has  intimate  knowledge  of  the
       internals of the database, it is able to cope with writers touching the
       database at the same time it is trying to traverse it.

       If fn is supplied it will be called with the state parameter  for  each
       element  in  the database, as the forth argument. The First argument is
       the database tdb the second is the key and the third is  the  data.  If
       this  function  call  returns  anything but 0, the traversal will stop.
       Unlike in tdb_fetch() the programmer is not required to free either the
       pointer from either the key or data parameters that are passed into the
       function. The fn function should have the prototype:
          int (*tdb_traverse_func)(TDB_CONTEXT *, TDB_DATA, TDB_DATA, void *);

       Calling tdb_traverse with a NULL fn parameter is the appropriate way to
       count the number of elements in the database.

RETURN VALUE
       The return value is the number of elements traversed or -1 if there was
       an error.

AUTHORS
       Software: Andrew Tridgell <tridge@linuxcare.com> and Luke Kenneth  Cas‐
       son Leighton Man page: Ben Woodard <ben@valinux.com>

SEE ALSO
       gdbm(3), tdb(3)

Samba                            Aug 16, 2000                  TDB_TRAVERSE(3)
~
*/
//typedef int (*RESULTS_ITERATE_FUNC)(char *key,char *value,void *env);
//static int db_traverse_func(char *key,char *value,void *env) {
//    return LNXPROC_OK;
//}
#ifdef LNXPROC_TDB
struct db_traverse_env_t {
    LNXPROC_RESULTS_ITERATE_FUNC func;
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
lnxproc_results_iterate(LNXPROC_RESULTS_T * results,
                        LNXPROC_RESULTS_ITERATE_FUNC func, void *data)
{
    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    LNXPROC_DEBUG("Results %p\n", results);

    if (!results->db) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
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
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_ITERATE,
                            "Iterate results %s\n", tdb_errorstr(results->db));
        return LNXPROC_ERROR_RESULTS_DB_ITERATE;
    }
#endif
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
