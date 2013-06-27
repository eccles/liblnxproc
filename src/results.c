/*
 * This file is part of topiary.
 *
 *  topiary is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  topiary is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with topiary.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#include "config.h"

#include <ctype.h>              //isdigit
#include <unistd.h>             //sysconf

#include "allocate.h"
#include "io.h"
#include "strlcpy.h"
#include "reference.h"
#include "val2str.h"
#include "error_private.h"
#include "results_private.h"

int
_topiary_results_table_valuestr(_TOPIARY_RESULTS_TABLE_T *entry, char *buf,
                                size_t len, char **res)
{
    int ret = 0;

    if (res && entry) {
        switch (entry->valuetype) {
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_INT:
            ret = int2str(entry->value.i, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_INT:
            ret = unsigned2str(entry->value.ui, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_LONG:
            ret = long2str(entry->value.ui, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
            ret = unsignedlong2str(entry->value.ul, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_FLOAT:
            ret = float2str(entry->value.f, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_FIXED:
            ret =
                fixed2str(entry->value.f, entry->width, entry->precision, buf,
                          len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STR:
            *res = entry->value.s;
            ret = strlen(*res);
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREFS:
            ret = strlcpy(buf, entry->value.sptr, len);
            *res = entry->value.sptr;
            ret = strlen(*res);
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_PTR:
            ret = ptr2str(entry->value.ptr, buf, len);
            *res = buf;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_NONE:
            ret = strlcpy(buf, "None", len);
            *res = buf;
            break;
        default:
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "valuetype %d",
                                 entry->valuetype);
            buf[0] = '\0';
            *res = buf;
            ret = TOPIARY_ERROR_ILLEGAL_ARG;
            break;
        }
    }
    return ret;
}

/*
 * used to detremine if quotes must be used when outputting JSON.
 */
static int
_topiary_results_table_valuenumeric(_TOPIARY_RESULTS_TABLE_T *entry,
                                    int *numeric)
{
    int ret = 0;

    if (numeric && entry) {
        switch (entry->valuetype) {
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_INT:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_INT:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_LONG:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_FLOAT:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_FIXED:
            *numeric = 1;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STR:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREFS:
            *numeric = 0;
            break;
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_PTR:
        case _TOPIARY_RESULTS_TABLE_VALUETYPE_NONE:
        default:
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "valuetype %d",
                                 entry->valuetype);
            *numeric = -1;
            ret = TOPIARY_ERROR_ILLEGAL_ARG;
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
    int comma;
};

static int
internal_print_func(_TOPIARY_RESULTS_T *results,
                    _TOPIARY_RESULTS_TABLE_T *entry, void *data)
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

    int buflen = _topiary_results_table_valuestr(entry, buf, sizeof buf, &pbuf);

    writen(rprint->fd, pbuf, buflen);
    writec(rprint->fd, '\n');

    return TOPIARY_OK;
}

static void
writecomma(struct rprint_t *rprint)
{
    if (rprint->comma) {
        writec(rprint->fd, ',');
    }
    else {
        rprint->comma = 1;
    }
}

static void
writekey(struct rprint_t *rprint, char *str, int len)
{
    _TOPIARY_DEBUG("Len %1$d key '%2$*1$s'\n", len, str);

    writecomma(rprint);
    writec(rprint->fd, '"');
    writen(rprint->fd, str, len);
    writec(rprint->fd, '"');
    writec(rprint->fd, ':');
}

static void
writeobject(struct rprint_t *rprint, char *str, int len)
{
    _TOPIARY_DEBUG("Len %1$d key '%2$*1$s'\n", len, str);

    writekey(rprint, str, len);
    writen(rprint->fd, "{\n", 2);
    rprint->comma = 0;
}

