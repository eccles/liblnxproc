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

#ifndef LIBLNXPROC_DB_H
#define LIBLNXPROC_DB_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <tdb.h>
#include "error.h"

    struct lnxproc_db_t;
    typedef struct lnxproc_db_t LNXPROC_DB_T;

    typedef TDB_DATA LNXPROC_DB_DATA_T;

    const char *lnxproc_db_filename(LNXPROC_DB_T * db);
    int lnxproc_db_print(LNXPROC_DB_T * db);
    LNXPROC_DB_T *lnxproc_db_init(const char *filename,
                                  LNXPROC_ERROR_CALLBACK callback);
    LNXPROC_DB_T *lnxproc_db_free(LNXPROC_DB_T * db);

    LNXPROC_DB_DATA_T lnxproc_db_fetch(LNXPROC_DB_T * db, char *key,
                                       size_t keylen);
    int lnxproc_db_store(LNXPROC_DB_T * db, LNXPROC_DB_DATA_T key,
                         LNXPROC_DB_DATA_T data);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_DB_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
