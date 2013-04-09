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

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "db.h"

struct lnxproc_db_t {
    const char *filename;
    LNXPROC_ERROR_CALLBACK callback;
};

const char *
lnxproc_db_filename(LNXPROC_DB_T * db)
{
    LNXPROC_DEBUG("Base %p\n", db);
    const char *filename = NULL;

    if (db) {
        filename = db->filename;
    }

    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    return filename;
}

int
lnxproc_db_print(LNXPROC_DB_T * db)
{
    LNXPROC_DEBUG("Db %p\n", db);

    if (db) {
        printf("Filename %s\n", db->filename);
    }

    LNXPROC_DEBUG("WARNING: Db is null\n");
    return LNXPROC_ERROR_DB_NULL;
}

LNXPROC_DB_T *
lnxproc_db_init(const char *filename, LNXPROC_ERROR_CALLBACK callback)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    LNXPROC_DEBUG("callback %p\n", callback);

    LNXPROC_DB_T *db = malloc(sizeof(LNXPROC_DB_T));

    if (!db) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_DB_MALLOC);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_MALLOC, "Malloc db\n");
        return db;
    }

    db->filename = filename;
    db->callback = callback;
    LNXPROC_DEBUG("Successful\n");
    return db;
}

LNXPROC_DB_T *
lnxproc_db_free(LNXPROC_DB_T * db)
{
    LNXPROC_DEBUG("Db %p\n", db);

    if (db) {
        LNXPROC_DEBUG("Free Db\n");
        free(db);
        db = NULL;
    }

    return db;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
