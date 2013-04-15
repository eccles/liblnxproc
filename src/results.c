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
#include <tdb.h>

#include "error.h"
#include "results.h"

char *
lnxproc_results_timeval_str(LNXPROC_RESULTS_T * results, char *buf,
                            size_t buflen)
{
    if (results && buf) {
        snprintf(buf, buflen, "%d.%06d", (int) results->tv.tv_sec,
                 (int) results->tv.tv_usec);
    }
    return buf;
}

struct timeval *
lnxproc_results_timeval(LNXPROC_RESULTS_T * results)
{
    LNXPROC_DEBUG("Timestamp %p\n", results);
    struct timeval *tv = NULL;

    if (results) {
        gettimeofday(&results->tv, NULL);
        tv = &results->tv;
#ifdef DEBUG
        char buf[64];

        lnxproc_results_timeval_str(results, buf, sizeof buf);
        LNXPROC_DEBUG("results %s\n", buf);
#endif
    }

    LNXPROC_DEBUG("val %p\n", tv);
    return tv;
}

struct timeval *
lnxproc_results_tv(LNXPROC_RESULTS_T * results)
{
    LNXPROC_DEBUG("Timestamp %p\n", results);
    struct timeval *tv = NULL;

    if (results) {
        tv = &results->tv;
#ifdef DEBUG
        char buf[64];

        lnxproc_results_timeval_str(results, buf, sizeof buf);
        LNXPROC_DEBUG("results %s\n", buf);
#endif
    }

    LNXPROC_DEBUG("val %p\n", tv);
    return tv;
}

int
lnxproc_results_print(LNXPROC_RESULTS_T * results)
{
    LNXPROC_DEBUG("Timestamp %p\n", results);

    if (results) {
        char buf[64];

        lnxproc_results_timeval_str(results, buf, sizeof buf);
        printf("Timestamp %s\n", buf);
        return LNXPROC_OK;
    }

    LNXPROC_DEBUG("WARNING: Timestamp is null\n");
    return LNXPROC_ERROR_RESULTS_NULL;
}

LNXPROC_RESULTS_T *
lnxproc_results_new(void)
{
    LNXPROC_RESULTS_T *results = calloc(1, sizeof(LNXPROC_RESULTS_T));

    if (!results) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return NULL;
    }

    gettimeofday(&results->tv, NULL);

    results->db = DB_OPEN();
    if (!results->db) {
//        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_RESULTS_DB_OPEN);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_OPEN, "Open Db\n");
        results = lnxproc_results_free(results);
        return results;
    }

    LNXPROC_DEBUG("Successful\n");
    return results;
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
//              LNXPROC_SET_ERROR(results->callback, LNXPROC_ERROR_RESULTS_DB_CLOSE);
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

LNXPROC_RESULTS_DATA_T
lnxproc_results_fetch(LNXPROC_RESULTS_T * results, char *key, size_t keylen)
{
    LNXPROC_DEBUG("Db %p\n", results);
#ifdef LNXPROC_TDB
    LNXPROC_RESULTS_DATA_T dbkey = {
        .dsize = 0,
        .dptr = NULL,
    };
#endif
    if (results) {
        if (!results->db) {
//          LNXPROC_SET_ERROR(results->callback, LNXPROC_ERROR_RESULTS_DB_NOT_OPEN);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
                                "Fetch results\n");
            return dbkey;
        }
#ifdef LNXPROC_TDB
        dbkey.dsize = keylen;
        dbkey.dptr = (unsigned char *) key;
        LNXPROC_RESULTS_DATA_T ret = tdb_fetch(results->db, dbkey);

        if (!ret.dptr) {
//          LNXPROC_SET_ERROR(results->callback, LNXPROC_ERROR_RESULTS_DB_FETCH);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_FETCH,
                                "Fetch results %s\n",
                                tdb_errorstr(results->db));
        }
        return ret;
#endif
    }

    LNXPROC_DEBUG("WARNING: Db is null\n");
    return dbkey;
}

int
lnxproc_results_store(LNXPROC_RESULTS_T * results, LNXPROC_RESULTS_DATA_T key,
                      LNXPROC_RESULTS_DATA_T data)
{
    LNXPROC_DEBUG("Db %p\n", results);

    if (results) {
        if (!results->db) {
//          LNXPROC_SET_ERROR(results->callback, LNXPROC_ERROR_RESULTS_DB_NOT_OPEN);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_NOT_OPEN,
                                "Store results\n");
            return LNXPROC_ERROR_RESULTS_DB_NOT_OPEN;
        }
#ifdef LNXPROC_TDB
        int flag = TDB_REPLACE;
        int ret = tdb_store(results->db, key, data, flag);

        if (ret < 0) {
//          LNXPROC_SET_ERROR(results->callback, LNXPROC_ERROR_RESULTS_DB_STORE);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_DB_STORE,
                                "Store results %s\n",
                                tdb_errorstr(results->db));
            return LNXPROC_ERROR_RESULTS_DB_STORE;
        }
        return LNXPROC_OK;
#endif
    }

    LNXPROC_DEBUG("WARNING: Db is null\n");
    return LNXPROC_ERROR_RESULTS_NULL;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
