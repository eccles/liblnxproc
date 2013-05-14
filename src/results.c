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

static int
internal_print_func(char *key, char *value, void *data)
{
    printf("Key %s = %s\n", key, value);
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
    return _lnxproc_results_iterate(results, internal_print_func, NULL);

}

LNXPROC_ERROR_T
_lnxproc_results_new(_LNXPROC_RESULTS_T ** results)
{
    _LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_RESULTS_T %d\n",
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

    p->size = 0;
    p->length = 0;
    p->table = NULL;

    p->jiffies_per_sec = sysconf(_SC_CLK_TCK);
    p->secs_per_jiffy = 1.0 / p->jiffies_per_sec;
    p->page_size = sysconf(_SC_PAGE_SIZE) / 1024;

    *results = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

_LNXPROC_RESULTS_T *
_lnxproc_results_free(_LNXPROC_RESULTS_T * results)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (results) {
        _LNXPROC_DEBUG("Free Results\n");
        if (results->table) {
            free(results->table);
            results->table = NULL;
        }
        free(results);
        results = NULL;
    }

    return results;
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
        _LNXPROC_DEBUG("Clear %d bytes at %p\n",
                       sizeof(_LNXPROC_RESULTS_TABLE_T), t + i);
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

    _LNXPROC_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    if (nentries > results->size) {
        LNXPROC_ERROR_T ret =
            realloc_results_table(results, nentries - results->size);

        if (ret) {
            _LNXPROC_ERROR_DEBUG(ret, "\n");
            return ret;
        }
    }
    results->length = 0;
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_fetch(_LNXPROC_RESULTS_T * results, char **value,
                       char *fmt, ...)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    *value = NULL;
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    if (!fmt) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_KEY_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_KEY_NULL;
    }

    if (!value) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_VAL_ADDRESS_NULL;
    }

    if (!results->table) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_TABLE_NULL,
                             "Fetch results\n");
        return LNXPROC_ERROR_RESULTS_TABLE_NULL;
    }

    char key[32];
    va_list ap;

    va_start(ap, fmt);
#ifdef DEBUG
    size_t ksize = 1 + vsnprintf(key, sizeof key, fmt, ap);

    if (ksize > sizeof key) {
        _LNXPROC_DEBUG("WARNING: Key length %zd exceeds %zd bytes\n", ksize,
                       sizeof key);
        ksize = sizeof key;
    }
#else
    vsnprintf(key, sizeof key, fmt, ap);
#endif

    va_end(ap);

    _LNXPROC_DEBUG("Key %s\n", key);
    int i;
    _LNXPROC_RESULTS_TABLE_T *table = results->table;

    for (i = 0; i < results->length; i++) {
        _LNXPROC_DEBUG("%d key %s\n", i, table[i].key);
        if (!strcmp(table[i].key, key)) {
            *value = table[i].value;
            _LNXPROC_DEBUG("Value %s\n", table[i].value);
            return LNXPROC_OK;
        }
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_results_add(_LNXPROC_RESULTS_T * results, const char *value,
                     char *fmt, ...)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_RESULTS_NULL, "\n");
        return LNXPROC_ERROR_RESULTS_NULL;
    }

    _LNXPROC_DEBUG("Results %p Value %p '%s'\n", results, value, value);

    if (!results->table) {
        results->length = 0;
        results->size = 0;
    }
    if (results->length >= results->size) {
        LNXPROC_ERROR_T ret = realloc_results_table(results, 1);

        if (ret) {
            _LNXPROC_ERROR_DEBUG(ret, "Add results\n");
            return ret;
        }
    }
    va_list ap;

    _LNXPROC_RESULTS_TABLE_T *entry = results->table + results->length;

    va_start(ap, fmt);
#ifdef DEBUG
    size_t ksize = 1 + vsnprintf(entry->key, sizeof entry->key, fmt, ap);

    if (ksize > sizeof entry->key) {
        _LNXPROC_DEBUG("WARNING: Key length %zd exceeds %zd bytes\n", ksize,
                       sizeof entry->key);
        ksize = sizeof entry->key;
    }
#else
    vsnprintf(entry->key, sizeof entry->key, fmt, ap);
#endif

    va_end(ap);

    _LNXPROC_DEBUG("Key %d : '%s'\n", ksize, entry->key);

    size_t dsize = 1 + strlen(value);

    if (dsize > sizeof entry->value) {
        _LNXPROC_DEBUG("WARNING: Value length %zd exceeds %zd bytes\n", dsize,
                       sizeof entry->value);
        dsize = sizeof entry->value;
    }
    memcpy(entry->value, value, dsize);

    _LNXPROC_DEBUG("Value %d : '%s'\n", dsize, entry->value);

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

        for (i = 0; i < results->length; i++) {
            func(table[i].key, table[i].value, data);
        }
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
