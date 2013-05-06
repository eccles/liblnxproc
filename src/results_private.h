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

#ifndef LIBLNXPROC_RESULTS_PRIVATE_H
#define LIBLNXPROC_RESULTS_PRIVATE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include "error_private.h"
#include "util_private.h"
#include "db_private.h"

    struct _lnxproc_results_t {
        struct timeval tv;
        LNXPROC_RESULTS_FILE_T *db;
    };
    typedef struct _lnxproc_results_t _LNXPROC_RESULTS_T;

    LNXPROC_ERROR_T _lnxproc_results_new(_LNXPROC_RESULTS_T ** newresults);

    _LNXPROC_RESULTS_T *_lnxproc_results_free(_LNXPROC_RESULTS_T *
                                              results) WARN_UNUSED;
#define _LNXPROC_RESULTS_FREE(r) {\
    r =  _lnxproc_results_free(r);\
}

    LNXPROC_ERROR_T _lnxproc_results_timeval(_LNXPROC_RESULTS_T * results,
                                             struct timeval **tv);
    LNXPROC_ERROR_T _lnxproc_results_tv(_LNXPROC_RESULTS_T * results,
                                        struct timeval **tv);
    LNXPROC_ERROR_T _lnxproc_results_timeval_str(_LNXPROC_RESULTS_T * results,
                                                 char *buf, size_t buflen);
    LNXPROC_ERROR_T _lnxproc_results_print(_LNXPROC_RESULTS_T * results);

    LNXPROC_ERROR_T _lnxproc_results_fetch(_LNXPROC_RESULTS_T * results,
                                           char *key, size_t keylen,
                                           LNXPROC_RESULTS_DATA_T * val);

    LNXPROC_ERROR_T _lnxproc_results_store(_LNXPROC_RESULTS_T * results,
                                           char *value, char *fmt, ...);

    typedef int (*_LNXPROC_RESULTS_ITERATE_FUNC) (char *key, char *value,
                                                  void *data);
    LNXPROC_ERROR_T _lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                                             _LNXPROC_RESULTS_ITERATE_FUNC func,
                                             void *data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_RESULTS_PRIVATE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
