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

#include <unistd.h>             //sysconf

#include "allocate.h"
#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"

char *
_lnxproc_results_table_valuestr(_LNXPROC_RESULTS_TABLE_T * entry, char *buf,
                                size_t len, int copy)
{
    char *ret = buf;

    if (entry) {
        switch (entry->valuetype) {
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_INT:
            int2str(entry->value.i, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT:
            unsigned2str(entry->value.ui, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG:
            long2str(entry->value.ui, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
            unsignedlong2str(entry->value.ul, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT:
            float2str(entry->value.f, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STR:
            if (copy) {
                if (buf)
                    strlcpy(buf, entry->value.s, len);
            }
            else {
                ret = entry->value.s;
            }
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF:
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREFS:
            if (copy) {
                if (buf)
                    strlcpy(buf, entry->value.sptr, len);
            }
            else {
                ret = entry->value.sptr;
            }
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR:
            ptr2str(entry->value.ptr, buf, len);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_NONE:
            if (buf)
                strlcpy(buf, "None", len);
            break;
        default:
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "valuetype %d",
                                 entry->valuetype);
            buf[0] = '\0';
            break;
        }
    }
    return ret;
}

static int
internal_print_func(_LNXPROC_RESULTS_T * results,
                    _LNXPROC_RESULTS_TABLE_T * entry, void *data)
{
    char buf[64];

    printf("%s %s Key %s(%zd) = %s\n", (char *) data, results->tag, entry->key,
           entry->keylen, _lnxproc_results_table_valuestr(entry, buf,
                                                          sizeof buf, 0));
    return LNXPROC_OK;
}

int
_lnxproc_results_print(_LNXPROC_RESULTS_T * results)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    printf("Tag = %s\n", results->tag);
    printf("Jiffies per second = %ld\n", results->jiffies_per_sec);
    printf("Seconds per jiffy = %f\n", results->secs_per_jiffy);
    printf("Page size = %ld\n", results->page_size);
    printf("Table size = %zd\n", results->size);
    printf("Table length = %zd\n", results->length);
    _LNXPROC_DEBUG("Hash = %p\n", results->hash);
    char buf[32];

    if (results->hash) {
        strlcpy(buf, "Hash", sizeof buf);
        _LNXPROC_RESULTS_TABLE_T *entry, *tmp;

        HASH_ITER(hh, results->hash, entry, tmp) {
            internal_print_func(results, entry, buf);
        }
    }
    strlcpy(buf, "Line", sizeof buf);
    return _lnxproc_results_iterate(results, internal_print_func, buf);

}

int
_lnxproc_results_new(_LNXPROC_RESULTS_T ** results, char *tag)
{
    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof _LNXPROC_RESULTS_T %lu\n",
                   sizeof(_LNXPROC_RESULTS_T));
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results address");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (*results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results not null");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_RESULTS_T *p = Allocate(NULL, sizeof(_LNXPROC_RESULTS_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Results");
        return LNXPROC_ERROR_MALLOC;
    }

    if (tag) {
        p->tag = strdup(tag);
        if (!p->tag) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Results tag");
            _LNXPROC_RESULTS_FREE(p);
            return LNXPROC_ERROR_MALLOC;
        }
    }
    else {
        p->tag = NULL;
    }
    p->jiffies_per_sec = sysconf(_SC_CLK_TCK);
    p->secs_per_jiffy = 1.0 / p->jiffies_per_sec;
    p->page_size = sysconf(_SC_PAGE_SIZE) / 1024;

    *results = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

int
_lnxproc_results_free(_LNXPROC_RESULTS_T ** resultsptr)
{
    _LNXPROC_DEBUG("Results %p\n", resultsptr);

    if (resultsptr && *resultsptr) {
        _LNXPROC_DEBUG("Free Results\n");
        _LNXPROC_RESULTS_T *results = *resultsptr;

        DESTROY(results->tag);
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        if (table) {
            int i;

            for (i = 0; i < results->size; i++) {
                _LNXPROC_RESULTS_TABLE_T *entry = table + i;

                if (entry->valuetype == _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF) {
                    DESTROY(entry->value.sptr);
                }
            }
            DESTROY(results->table);
        }
        DESTROY(results);
        *resultsptr = NULL;
    }

    return LNXPROC_OK;
}

static int
allocate_results_table(_LNXPROC_RESULTS_T * results, size_t nentries)
{
    _LNXPROC_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    _LNXPROC_DEBUG("Sizeof table entry %zd\n",
                   sizeof(_LNXPROC_RESULTS_TABLE_T));

    size_t nsize =
        (nentries + results->size) * sizeof(_LNXPROC_RESULTS_TABLE_T);
    _LNXPROC_DEBUG("Nsize %zd\n", nsize);
    _LNXPROC_RESULTS_TABLE_T *t = Allocate(results->table, nsize);

    if (!t) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Results table");
        return LNXPROC_ERROR_MALLOC;
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

int
_lnxproc_results_init(_LNXPROC_RESULTS_T * results, size_t nentries)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    HASH_CLEAR(hh, results->hash);
    _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);

    _LNXPROC_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    if (nentries > results->size) {
        int ret = allocate_results_table(results, nentries - results->size);

        if (ret) {
            return ret;
        }
    }
    results->length = 0;
    return LNXPROC_OK;
}

int
_lnxproc_results_hash(_LNXPROC_RESULTS_T * results)
{
    if (results) {
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);
        if (table) {
            if (results->length > 10) {
                int i;

                for (i = 0; i < results->length; i++) {
                    _LNXPROC_RESULTS_TABLE_T *entry = table + i;

                    _LNXPROC_DEBUG("%d key %s\n", i, entry->key);
                    HASH_ADD(hh, results->hash, key, entry->keylen, entry);
                }
            }
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_results_fetch(_LNXPROC_RESULTS_T * results, char *key,
                       _LNXPROC_RESULTS_TABLE_T ** entry)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (!key) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results key");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (!entry) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results entry");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (*entry) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Results entry contents not null");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!results->table) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results table");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    _LNXPROC_DEBUG("Table %p\n", results->table);

    if (results->hash) {
        _LNXPROC_RESULTS_TABLE_T *tentry;

        HASH_FIND_STR(results->hash, key, tentry);
        if (!tentry) {
            return LNXPROC_ERROR_NOT_FOUND;
        }
        *entry = tentry;
#ifdef DEBUG
        char buf[64];

        _LNXPROC_DEBUG("Value %s\n",
                       _lnxproc_results_table_valuestr(tentry, buf, sizeof buf,
                                                       0));
#endif
        return LNXPROC_OK;
    }
    else {
        int i;
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            _LNXPROC_RESULTS_TABLE_T *tentry = table + i;

            _LNXPROC_DEBUG("%d key %s\n", i, tentry->key);
            if (!strcmp(tentry->key, key)) {
                *entry = tentry;
#ifdef DEBUG
                char buf[64];

                _LNXPROC_DEBUG("Value %s\n",
                               _lnxproc_results_table_valuestr(tentry, buf,
                                                               sizeof buf, 0));
#endif
                return LNXPROC_OK;
            }
        }
    }
    return LNXPROC_ERROR_NOT_FOUND;
}

