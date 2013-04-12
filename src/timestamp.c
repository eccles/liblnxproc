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

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "timestamp_private.h"

/*
    struct lnxproc_timestamp_t;
    typedef struct lnxproc_timestamp_t LNXPROC_TIMESTAMP_T;

    struct timeval *lnxproc_timestamp_timeval(LNXPROC_TIMESTAMP_T * timestamp);
    int lnxproc_timestamp_print(LNXPROC_TIMESTAMP_T * timestamp);
    LNXPROC_TIMESTAMP_T *lnxproc_timestamp_init(LNXPROC_ERROR_CALLBACK
                                                callback);
    LNXPROC_TIMESTAMP_T *lnxproc_timestamp_free(LNXPROC_TIMESTAMP_T *
                                                timestamp);

       int gettimeofday(struct timeval *tv, struct timezone *tz);
*/

char *
lnxproc_timestamp_str(LNXPROC_TIMESTAMP_T * timestamp, char *buf, size_t buflen)
{
    if (timestamp && buf) {
        snprintf(buf, buflen, "%d.%06d", (int) timestamp->tv.tv_sec,
                 (int) timestamp->tv.tv_usec);
    }
    return buf;
}

struct timeval *
lnxproc_timestamp_timeval(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);
    struct timeval *tv = NULL;

    if (timestamp) {
        gettimeofday(&timestamp->tv, NULL);
        tv = &timestamp->tv;
#ifdef DEBUG
        char buf[64];

        lnxproc_timestamp_str(timestamp, buf, sizeof buf);
        LNXPROC_DEBUG("timestamp %s\n", buf);
#endif
    }

    LNXPROC_DEBUG("val %p\n", tv);
    return tv;
}

struct timeval *
lnxproc_timestamp_tv(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);
    struct timeval *tv = NULL;

    if (timestamp) {
        tv = &timestamp->tv;
#ifdef DEBUG
        char buf[64];

        lnxproc_timestamp_str(timestamp, buf, sizeof buf);
        LNXPROC_DEBUG("timestamp %s\n", buf);
#endif
    }

    LNXPROC_DEBUG("val %p\n", tv);
    return tv;
}

int
lnxproc_timestamp_print(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);

    if (timestamp) {
        char buf[64];

        lnxproc_timestamp_str(timestamp, buf, sizeof buf);
        printf("Timestamp %s\n", buf);
        return LNXPROC_OK;
    }

    LNXPROC_DEBUG("WARNING: Timestamp is null\n");
    return LNXPROC_ERROR_TIMESTAMP_NULL;
}

LNXPROC_TIMESTAMP_T *
lnxproc_timestamp_init(LNXPROC_ERROR_CALLBACK callback)
{
    LNXPROC_DEBUG("callback %p\n", callback);

    LNXPROC_TIMESTAMP_T *timestamp = malloc(sizeof(LNXPROC_TIMESTAMP_T));

    if (!timestamp) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_TIMESTAMP_MALLOC);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_TIMESTAMP_MALLOC,
                            "Malloc timestamp\n");
        return timestamp;
    }

    timestamp->callback = callback;
    lnxproc_timestamp_timeval(timestamp);
    LNXPROC_DEBUG("Successful\n");
    return timestamp;
}

LNXPROC_TIMESTAMP_T *
lnxproc_timestamp_free(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);

    if (timestamp) {
        LNXPROC_DEBUG("Free Timestamp\n");
        free(timestamp);
        timestamp = NULL;
    }

    return timestamp;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
