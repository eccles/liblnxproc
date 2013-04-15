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

#include "error.h"

#define LNXPROC_TDB 1

#ifdef LNXPROC_TDB

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tdb.h>

    typedef TDB_CONTEXT LNXPROC_RESULTS_FILE_T;
    typedef TDB_DATA LNXPROC_RESULTS_DATA_T;

#define DB_OPEN()  ({\
    int hash_size = 0;\
    int tdb_flags = TDB_INTERNAL;\
    int open_flags = O_RDWR;\
    mode_t mode = 0;\
\
    LNXPROC_RESULTS_FILE_T *db = tdb_open(NULL, hash_size, tdb_flags, open_flags, mode);\
\
    db;\
})\

#endif                          // LNXPROC_TDB

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_DB_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