static int
internal_json_func(_TOPIARY_RESULTS_T *results,
                   _TOPIARY_RESULTS_TABLE_T *entry, void *data)
{
    struct rprint_t *rprint = data;
    char buf[64];

    char sep = entry->key[0];

    _TOPIARY_DEBUG("----> Separator '%c' \n", sep);

    char *key = entry->key + 1;
    int keylen = entry->keylen - 1;

#ifdef DEBUG
    _TOPIARY_DEBUG("Key '%s'(%d)\n", key, keylen);
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
            _TOPIARY_DEBUG("Len[%d] = %d\n", depth - 1, len[depth - 1]);
            i++;
            j = i;
            offset[depth] = key + i;
            _TOPIARY_DEBUG("Offset[%d] = %ld\n", depth, offset[depth] - key);
        }
        else {
            i++;
        }
    }
    len[depth++] = i - j;
    _TOPIARY_DEBUG("Len[%d] = %d\n", depth - 1, len[depth - 1]);
    _TOPIARY_DEBUG("Depth %d\n", depth);

    if (rprint->key) {
        if (depth < rprint->depth) {
            writen(rprint->fd, "}}}}}}}}", rprint->depth - depth);
            writec(rprint->fd, '\n');
        }
        int d = depth > rprint->depth ? rprint->depth : depth;

        for (i = 0; i < d - 1; i++) {
            if (len[i] != rprint->len[i] ||
                memcmp(offset[i], rprint->offset[i], len[i])) {
                int j;

                writen(rprint->fd, "}}}}}}}}", d - 1 - i);
                writec(rprint->fd, '\n');
                for (j = i; j < d - 1; j++) {
                    writeobject(rprint, offset[j], len[j]);
                }
            }
        }
        for (; i < depth - 1; i++) {
            writeobject(rprint, offset[i], len[i]);
        }
    }
    else {
        for (i = 0; i < depth - 1; i++) {
            writeobject(rprint, offset[i], len[i]);
        }
    }

    rprint->key = key;
    rprint->depth = depth;
    memcpy(rprint->offset, offset, sizeof(offset));
    memcpy(rprint->len, len, sizeof(len));

    writekey(rprint, offset[depth - 1], len[depth - 1]);

    char *pbuf;
    int buflen = _topiary_results_table_valuestr(entry, buf, sizeof buf, &pbuf);

    int numeric;

    _topiary_results_table_valuenumeric(entry, &numeric);

    if (numeric == 0) {
        writec(rprint->fd, '"');
    }
    writen(rprint->fd, pbuf, buflen);
    if (numeric == 0) {
        writec(rprint->fd, '"');
    }
    writec(rprint->fd, '\n');

    return TOPIARY_OK;
}

static int
rowcmp(_TOPIARY_RESULTS_TABLE_T *r1, _TOPIARY_RESULTS_TABLE_T *r2)
{
    return strcmp(r1->key, r2->key);
}

int
_topiary_results_print(_TOPIARY_RESULTS_T *results, int fd,
                       TOPIARY_PRINT_T print)
{
    _TOPIARY_DEBUG("Results %p\n", results);

    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (print == TOPIARY_PRINT_ALL) {
        char buf[32];

        writen(fd, "Timestamp ", 10);
        topiary_timeval_print(&results->tv, buf, sizeof buf);
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
        n = fixed2str(results->secs_per_jiffy, 5, 3, buf, sizeof buf);
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

    _TOPIARY_DEBUG("Hash = %p\n", results->hash);

    struct rprint_t rprint = {
        .fd = fd,
        .key = NULL,
        .depth = 0,
        .comma = 1,
    };

    if (print == TOPIARY_PRINT_ALL) {
        if (results->hash) {
            strlcpy(rprint.tag, "Hash", sizeof rprint.tag);
            _TOPIARY_RESULTS_TABLE_T *entry, *tmp;

            HASH_ITER(hh, results->hash, entry, tmp) {
                internal_print_func(results, entry, &rprint);
            }
        }
    }
    int ret = TOPIARY_OK;;
    if (print == TOPIARY_PRINT_JSON) {
        if (!results->hash) {
            _topiary_results_hash(results);
        }
        writec(fd, '"');
        writestring(fd, results->tag);
        writen(fd, "\":{\n", 4);

        char buf[32];

        writen(fd, "\"timestamp\":", 12);
        topiary_timeval_print(&results->tv, buf, sizeof buf);
        writestring(fd, buf);
        writen(fd, ",\n", 2);

        writen(fd, "\"error\":", 8);
        int n = int2str(results->error, buf, sizeof buf);

        writen(fd, buf, n);
        writec(fd, '\n');

        _TOPIARY_RESULTS_TABLE_T *entry, *tmp;

        HASH_ITER(hh, results->hash, entry, tmp) {
            internal_json_func(results, entry, &rprint);
        }
        writen(fd, "}}}}}}}}", rprint.depth);
        writec(fd, '\n');
    }
    else {
        strlcpy(rprint.tag, "Line", sizeof rprint.tag);

        ret = _topiary_results_iterate(results, internal_print_func, &rprint);
    }
    return ret;
}

int
_topiary_results_new(_TOPIARY_RESULTS_T **resultsptr, char *tag)
{
    if (!resultsptr) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results address");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (*resultsptr) {
        Acquire(*resultsptr, 0);
        return TOPIARY_OK;
    }

    _TOPIARY_RESULTS_T *p = Acquire(NULL, sizeof(_TOPIARY_RESULTS_T));

    if (!p) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Results");
        return TOPIARY_ERROR_MALLOC;
    }

    if (tag) {
        p->tag = strdup(tag);
        if (!p->tag) {
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Results tag");
            _TOPIARY_RESULTS_FREE(p);
            return TOPIARY_ERROR_MALLOC;
        }
    }
    else {
        p->tag = NULL;
    }
    p->jiffies_per_sec = sysconf(_SC_CLK_TCK);
    p->secs_per_jiffy = 1.0 / p->jiffies_per_sec;
    p->page_size = sysconf(_SC_PAGE_SIZE) / 1024;

    *resultsptr = p;
    _TOPIARY_DEBUG("Successful\n");
    return TOPIARY_OK;
}

