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
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "error_private.h"
#include "results_private.h"

char *
_lnxproc_results_table_valuestr(_LNXPROC_RESULTS_TABLE_T * entry, char *buf,
                                size_t len)
{
    if (entry && buf) {
        switch (entry->valuetype) {
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_INT:
            snprintf(buf, len, "%d", entry->value.i);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT:
            snprintf(buf, len, "%u", entry->value.ui);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG:
            snprintf(buf, len, "%ld", entry->value.l);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
            snprintf(buf, len, "%lu", entry->value.ul);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT:
            snprintf(buf, len, "%f", entry->value.f);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STR:
            snprintf(buf, len, "%s", entry->value.s);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR:
            snprintf(buf, len, "%p", entry->value.p);
            break;
        }
    }
    return buf;
}

LNXPROC_ERROR_T
_lnxproc_results_table_copy(_LNXPROC_RESULTS_TABLE_T * dest,
                            _LNXPROC_RESULTS_TABLE_T * src)
{
    if (src && dest) {
        switch (src->valuetype) {
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_INT:
            dest->valuetype = src->valuetype;
            dest->value.i = src->value.i;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT:
            dest->valuetype = src->valuetype;
            dest->value.ui = src->value.ui;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG:
            dest->valuetype = src->valuetype;
            dest->value.l = src->value.l;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
            dest->valuetype = src->valuetype;
            dest->value.ul = src->value.ul;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT:
            dest->valuetype = src->valuetype;
            dest->value.f = src->value.f;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STR:
            dest->valuetype = src->valuetype;
            strcpy(dest->value.s, src->value.s);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR:
            dest->valuetype = src->valuetype;
            dest->value.p = src->value.p;
            break;
        }
    }
    return LNXPROC_OK;
}

static int
internal_print_func(_LNXPROC_RESULTS_TABLE_T * entry, void *data)
{
    char buf[64];

    printf("Key %s = %s\n", entry->key,
           _lnxproc_results_table_valuestr(entry, buf, sizeof buf));
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_print(_LNXPROC_RESULTS_T * results)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    printf("Jiffies per second = %ld\n", results->jiffies_per_sec);
    printf("Seconds per jiffy = %f\n", results->secs_per_jiffy);
    printf("Page size = %ld\n", results->page_size);
    printf("Table size = %zd\n", results->size);
    printf("Table length = %zd\n", results->length);
    printf("Hash = %p\n", results->hash);
    if (results->hash) {
        char buf[64];
        _LNXPROC_RESULTS_TABLE_T *entry, *tmp;

        HASH_ITER(hh, results->hash, entry, tmp) {
            printf("Hash Key: %s = %s\n", entry->key,
                   _lnxproc_results_table_valuestr(entry, buf, sizeof buf));
        }
    }
    return _lnxproc_results_iterate(results, internal_print_func, NULL);

}

