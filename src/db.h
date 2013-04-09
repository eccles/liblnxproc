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

#ifndef LIBLNXPROC_BASE_H
#define LIBLNXPROC_BASE_H 1

#ifdef __cplusplus
extern "C" {
#endif

//#include <stddef.h>
#include "error.h"

    struct lnxproc_db_t;
    typedef struct lnxproc_db_t LNXPROC_DB_T;

    const char *lnxproc_db_filename(LNXPROC_DB_T * db);
    int lnxproc_db_print(LNXPROC_DB_T * base);
    LNXPROC_DB_T *lnxproc_db_init(const char *filename,
                                  LNXPROC_ERROR_CALLBACK callback);
    LNXPROC_DB_T *lnxproc_db_free(LNXPROC_DB_T * db);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_DB_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