int
_topiary_results_size(_TOPIARY_RESULTS_T *results, size_t * size)
{
    if (!size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Size");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size += sizeof(*results);

    if (results->tag)
        *size += strlen(results->tag);

    _TOPIARY_RESULTS_TABLE_T *table = results->table;

    int i;

    for (i = 0; i < results->size; i++) {
        _TOPIARY_RESULTS_TABLE_T *entry = table + i;

        *size += sizeof(*entry);

        if (entry->valuetype == _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF) {
            *size += strlen(entry->value.sptr);
        }
    }
    return TOPIARY_OK;
}

void
_topiary_results_release(void *arg)
{
    _TOPIARY_DEBUG("Results %p\n", arg);

    if (arg) {
        _TOPIARY_RESULTS_T *results = arg;

        _TOPIARY_DEBUG("Free Results\n");

        _TOPIARY_DEBUG("Free Results tag %p\n", results->tag);
        DESTROY(results->tag);
        _TOPIARY_DEBUG("Free Results hash %p\n", results->hash);
        HASH_CLEAR(hh, results->hash);
        _TOPIARY_RESULTS_TABLE_T *table = results->table;

        if (table) {
            int i;

            for (i = 0; i < results->size; i++) {
                _TOPIARY_RESULTS_TABLE_T *entry = table + i;

                if (entry->valuetype == _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF) {
                    _TOPIARY_DEBUG
                        ("Free Results table[%1$d] string %2$p '%2$s'\n", i,
                         entry->value.sptr);
                    DESTROY(entry->value.sptr);
                }
            }
            _TOPIARY_DEBUG("Free Results table %p\n", results->table);
            DESTROY(results->table);
        }
    }
}

int
_topiary_results_free(_TOPIARY_RESULTS_T **resultsptr)
{
    _TOPIARY_DEBUG("Results %p\n", resultsptr);

    if (resultsptr) {
        RELEASE(*resultsptr, _topiary_results_release);
    }

    return TOPIARY_OK;
}

static int
allocate_results_table(_TOPIARY_RESULTS_T *results, size_t nentries)
{
    _TOPIARY_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    _TOPIARY_DEBUG("Sizeof table entry %zd\n",
                   sizeof(_TOPIARY_RESULTS_TABLE_T));

    size_t nsize =
        (nentries + results->size) * sizeof(_TOPIARY_RESULTS_TABLE_T);
    _TOPIARY_DEBUG("Nsize %zd\n", nsize);
    _TOPIARY_RESULTS_TABLE_T *t = Allocate(results->table, nsize);

    if (!t) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Results table");
        return TOPIARY_ERROR_MALLOC;
    }
    int i;

    _TOPIARY_DEBUG("New Table %p\n", t);

    for (i = results->size; i < (nentries + results->size); i++) {
        _TOPIARY_RESULTS_TABLE_T *entry = t + i;

        _TOPIARY_DEBUG("Clear %lu bytes at %p\n",
                       sizeof(_TOPIARY_RESULTS_TABLE_T), entry);
        memset(entry, 0, sizeof(_TOPIARY_RESULTS_TABLE_T));
    }
    results->table = t;
    results->size += nentries;

    _TOPIARY_DEBUG("New Table size %zd\n", results->size);
    return TOPIARY_OK;
}