LNXPROC_ERROR_T
_lnxproc_results_new(_LNXPROC_RESULTS_T ** results)
{
    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_RESULTS_T %lu\n",
                   sizeof(_LNXPROC_RESULTS_T));
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_NULL;
    }

    if (*results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL,
                             "\n");
        return LNXPROC_ERROR_RESULTS_ADDRESS_CONTENTS_NOT_NULL;
    }

    _LNXPROC_RESULTS_T *p = calloc(1, sizeof(_LNXPROC_RESULTS_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    p->jiffies_per_sec = sysconf(_SC_CLK_TCK);
    p->secs_per_jiffy = 1.0 / p->jiffies_per_sec;
    p->page_size = sysconf(_SC_PAGE_SIZE) / 1024;

    *results = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_free(_LNXPROC_RESULTS_T ** resultsptr)
{
    _LNXPROC_DEBUG("Results %p\n", resultsptr);

    if (resultsptr && *resultsptr) {
        _LNXPROC_RESULTS_T *results = *resultsptr;

        _LNXPROC_DEBUG("Free Results\n");
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);
        if (results->table) {
            free(results->table);
            results->table = NULL;
        }
        free(results);
        results = NULL;
    }

    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
realloc_results_table(_LNXPROC_RESULTS_T * results, size_t nentries)
{
    _LNXPROC_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    _LNXPROC_DEBUG("Sizeof table entry %zd\n",
                   sizeof(_LNXPROC_RESULTS_TABLE_T));

    size_t nsize =
        (nentries + results->size) * sizeof(_LNXPROC_RESULTS_TABLE_T);
    _LNXPROC_DEBUG("Nsize %zd\n", nsize);
    _LNXPROC_RESULTS_TABLE_T *t = realloc(results->table, nsize);

    if (!t) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_TABLE_MALLOC, "\n");
        return LNXPROC_ERROR_RESULTS_TABLE_MALLOC;
    }
    int i;

    _LNXPROC_DEBUG("New Table %p\n", t);

    for (i = results->size; i < (nentries + results->size); i++) {
        _LNXPROC_RESULTS_TABLE_T *entry = t + i;

        _LNXPROC_DEBUG("Clear %lu bytes at %p\n",
                       sizeof(_LNXPROC_RESULTS_TABLE_T), entry);
        memset(entry, 0, sizeof(_LNXPROC_RESULTS_TABLE_T));
    }
    results->table = t;
    results->size += nentries;

    _LNXPROC_DEBUG("New Table size %zd\n", results->size);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_init(_LNXPROC_RESULTS_T * results, size_t nentries)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    HASH_CLEAR(hh, results->hash);
    _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);

    _LNXPROC_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    if (nentries > results->size) {
        LNXPROC_ERROR_T ret =
            realloc_results_table(results, nentries - results->size);

        if (ret) {
            _LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
    }
    memset(results->table, 0, results->size * sizeof(_LNXPROC_RESULTS_TABLE_T));
    results->length = 0;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_hash(_LNXPROC_RESULTS_T * results)
{
    if (results) {
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);
        if (table) {
            int i;

            for (i = 0; i < results->length; i++) {
                _LNXPROC_RESULTS_TABLE_T *entry = table + i;

                _LNXPROC_DEBUG("%d key %s\n", i, entry->key);
                HASH_ADD_STR(results->hash, key, entry);
            }
        }
    }

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_fetch(_LNXPROC_RESULTS_T * results,
                       _LNXPROC_RESULTS_TABLE_T * entry)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    if (!entry) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL;
    }

    if (!results->table) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_TABLE_NULL,
                             "Fetch results\n");
        return LNXPROC_ERROR_RESULTS_TABLE_NULL;
    }
    _LNXPROC_DEBUG("Table %p\n", results->table);

    if (results->hash) {
        _LNXPROC_RESULTS_TABLE_T *tentry;

        HASH_FIND_STR(results->hash, entry->key, tentry);
        if (tentry) {
            _lnxproc_results_table_copy(entry, tentry);
#ifdef DEBUG
            char buf[64];

            _LNXPROC_DEBUG("Value %s\n",
                           _lnxproc_results_table_valuestr(entry, buf,
                                                           sizeof buf));
#endif
        }
    }
    else {
        int i;
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            _LNXPROC_DEBUG("%d key %s\n", i, table[i].key);
            if (!strcmp(table[i].key, entry->key)) {
                _lnxproc_results_table_copy(entry, table + i);
#ifdef DEBUG
                char buf[64];

                _LNXPROC_DEBUG("Value %s\n",
                               _lnxproc_results_table_valuestr(entry, buf,
                                                               sizeof buf));
#endif
                return LNXPROC_OK;
            }
        }
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_add(_LNXPROC_RESULTS_T * results,
                     _LNXPROC_RESULTS_TABLE_T * entry)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }
    if (!entry) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL;
    }

#ifdef DEBUG
    char buf[64];

    _LNXPROC_DEBUG("Results %1$p Entry Key %2$p '%2$s' Value %3$p '%3$s'\n",
                   results, entry->key,
                   _lnxproc_results_table_valuestr(entry, buf, sizeof buf));
#endif

    if (!results->table) {
        results->length = 0;
        results->size = 0;
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);
    }
    _LNXPROC_DEBUG("Table %p\n", results->table);

    if (results->length >= results->size) {
        LNXPROC_ERROR_T ret = realloc_results_table(results, 1);

        if (ret) {
            _LNXPROC_ERROR_DEBUG(ret, "Add results\n");
            return ret;
        }
    }
    _LNXPROC_DEBUG("Table %p\n", results->table);

    _LNXPROC_RESULTS_TABLE_T *tentry = results->table + results->length;

    memcpy(tentry, entry, sizeof(*tentry));
    results->length++;

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                         _LNXPROC_RESULTS_ITERATE_FUNC func, void *data)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    _LNXPROC_DEBUG("Results %p\n", results);
    if (results->table) {
        int i;
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            func(table + i, data);
        }
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
