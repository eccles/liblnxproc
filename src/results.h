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

#ifndef LIBLNXPROC_RESULTS_H
#define LIBLNXPROC_RESULTS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include "db.h"
#include "error.h"

    struct lnxproc_results_t {
        struct timeval tv;
//        const char *filename;
        LNXPROC_RESULTS_FILE_T *db;
    };
    typedef struct lnxproc_results_t LNXPROC_RESULTS_T;

    LNXPROC_ERROR_T lnxproc_results_new(LNXPROC_RESULTS_T ** newresults);

#ifndef WARN_UNUSED
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

    LNXPROC_RESULTS_T *lnxproc_results_free(LNXPROC_RESULTS_T *
                                            results) WARN_UNUSED;
#define LNXPROC_RESULTS_FREE(r) {\
    r =  lnxproc_results_free(r);\
}

//    LNXPROC_ERROR_T lnxproc_results_filename(LNXPROC_RESULTS_T * results, char **filename);
    LNXPROC_ERROR_T lnxproc_results_timeval(LNXPROC_RESULTS_T * results,
                                            struct timeval **tv);
    LNXPROC_ERROR_T lnxproc_results_tv(LNXPROC_RESULTS_T * results,
                                       struct timeval **tv);
    LNXPROC_ERROR_T lnxproc_results_timeval_str(LNXPROC_RESULTS_T * results,
                                                char *buf, size_t buflen);
    LNXPROC_ERROR_T lnxproc_results_print(LNXPROC_RESULTS_T * results);

    LNXPROC_ERROR_T lnxproc_results_fetch(LNXPROC_RESULTS_T * results,
                                          char *key, size_t keylen,
                                          LNXPROC_RESULTS_DATA_T * val);

    LNXPROC_ERROR_T lnxproc_results_store(LNXPROC_RESULTS_T * results,
                                          LNXPROC_RESULTS_DATA_T key,
                                          LNXPROC_RESULTS_DATA_T data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_RESULTS_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
