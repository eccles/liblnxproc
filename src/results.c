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
#include "io.h"
#include "strlcpy.h"
#include "reference.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"

int
_lnxproc_results_table_valuestr(_LNXPROC_RESULTS_TABLE_T *entry, char *buf,
                                size_t len, char **res)
{
    int ret = 0;

    if (res && entry) {
        switch (entry->valuetype) {
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_INT:
            ret = int2str(entry->value.i, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNEDINT:
            ret = unsigned2str(entry->value.ui, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_LONG:
            ret = long2str(entry->value.ui, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
            ret = unsignedlong2str(entry->value.ul, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_FLOAT:
            ret = float2str(entry->value.f, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STR:
            //ret = strlcpy(buf, entry->value.s, len);
            *res = entry->value.s;
            ret = strlen(*res);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF:
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREFS:
            ret = strlcpy(buf, entry->value.sptr, len);
            *res = entry->value.sptr;
            ret = strlen(*res);
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_PTR:
            ret = ptr2str(entry->value.ptr, buf, len);
            *res = buf;
            break;
        case _LNXPROC_RESULTS_TABLE_VALUETYPE_NONE:
            ret = strlcpy(buf, "None", len);
            *res = buf;
            break;
        default:
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "valuetype %d",
                                 entry->valuetype);
            buf[0] = '\0';
            *res = buf;
            ret = 1;
            break;
        }
    }
    return ret;
}

#define NDEPTH 6
struct rprint_t {
    char tag[32];
    int fd;
    char *offset[NDEPTH];
    int len[NDEPTH];
    char *key;
    int depth;
};

static int
internal_print_func(_LNXPROC_RESULTS_T *results,
                    _LNXPROC_RESULTS_TABLE_T *entry, void *data)
{
    struct rprint_t *rprint = data;
    char buf[64];
    char *pbuf;

    writestring(rprint->fd, rprint->tag);
    writec(rprint->fd, ' ');
    writestring(rprint->fd, results->tag);
    writestring(rprint->fd, " Key ");
    writen(rprint->fd, entry->key, entry->keylen);
    writec(rprint->fd, '(');
    int n = sizet2str(entry->keylen, buf, sizeof buf);

    writen(rprint->fd, buf, n);
    writestring(rprint->fd, ") = ");

    int buflen = _lnxproc_results_table_valuestr(entry, buf, sizeof buf, &pbuf);

    writen(rprint->fd, pbuf, buflen);
    writec(rprint->fd, '\n');

    return LNXPROC_OK;
}

static int
internal_json_func(_LNXPROC_RESULTS_T *results,
                   _LNXPROC_RESULTS_TABLE_T *entry, void *data)
{
    struct rprint_t *rprint = data;
    char buf[64];

    char sep = entry->key[0];

    _LNXPROC_DEBUG("----> Separator '%c' \n", sep);

    char *key = entry->key + 1;
    int keylen = entry->keylen - 1;

#ifdef DEBUG
    _LNXPROC_DEBUG("Key '%s'(%d)\n", key, keylen);
#endif

    char *offset[NDEPTH];
    int len[NDEPTH];

    offset[0] = key;
    int depth = 0;

    int j = 0;
    int i = 0;

    while (i < keylen) {
        if (key[i] == sep) {
            len[depth++] = i - j;
            _LNXPROC_DEBUG("Len[%d] = %d\n", depth - 1, len[depth - 1]);
            i++;
            j = i;
            offset[depth] = key + i;
            _LNXPROC_DEBUG("Offset[%d] = %ld\n", depth, offset[depth] - key);
        }
        else {
            i++;
        }
    }
    len[depth++] = i - j;
    _LNXPROC_DEBUG("Len[%d] = %d\n", depth - 1, len[depth - 1]);
    _LNXPROC_DEBUG("Depth %d\n", depth);

    if (rprint->key) {
        if (depth < rprint->depth) {
            for (i = rprint->depth; i > depth; i--) {
                writen(rprint->fd, "},\n", 3);
            }
        }
        int d = depth > rprint->depth ? rprint->depth : depth;

        for (i = 0; i < d - 1; i++) {
            if (len[i] != rprint->len[i] ||
                memcmp(offset[i], rprint->offset[i], len[i])) {
                int j;

                for (j = i; j < d - 1; j++) {
                    writen(rprint->fd, "},\n", 3);
                }
                for (j = i; j < d - 1; j++) {
                    writec(rprint->fd, '"');
                    _LNXPROC_DEBUG("%1$d:Offset %2$ld Len %3$d key '%4$*3$s'\n",
                                   j, offset[j] - key, len[j], offset[j]);
                    writen(rprint->fd, offset[j], len[j]);
                    writen(rprint->fd, "\" : {\n", 6);
                }
            }
        }
        for (; i < depth - 1; i++) {
            writec(rprint->fd, '"');
            _LNXPROC_DEBUG("%1$d:Offset %2$ld Len %3$d key '%4$*3$s'\n", i,
                           offset[i] - key, len[i], offset[i]);
            writen(rprint->fd, offset[i], len[i]);
            writen(rprint->fd, "\" : {\n", 6);
        }
    }
    else {
        for (i = 0; i < depth - 1; i++) {
            writec(rprint->fd, '"');
            _LNXPROC_DEBUG("%1$d:Offset %2$ld Len %3$d key '%4$*3$s'\n", i,
                           offset[i] - key, len[i], offset[i]);
            writen(rprint->fd, offset[i], len[i]);
            writen(rprint->fd, "\" : {\n", 6);
        }
    }

    rprint->key = key;
    rprint->depth = depth;
    memcpy(rprint->offset, offset, sizeof(offset));
    memcpy(rprint->len, len, sizeof(len));

    writec(rprint->fd, '"');
    writen(rprint->fd, offset[depth - 1], len[depth - 1]);
    writen(rprint->fd, "\" : \"", 5);

    char *pbuf;
    int buflen = _lnxproc_results_table_valuestr(entry, buf, sizeof buf, &pbuf);

    writen(rprint->fd, pbuf, buflen);
    writen(rprint->fd, "\",\n", 3);

    return LNXPROC_OK;
}

static int
rowcmp(_LNXPROC_RESULTS_TABLE_T *r1, _LNXPROC_RESULTS_TABLE_T *r2)
{
    return strcmp(r1->key, r2->key);
}

int
_lnxproc_results_print(_LNXPROC_RESULTS_T *results, int fd,
                       LNXPROC_PRINT_T print)
{
    _LNXPROC_DEBUG("Results %p\n", results);

    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (print == LNXPROC_PRINT_ALL) {
        char buf[32];

        writen(fd, "Timestamp ", 10);
        lnxproc_timeval_print(&results->tv, buf, sizeof buf);
        writestring(fd, buf);
        writec(fd, '\n');

        writen(fd, "Error ", 6);
        int n = int2str(results->error, buf, sizeof buf);

        writen(fd, buf, n);
        writec(fd, '\n');

        writestring(fd, "Tag = ");
        writestring(fd, results->tag);
        writec(fd, '\n');

        writestring(fd, "Jiffies per second = ");
        n = long2str(results->jiffies_per_sec, buf, sizeof buf);
        writen(fd, buf, n);
        writec(fd, '\n');

        writestring(fd, "Seconds per jiffy = ");
        n = float2str(results->secs_per_jiffy, buf, sizeof buf);
        writen(fd, buf, n);
        writec(fd, '\n');

        writestring(fd, "Page size = ");
        n = long2str(results->page_size, buf, sizeof buf);
        writen(fd, buf, n);
        writec(fd, '\n');

        writestring(fd, "Table size = ");
        n = sizet2str(results->size, buf, sizeof buf);
        writen(fd, buf, n);
        writec(fd, '\n');

        writestring(fd, "Table length = ");
        n = sizet2str(results->length, buf, sizeof buf);
        writen(fd, buf, n);
        writec(fd, '\n');

    }

    _LNXPROC_DEBUG("Hash = %p\n", results->hash);

    struct rprint_t rprint = {
        .fd = fd,
        .key = NULL,
        .depth = 0,
    };

    if (print == LNXPROC_PRINT_ALL) {
        if (results->hash) {
            strlcpy(rprint.tag, "Hash", sizeof rprint.tag);
            _LNXPROC_RESULTS_TABLE_T *entry, *tmp;

            HASH_ITER(hh, results->hash, entry, tmp) {
                internal_print_func(results, entry, &rprint);
            }
        }
    }
    int ret = LNXPROC_OK;;
    if (print == LNXPROC_PRINT_JSON) {
        if (!results->hash) {
            _lnxproc_results_hash(results);
        }
        writec(fd, '"');
        writestring(fd, results->tag);
        writen(fd, "\" : {\n", 6);

        char buf[32];

        writen(fd, "\"timestamp\" : \"", 15);
        lnxproc_timeval_print(&results->tv, buf, sizeof buf);
        writestring(fd, buf);
        writen(fd, "\",\n", 3);

        writen(fd, "\"error\" : \"", 11);
        int n = int2str(results->error, buf, sizeof buf);

        writen(fd, buf, n);
        writen(fd, "\",\n", 3);

        _LNXPROC_RESULTS_TABLE_T *entry, *tmp;

        HASH_ITER(hh, results->hash, entry, tmp) {
            internal_json_func(results, entry, &rprint);
        }
        writen(fd, "}}}}}}}}", rprint.depth);
        writen(fd, ",\n", 2);
    }
    else {
        strlcpy(rprint.tag, "Line", sizeof rprint.tag);

        ret = _lnxproc_results_iterate(results, internal_print_func, &rprint);
    }
    return ret;
}

int
_lnxproc_results_new(_LNXPROC_RESULTS_T **resultsptr, char *tag)
{
    if (!resultsptr) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results address");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (*resultsptr) {
        Acquire(*resultsptr, 0);
        return LNXPROC_OK;
    }

    _LNXPROC_RESULTS_T *p = Acquire(NULL, sizeof(_LNXPROC_RESULTS_T));

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

    *resultsptr = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

int
_lnxproc_results_size(_LNXPROC_RESULTS_T *results, size_t * size)
{
    if (!size) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Size");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    *size += sizeof(*results);

    if (results->tag)
        *size += strlen(results->tag);

    _LNXPROC_RESULTS_TABLE_T *table = results->table;

    int i;

    for (i = 0; i < results->size; i++) {
        _LNXPROC_RESULTS_TABLE_T *entry = table + i;

        *size += sizeof(*entry);

        if (entry->valuetype == _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF) {
            *size += strlen(entry->value.sptr);
        }
    }
    return LNXPROC_OK;
}

void
_lnxproc_results_release(void *arg)
{
    _LNXPROC_DEBUG("Results %p\n", arg);

    if (arg) {
        _LNXPROC_RESULTS_T *results = arg;

        _LNXPROC_DEBUG("Free Results\n");

        _LNXPROC_DEBUG("Free Results tag %p\n", results->tag);
        DESTROY(results->tag);
        _LNXPROC_DEBUG("Free Results hash %p\n", results->hash);
        HASH_CLEAR(hh, results->hash);
        _LNXPROC_RESULTS_TABLE_T *table = results->table;

        if (table) {
            int i;

            for (i = 0; i < results->size; i++) {
                _LNXPROC_RESULTS_TABLE_T *entry = table + i;

                if (entry->valuetype == _LNXPROC_RESULTS_TABLE_VALUETYPE_STRREF) {
                    _LNXPROC_DEBUG
                        ("Free Results table[%1$d] string %2$p '%2$s'\n", i,
                         entry->value.sptr);
                    DESTROY(entry->value.sptr);
                }
            }
            _LNXPROC_DEBUG("Free Results table %p\n", results->table);
            DESTROY(results->table);
        }
    }
}

int
_lnxproc_results_free(_LNXPROC_RESULTS_T **resultsptr)
{
    _LNXPROC_DEBUG("Results %p\n", resultsptr);

    if (resultsptr) {
        RELEASE(*resultsptr, _lnxproc_results_release);
    }

    return LNXPROC_OK;
}

static int
allocate_results_table(_LNXPROC_RESULTS_T *results, size_t nentries)
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
_lnxproc_results_init(_LNXPROC_RESULTS_T *results, size_t nentries)
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
_lnxproc_results_hash(_LNXPROC_RESULTS_T *results)
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
                HASH_ADD(hh, results->hash, key, entry->keylen, entry);
            }
            HASH_SORT(results->hash, rowcmp);
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_results_last(_LNXPROC_RESULTS_T *results,
                      _LNXPROC_RESULTS_TABLE_T **entry)
{
    if (!results) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Results");
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
    *entry = results->table + results->length - 1;
    return LNXPROC_OK;
}

int
_lnxproc_results_fetch(_LNXPROC_RESULTS_T *results, char *key,
                       _LNXPROC_RESULTS_TABLE_T **entry)
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
        char *pbuf;

        _lnxproc_results_table_valuestr(tentry, buf, sizeof buf, &pbuf);
        _LNXPROC_DEBUG("Value %s\n", pbuf);
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
                char *pbuf;

                _lnxproc_results_table_valuestr(tentry, buf, sizeof buf, &pbuf);

                _LNXPROC_DEBUG("Value %s\n", pbuf);
#endif
                return LNXPROC_OK;
            }
        }
    }
    return LNXPROC_ERROR_NOT_FOUND;
}

static int
prepare_entry(_LNXPROC_RESULTS_T *results, const char *key,
              _LNXPROC_RESULTS_TABLE_T **tentry)
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
_lnxproc_results_add_float(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_string(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_stringref(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_ptr(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_int(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_long(_LNXPROC_RESULTS_T *results, const char *key,
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
_lnxproc_results_add_unsigned_long(_LNXPROC_RESULTS_T *results,
                                   const char *key, const unsigned long value)
{
    _LNXPROC_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _LNXPROC_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG;
    tentry->value.ul = value;
    return LNXPROC_OK;
}

int
_lnxproc_results_iterate(_LNXPROC_RESULTS_T *results,
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