int
_topiary_results_init(_TOPIARY_RESULTS_T *results, size_t nentries)
{
    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    HASH_CLEAR(hh, results->hash);
    _TOPIARY_DEBUG("Table hash freed to %p\n", results->hash);

    _TOPIARY_DEBUG("Table %p nentries %zd\n", results->table, nentries);
    if (nentries > results->size) {
        int ret = allocate_results_table(results, nentries - results->size);

        if (ret) {
            return ret;
        }
    }
    results->length = 0;
    return TOPIARY_OK;
}

int
_topiary_results_hash(_TOPIARY_RESULTS_T *results)
{
    if (results) {
        HASH_CLEAR(hh, results->hash);
        _TOPIARY_RESULTS_TABLE_T *table = results->table;

        _TOPIARY_DEBUG("Table %p\n", table);
        if (table) {
            int i;

            for (i = 0; i < results->length; i++) {
                _TOPIARY_RESULTS_TABLE_T *entry = table + i;

                _TOPIARY_DEBUG("%d key %s\n", i, entry->key);
                HASH_ADD(hh, results->hash, key, entry->keylen, entry);
            }
            HASH_SORT(results->hash, rowcmp);
        }
    }

    return TOPIARY_OK;
}

int
_topiary_results_last(_TOPIARY_RESULTS_T *results,
                      _TOPIARY_RESULTS_TABLE_T **entry)
{
    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!entry) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results entry");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (*entry) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Results entry contents not null");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!results->table) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results table");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *entry = results->table + results->length - 1;
    return TOPIARY_OK;
}

int
_topiary_results_fetch(_TOPIARY_RESULTS_T *results, char *key,
                       _TOPIARY_RESULTS_TABLE_T **entry)
{
    _TOPIARY_DEBUG("Results %p\n", results);

    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!key) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results key");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!entry) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results entry");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (*entry) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG,
                             "Results entry contents not null");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!results->table) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results table");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    _TOPIARY_DEBUG("Table %p\n", results->table);

    if (results->hash) {
        _TOPIARY_RESULTS_TABLE_T *tentry;

        HASH_FIND_STR(results->hash, key, tentry);
        if (!tentry) {
            return TOPIARY_ERROR_NOT_FOUND;
        }
        *entry = tentry;
#ifdef DEBUG
        char buf[64];
        char *pbuf;

        _topiary_results_table_valuestr(tentry, buf, sizeof buf, &pbuf);
        _TOPIARY_DEBUG("Value %s\n", pbuf);
#endif
        return TOPIARY_OK;
    }
    else {
        int i;
        _TOPIARY_RESULTS_TABLE_T *table = results->table;

        _TOPIARY_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            _TOPIARY_RESULTS_TABLE_T *tentry = table + i;

            _TOPIARY_DEBUG("%d key %s\n", i, tentry->key);
            if (!strcmp(tentry->key, key)) {
                *entry = tentry;
#ifdef DEBUG
                char buf[64];
                char *pbuf;

                _topiary_results_table_valuestr(tentry, buf, sizeof buf, &pbuf);

                _TOPIARY_DEBUG("Value %s\n", pbuf);
#endif
                return TOPIARY_OK;
            }
        }
    }
    return TOPIARY_ERROR_NOT_FOUND;
}

static int
prepare_entry(_TOPIARY_RESULTS_T *results, const char *key,
              _TOPIARY_RESULTS_TABLE_T **tentry)
{
    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!key) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results key");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (!results->table) {
        results->length = 0;
        results->size = 0;
        HASH_CLEAR(hh, results->hash);
        _TOPIARY_DEBUG("Table hash freed to %p\n", results->hash);
    }
    _TOPIARY_DEBUG("Table %p\n", results->table);

    if (results->length >= results->size) {
        int ret = allocate_results_table(results, 1);

        if (ret) {
            return ret;
        }
    }
    _TOPIARY_DEBUG("New Table %p\n", results->table);

    results->length++;

    _TOPIARY_RESULTS_TABLE_T *entry = results->table + results->length - 1;

    _TOPIARY_DEBUG("Entry %p(%zd) Type %d\n", entry, results->length - 1,
                   entry->valuetype);
    if (entry->valuetype == _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF) {
        DESTROY(entry->value.sptr);
    }

