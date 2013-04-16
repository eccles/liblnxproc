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
    printf("Results %s\n", buf);
    return LNXPROC_OK;

}

LNXPROC_ERROR_T
lnxproc_results_new(LNXPROC_RESULTS_T ** results)
{
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
lnxproc_results_store(LNXPROC_RESULTS_T * results, LNXPROC_RESULTS_DATA_T key,
                      LNXPROC_RESULTS_DATA_T data)
{
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
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