static int
prepare_entry(_LNXPROC_RESULTS_T * results, const char *key,
              _LNXPROC_RESULTS_TABLE_T ** tentry)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (!key) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results key");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!results->table) {
        results->length = 0;
        results->size = 0;
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_DEBUG("Table hash freed to %p\n", results->hash);
    }
    _LNXPROC_DEBUG("Table %p\n", results->table);

    if (results->length >= results->size) {
        int ret = allocate_results_table(results, 1);

        if (ret) {
            return ret;
        }
    }
    _LNXPROC_DEBUG("New Table %p\n", results->table);

    results->length++;

    _LNXPROC_RESULTS_TABLE_T *entry = results->table + results->length - 1;

    _LNXPROC_DEBUG("Entry %p(%zd) Type %d\n", entry, results->length - 1,
                   entry->valuetype);
    if (entry->valuetype == _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF) {
        DESTROY(entry->value.sptr);
    }

#ifdef DEBUG
    if (strlen(key) >= sizeof(entry->key)) {
        _LNXPROC_DEBUG("WARNING:%s:Key length %lu > table key length %zd\n",
                       results->tag, strlen(key), sizeof(entry->key) - 1);
    }
#endif

    entry->keylen = strlcpy(entry->key, key, sizeof(entry->key));
    *tentry = entry;
    return LNXPROC_OK;
}

int
_lnxproc_results_add_float(_LNXPROC_RESULTS_T * results, const char *key,
                           const float value)
{
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT;
    tentry->value.f = value;
    return LNXPROC_OK;
}

int
_lnxproc_results_add_string(_LNXPROC_RESULTS_T * results, const char *key,
                            const char *value, size_t valuelen)
{
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    if (valuelen < 1)
        valuelen = strlen(value);

    if (valuelen >= _LNXPROC_RESULTS_TABLE_VALLEN) {
        tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF;
        tentry->value.sptr = strdup(value);
        _LNXPROC_DEBUG("%s:Allocate %p at entry %zd\n", results->tag,
                       tentry->value.sptr, results->length - 1);
        if (!tentry->value.sptr) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC,
                                 "Results add string reference");
            return LNXPROC_ERROR_MALLOC;
        }
    }
    else {
        tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_STR;
        strlcpy(tentry->value.s, value, sizeof tentry->value.s);
    }
    return LNXPROC_OK;
}

int
_lnxproc_results_add_stringref(_LNXPROC_RESULTS_T * results, const char *key,
                               const char *value)
{
    if (!value) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results value");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREFS;
    tentry->value.sptr = (char *) value;
    return LNXPROC_OK;
}

int
_lnxproc_results_add_ptr(_LNXPROC_RESULTS_T * results, const char *key,
                         const void *value)
{
    if (!value) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results value");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR;
    tentry->value.ptr = (void *) value;
    return LNXPROC_OK;
}

int
_lnxproc_results_add_int(_LNXPROC_RESULTS_T * results, const char *key,
                         const int value)
{
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_INT;
    tentry->value.i = value;
    return LNXPROC_OK;
}

int
_lnxproc_results_add_long(_LNXPROC_RESULTS_T * results, const char *key,
                          const long value)
{
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG;
    tentry->value.l = value;
    return LNXPROC_OK;
}

int
_lnxproc_results_iterate(_LNXPROC_RESULTS_T * results,
                         _LNXPROC_RESULTS_ITERATE_FUNC func, void *data)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_DEBUG("Results %p\n", results);
    if (results->table) {
        int i;
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        _LNXPROC_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            func(results, table + i, data);
        }
    }
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