#ifdef DEBUG
    if (strlen(key) >= sizeof(entry->key)) {
        _TOPIARY_DEBUG("WARNING:%s:Key length %lu > table key length %zd\n",
                       results->tag, strlen(key), sizeof(entry->key) - 1);
    }
#endif

    entry->keylen = strlcpy(entry->key, key, sizeof(entry->key));
    *tentry = entry;
    return TOPIARY_OK;
}

int
_topiary_results_add_float(_TOPIARY_RESULTS_T *results, const char *key,
                           const float value)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_FLOAT;
    tentry->value.f = value;
    return TOPIARY_OK;
}

int
_topiary_results_add_fixed(_TOPIARY_RESULTS_T *results, const char *key,
                           const float value, const int width,
                           const int precision)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->width = width;
    tentry->precision = precision;
    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_FIXED;
    tentry->value.f = value;
    return TOPIARY_OK;
}

int
_topiary_results_add_string(_TOPIARY_RESULTS_T *results, const char *key,
                            const char *value, size_t valuelen)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    if (valuelen < 1)
        valuelen = strlen(value);

    if (valuelen >= _TOPIARY_RESULTS_TABLE_VALLEN) {
        tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREF;
        tentry->value.sptr = strdup(value);
        _TOPIARY_DEBUG("%s:Allocate %p at entry %zd\n", results->tag,
                       tentry->value.sptr, results->length - 1);
        if (!tentry->value.sptr) {
            _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC,
                                 "Results add string reference");
            return TOPIARY_ERROR_MALLOC;
        }
    }
    else {
        tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_STR;
        strlcpy(tentry->value.s, value, sizeof tentry->value.s);
    }
    return TOPIARY_OK;
}

int
_topiary_results_add_stringref(_TOPIARY_RESULTS_T *results, const char *key,
                               const char *value)
{
    if (!value) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results value");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_STRREFS;
    tentry->value.sptr = (char *) value;
    return TOPIARY_OK;
}

int
_topiary_results_add_ptr(_TOPIARY_RESULTS_T *results, const char *key,
                         const void *value)
{
    if (!value) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results value");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_PTR;
    tentry->value.ptr = (void *) value;
    return TOPIARY_OK;
}

int
_topiary_results_add_int(_TOPIARY_RESULTS_T *results, const char *key,
                         const int value)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_INT;
    tentry->value.i = value;
    return TOPIARY_OK;
}

int
_topiary_results_add_long(_TOPIARY_RESULTS_T *results, const char *key,
                          const long value)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_LONG;
    tentry->value.l = value;
    return TOPIARY_OK;
}

int
_topiary_results_add_unsigned_int(_TOPIARY_RESULTS_T *results,
                                  const char *key, const unsigned int value)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_INT;
    tentry->value.ui = value;
    return TOPIARY_OK;
}

int
_topiary_results_add_unsigned_long(_TOPIARY_RESULTS_T *results,
                                   const char *key, const unsigned long value)
{
    _TOPIARY_RESULTS_TABLE_T *tentry;
    int ret = prepare_entry(results, key, &tentry);

    if (ret) {
        return ret;
    }

    tentry->valuetype = _TOPIARY_RESULTS_TABLE_VALUETYPE_UNSIGNED_LONG;
    tentry->value.ul = value;
    return TOPIARY_OK;
}

int
_topiary_results_iterate(_TOPIARY_RESULTS_T *results,
                         _TOPIARY_RESULTS_ITERATE_FUNC func, void *data)
{
    if (!results) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Results");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    _TOPIARY_DEBUG("Results %p\n", results);
    if (results->table) {
        int i;
        _TOPIARY_RESULTS_TABLE_T *table = results->table;

        _TOPIARY_DEBUG("Table %p\n", table);

        for (i = 0; i < results->length; i++) {
            func(results, table + i, data);
        }
    }
    return TOPIARY_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
