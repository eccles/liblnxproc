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
#include "timestamp.h"

#include <sys/time.h>

/*
       int gettimeofday(struct timeval *tv, struct timezone *tz);
*/
struct lnxproc_timestamp_t {
    struct timeval val;
    const char *filename;
    LNXPROC_ERROR_CALLBACK callback;
};

static char *
timestamp_timeval_print(struct timeval *val, char *buf, size_t buflen)
{
    if (buf) {
        snprintf(buf, buflen, "%d.%06d", (int) val->tv_sec, (int) val->tv_usec);
    }
    return buf;
}

struct timeval *
lnxproc_timestamp_timeval(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);
    struct timeval *val = NULL;

    if (timestamp) {
        val = &timestamp->val;
#ifdef DEBUG
        char buf[64];

        timestamp_timeval_print(val, buf, sizeof buf);
        LNXPROC_DEBUG("timestamp %s\n", buf);
#endif
    }

    LNXPROC_DEBUG("val %p\n", val);
    return val;
}

int
lnxproc_timestamp_print(LNXPROC_TIMESTAMP_T * timestamp)
{
    LNXPROC_DEBUG("Timestamp %p\n", timestamp);

    if (timestamp) {
        char buf[64];

        timestamp_timeval_print(&timestamp->val, buf, sizeof buf);
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
