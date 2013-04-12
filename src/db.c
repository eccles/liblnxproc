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
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "db_private.h"

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
        return LNXPROC_OK;
    }

    LNXPROC_DEBUG("WARNING: Db is null\n");
    return LNXPROC_ERROR_DB_NULL;
}

LNXPROC_DB_DATA_T 
lnxproc_db_fetch(LNXPROC_DB_T * db, char *key, size_t keylen)
{
    LNXPROC_DEBUG("Db %p\n", db);
    LNXPROC_DB_DATA_T dbkey = {
        .dsize = 0,
        .dptr = NULL,
    };

    if (db) {
        if( !db->db ) {
            LNXPROC_SET_ERROR(db->callback, LNXPROC_ERROR_DB_NOT_OPEN);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_NOT_OPEN, "Fetch db\n");
            return dbkey;
        }
        dbkey.dsize = keylen;
        dbkey.dptr = (unsigned char *)key;
        LNXPROC_DB_DATA_T ret = tdb_fetch(db->db,dbkey);
        if( !ret.dptr ) {
            LNXPROC_SET_ERROR(db->callback, LNXPROC_ERROR_DB_FETCH);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_FETCH, "Fetch db %s\n",
                                   tdb_errorstr(db->db));
        }
        return ret;
    }

    LNXPROC_DEBUG("WARNING: Db is null\n");
    return dbkey;
}

int 
lnxproc_db_store(LNXPROC_DB_T * db, LNXPROC_DB_DATA_T key, LNXPROC_DB_DATA_T data)
{
    LNXPROC_DEBUG("Db %p\n", db);

    if (db) {
        if( !db->db ) {
            LNXPROC_SET_ERROR(db->callback, LNXPROC_ERROR_DB_NOT_OPEN);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_NOT_OPEN, "Store db\n");
            return LNXPROC_ERROR_DB_NOT_OPEN;
        }
        int flag = TDB_REPLACE;
        int ret = tdb_store(db->db, key, data, flag);
        if( ret < 0 ) {
            LNXPROC_SET_ERROR(db->callback, LNXPROC_ERROR_DB_STORE);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_STORE, "Store db %s\n",
                                                       tdb_errorstr(db->db));
            return LNXPROC_ERROR_DB_STORE;
        }
        return LNXPROC_OK;
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

    int hash_size = 0;
    int tdb_flags = TDB_INTERNAL;
    int open_flags = O_RDWR;
    mode_t mode = 0;

    db->db = tdb_open(NULL, hash_size, tdb_flags, open_flags, mode);
    if (!db->db) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_DB_OPEN);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_OPEN, "Open db\n");
        return lnxproc_db_free(db);
    }

    LNXPROC_DEBUG("Successful\n");
    return db;
}

LNXPROC_DB_T *
lnxproc_db_free(LNXPROC_DB_T * db)
{
    LNXPROC_DEBUG("Db %p\n", db);

    if (db) {
        if (db->db) {
            int ret = tdb_close(db->db);

            if (ret < 0) {
                LNXPROC_SET_ERROR(db->callback, LNXPROC_ERROR_DB_CLOSE);
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_DB_CLOSE, "Close db\n");
            }
        }
        LNXPROC_DEBUG("Free Db\n");
        free(db);
        db = NULL;
    }

    return db;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
