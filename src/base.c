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

#include <errno.h>              //errno
#include <fcntl.h>              //open
#include <limits.h>             //SSIZE_MAX
#include <stdio.h>              //printf
#include <glob.h>               // glob
#include <regex.h>              // regex

#include <unistd.h>             // read,close

#include "allocate.h"
#include "memdup.h"
#include "strlcpy.h"
#include "util_private.h"
#include "print_private.h"
#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "array_private.h"
#include "base_private.h"
#include "opt_private.h"
#include "limit_chr.h"

char *
_lnxproc_base_typestr(_LNXPROC_BASE_TYPE_T type, char *buf, size_t len)
{
    static const char *typestr[_LNXPROC_BASE_TYPE_SIZE] = {
        "Base type : No attributes",
        "Base type : Previous results are stored",
        "Base type : Results are memoized",
    };

    if (type >= 0 || type < _LNXPROC_BASE_TYPE_SIZE) {
        snprintf(buf, len, "%d:%s", type, typestr[type]);
    }
    else {
        snprintf(buf, len, "%d:Unknown error", type);
    }
    return buf;
}

int
_lnxproc_base_print(_LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    if (base) {
        char buf[64];

        printf("Base %p\n", base);
        printf("Attribute %s\n",
               _lnxproc_base_typestr(base->type, buf, sizeof buf));
        printf("Rawread %p\n", base->rawread);
        printf("Normalize %p\n", base->normalize);
        printf("Read %p\n", base->read);
        printf("File prefix %1$p '%1$s'\n", base->fileprefix);
        printf("File glob %1$p '%1$s'\n", base->fileglob);
        printf("File suffix %1$p '%1$s'\n", base->filesuffix);
        printf("Filenames %p\n", base->filenames);
        int i;

        for (i = 0; i < base->nfiles; i++) {
            printf("Filename %s\n", base->filenames[i]);
        }
        printf("No. of reads %lu\n", base->count);

        _LNXPROC_BASE_DATA_T *data = base->current;

        printf("CURRENT at %p(%d)\n", data, data->id);
        printf("Rawread duration %ld usecs\n", data->rawread_time);
        printf("Map duration %ld usecs\n", data->map_time);
        printf("Normalize duration %ld usecs\n", data->normalize_time);
        printf("Lines %p\n", data->lines);
        printf("Buflen %zd\n", data->buflen);
        printf("Nbytes %d\n", data->nbytes);
        _lnxproc_array_print(data->array, 0);
        _lnxproc_results_print(data->results, STDOUT_FILENO, LNXPROC_PRINT_ALL);

        if (base->previous) {
            data = base->previous;
            printf("PREVIOUS at %p(%d)\n", data, data->id);
            printf("Previous Rawread duration %ld usecs\n", data->rawread_time);
            printf("Previous Map duration %ld usecs\n", data->map_time);
            printf("Previous Hash duration %ld usecs\n", data->hash_time);
            printf("Previous Normalize duration %ld usecs\n",
                   data->normalize_time);
            printf("Previous Lines %p\n", data->lines);
            printf("Previous Buflen %zd\n", data->buflen);
            printf("Previous Nbytes %d\n", data->nbytes);
            _lnxproc_array_print(data->array, 0);
            _lnxproc_results_print(data->results, STDOUT_FILENO,
                                   LNXPROC_PRINT_ALL);
        }
        return LNXPROC_OK;
    }

    _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
    return LNXPROC_ERROR_ILLEGAL_ARG;
}

static int
base_rawread(char *filename, char **readbuf, size_t * nbytes)
{
    _LNXPROC_DEBUG("Filename %s Readbuf %p Nbytes %zd\n", filename, *readbuf,
                   *nbytes);

    if (*nbytes < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s",
                             filename);
        return LNXPROC_ERROR_BASE_READ_OVERFLOW;
    }

    _LNXPROC_DEBUG("Open %s\n", filename);

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        int myerrno = -errno;

        _LNXPROC_ERROR_DEBUG(myerrno, "Open %s", filename);
        return myerrno;
    }

    /* we do not have to check return status of close as we are only
     * reading the file
     */
    char *mybuf = *readbuf;
    size_t mybytes = *nbytes;

    if (mybytes > SSIZE_MAX) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_SSIZE_MAX, "mybytes %zd",
                             mybytes);
        close(fd);
        return LNXPROC_ERROR_BASE_READ_SSIZE_MAX;
    }

    _LNXPROC_DEBUG("Read %s\n", filename);
    ssize_t inbytes;

    do {
        inbytes = read(fd, mybuf, mybytes);

        if (inbytes < 0) {
            int myerrno = -errno;

            _LNXPROC_ERROR_DEBUG(myerrno, "Read %s", filename);
            _LNXPROC_DEBUG("Close %s\n", filename);
            close(fd);

            return myerrno;
        }
        _LNXPROC_DEBUG("%zd bytes read\n", inbytes);
        mybuf += inbytes;
        mybytes -= inbytes;
    } while (inbytes > 0);
    close(fd);

    _LNXPROC_DEBUG("Nbytes %zd read\n", (*nbytes) - mybytes);

    if (mybytes < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s",
                             filename);
        return LNXPROC_ERROR_BASE_READ_OVERFLOW;
    }
    *mybuf = '\f';
    mybuf += 1;
    mybytes -= 1;
    *readbuf = mybuf;
    *nbytes = mybytes;
    return LNXPROC_OK;
}

static int
base_read_glob_files(_LNXPROC_BASE_T *base, char **readbuf, size_t * nbytes)
{

    const char *globwild = base->fileglob;
    const char *regwild = "([[:alnum:]_.+-]+)";
    char globpat[FILENAME_MAX];
    char regpat[FILENAME_MAX];

    _LNXPROC_BASE_DATA_T *data = base->current;
    _LNXPROC_ARRAY_T *array = data->array;
    _LNXPROC_LIMITS_T *limits = array->limits;
    size_t dim = limits->dim;
    size_t nseps = dim - 1;
    char separators[nseps];
    int i;

    for (i = nseps; i > 0; i--) {
        separators[nseps - i] = limits->row[i].chars[0];
    }
    int m = 0;
    int n = 0;

    if (base->fileprefix) {
        STRLCAT(globpat, base->fileprefix, m, sizeof(globpat));
        STRLCAT(regpat, base->fileprefix, n, sizeof(regpat));
    }

    STRLCAT(globpat, "/", m, sizeof(globpat));
    STRLCAT(globpat, globwild, m, sizeof(globpat));
    STRLCAT(regpat, "/", n, sizeof(regpat));
    STRLCAT(regpat, regwild, n, sizeof(regpat));

    if (base->filesuffix) {
        STRLCAT(globpat, "/", m, sizeof(globpat));
        STRLCAT(globpat, base->filesuffix, m, sizeof(globpat));
        STRLCAT(regpat, "/", n, sizeof(regpat));
        STRLCAT(regpat, base->filesuffix, n, sizeof(regpat));
    }
#ifdef DEBUG
    char errbuf[128];
#endif

    _LNXPROC_DEBUG("Glob pattern '%s'\n", globpat);
    glob_t globbuf;

    memset(&globbuf, 0, sizeof(globbuf));

    int ret =
        glob(globpat, GLOB_MARK | GLOB_NOSORT | GLOB_BRACE, NULL, &globbuf);
    if (ret) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_GLOB_FAILURE, " errno = %d",
                             ret);
        return LNXPROC_ERROR_BASE_GLOB_FAILURE;
    }

    _LNXPROC_DEBUG("Regex pattern '%s'\n", regpat);
    regex_t reg;

    memset(&reg, 0, sizeof(regex_t));

    regmatch_t pmatch[3];
    size_t pmlen = sizeof(pmatch) / sizeof(pmatch[0]);

    ret = regcomp(&reg, regpat, REG_EXTENDED);

    if (ret) {
#ifdef DEBUG
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_REGEX_FAILURE, "%s", regpat);
        regerror(ret, &reg, errbuf, sizeof errbuf);
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_REGEX_FAILURE, " errno = %s",
                             errbuf);
#endif
        globfree(&globbuf);
        return LNXPROC_ERROR_BASE_REGEX_FAILURE;
    }

    int j;

    for (i = 0; i < globbuf.gl_pathc; i++) {
        _LNXPROC_DEBUG("%d:File %s\n", i, globbuf.gl_pathv[i]);
        ret = regexec(&reg, globbuf.gl_pathv[i], pmlen, pmatch, 0);
        if (ret) {
#ifdef DEBUG
            regerror(ret, &reg, errbuf, sizeof errbuf);
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_REGEX_FAILURE,
                                 " errno = %s", errbuf);
#endif
            globfree(&globbuf);
            regfree(&reg);
            return LNXPROC_ERROR_BASE_REGEX_FAILURE;
        }
        j = 1;                  // $0 is the whole string, $1 is the first regex parameter
        if (pmatch[j].rm_so > -1) {
            char *s = globbuf.gl_pathv[i] + (int) pmatch[j].rm_so;
            int len = (int) pmatch[j].rm_eo - (int) pmatch[j].rm_so;

            _LNXPROC_DEBUG("%d:%d:Match from %d to %d '%.*s'\n", i, j,
                           (int) pmatch[j].rm_so, (int) pmatch[j].rm_eo, len,
                           s);
            if ((*nbytes) < len + 1) {
                globfree(&globbuf);
                regfree(&reg);
                return LNXPROC_ERROR_BASE_READ_OVERFLOW;
            }
            int n = strlcpy(*readbuf, s, len + 1);

            *readbuf += n;
            *nbytes -= n;

            if ((*nbytes) < nseps) {
                globfree(&globbuf);
                regfree(&reg);
                return LNXPROC_ERROR_BASE_READ_OVERFLOW;
            }

            memcpy(*readbuf, separators, nseps);
            *readbuf += nseps;
            *nbytes -= nseps;

            _LNXPROC_DEBUG("Readbuf %p Nbytes %zd\n", *readbuf, *nbytes);
            ret = base_rawread(globbuf.gl_pathv[i], readbuf, nbytes);
            if (ret) {
                if (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW) {
                    globfree(&globbuf);
                    regfree(&reg);
                    return ret;
                }
                *readbuf -= n + nseps;
                *nbytes += n + nseps;
            }
        }
    }
    globfree(&globbuf);
    regfree(&reg);
    return LNXPROC_OK;
}

static int
base_read_files(_LNXPROC_BASE_T *base)
{
    _LNXPROC_BASE_DATA_T *data = base->current;
    int i;
    char *readbuf = data->lines;
    size_t nbytes = data->buflen;

    int ret;

    for (i = 0; i < base->nfiles; i++) {
        ret = base_rawread(base->filenames[i], &readbuf, &nbytes);

        if (ret) {
            return ret;
        }
    }
    if (base->fileglob && (base->fileprefix || base->filesuffix)) {
        ret = base_read_glob_files(base, &readbuf, &nbytes);
        if (ret) {
            return ret;
        }
    }
    data->nbytes = data->buflen - nbytes;

    return LNXPROC_OK;
}

static int
base_new_rawread_buffer(_LNXPROC_BASE_DATA_T *data, size_t newlen)
{
    _LNXPROC_DEBUG("Allocate lines buffer to %zd bytes\n", newlen);
    char *p = Allocate(data->lines, newlen + 1);

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base buffer");
        return LNXPROC_ERROR_MALLOC;
    }
    memset(p, 0, newlen);
    p[newlen] = '\f';
    data->lines = p;
    data->buflen = newlen;
    return LNXPROC_OK;
}

static int
base_resize_rawread_buffer(_LNXPROC_BASE_DATA_T *data)
{
    _LNXPROC_DEBUG("Resize lines buffer from %zd bytes\n", data->buflen);
    return base_new_rawread_buffer(data, data->buflen * 2);
}

static int
base_rawread_default(_LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Execute default rawread method\n");

    int ret;
    _LNXPROC_BASE_DATA_T *data = base->current;

    do {
        ret = base_read_files(base);
        if (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW) {
            base_resize_rawread_buffer(data);
        }
    } while (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW);
    if (ret) {
        return ret;
    }
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

int
_lnxproc_base_rawread(_LNXPROC_BASE_T *base)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_BASE_DATA_T *data = base->current;

    if (base->rawread) {
        int ret;

        if (base->type == _LNXPROC_BASE_TYPE_PREVIOUS) {
            if (base->count > 0) {
                ret = _lnxproc_base_store_previous(base);
                if (ret) {
                    return ret;
                }
            }
        }

        struct timeval start = lnxproc_timeval();

        ret = base->rawread(base);
        if (ret) {
            return ret;
        }

        data->results->tv = lnxproc_timeval();
        data->rawread_time = lnxproc_timeval_diff(&start, &data->results->tv);
#ifdef DEBUG
        char buf[32];

        _LNXPROC_DEBUG("Current timestamp %s\n",
                       lnxproc_timeval_print(&data->results->tv, buf,
                                             sizeof buf));
#endif
    }
    else {
        data->rawread_time = 0.0;
    }
    return LNXPROC_OK;
}

static int
base_map(_LNXPROC_BASE_T *base)
{
    _LNXPROC_BASE_DATA_T *data = base->current;

    struct timeval start = lnxproc_timeval();

    _LNXPROC_DEBUG("Data %p\n", data);

    _LNXPROC_ARRAY_T *array = data->array;

    _LNXPROC_DEBUG("Array %p\n", array);

    char *lines = data->lines;
    int nbytes = data->nbytes;

    _LNXPROC_DEBUG("Lines %p Nbytes %d\n", lines, nbytes);

#ifdef DEBUG
    char basebuf[128];

    _lnxproc_chars_print(lines, nbytes, basebuf, sizeof basebuf);
    _LNXPROC_DEBUG("Chars %s\n", basebuf);
#endif

    if (nbytes > 0) {
        char *c = lines;

        _LNXPROC_DEBUG("Start at Chars %p\n", c);
        char *d = c + nbytes;

        _LNXPROC_DEBUG("End at Chars %p\n", d);

        if (array) {

            _LNXPROC_LIMITS_T *limits = data->array->limits;
            size_t dim = limits->dim;

            _LNXPROC_DEBUG("Limits %p Dim %zd\n", limits, dim);

            size_t idx[dim];

            memset(idx, 0, dim * sizeof(size_t));

            _LNXPROC_DEBUG("%p:Idx[%d] = %zd\n", idx, 0, idx[0]);
            int i, j;

            /* swallow any leading separators */
            for (i = dim - 1; i >= 0 && c < d; i--) {
                _LNXPROC_DEBUG("At Char %p '%c'\n", c, *c);
                while (c < d && limit_chr(limits->row + i, *c)) {
                    c++;
                }
            }

            char *saveptr = c;

            while (c < d) {

                int increment = 1;

                _LNXPROC_DEBUG("At Char %p '%c'\n", c, *c);
                for (i = dim - 1; (i >= 0) && (c < d); i--) {
                    if (limit_chr(limits->row + i, *c)) {
                        *c = '\0';
                        _LNXPROC_DEBUG("Saveptr %1$p '%1$s'\n", saveptr);
                        if (c > saveptr) {
#ifdef DEBUG
                            int k;

                            for (k = 0; k < dim; k++) {
                                _LNXPROC_DEBUG("Idx[%d] %zd\n", k, idx[k]);
                            }
#endif
                            int ret = _lnxproc_array_set_last(array, idx, dim,
                                                              saveptr);

                            if (ret) {
                                return ret;
                            }
                        }
                        c++;
                        /* swallow any repeated separators */
                        while ((c < d) && limit_chr(limits->row + i, *c)) {
                            c++;
                        }
                        idx[i]++;
                        _LNXPROC_DEBUG("New Idx[%d] %zd\n", i, idx[i]);
                        for (j = i + 1; j < dim; j++) {
                            idx[j] = 0;
                            while ((c < d) && limit_chr(limits->row + j, *c)) {
                                c++;
                            }
                            _LNXPROC_DEBUG("New Idx[%d] %zd\n", j, idx[j]);
                        }
                        saveptr = c;
                        _LNXPROC_DEBUG("New Saveptr %p\n", saveptr);
                        increment = 0;
                    }
                }
                if (increment) {
                    c++;
                }
            }
        }

        else {
            while (c < d) {
                if (strchr("\n", *c)) {
                    *c = '\0';
                    break;
                }
                c++;
            }
        }
    }

    struct timeval end = lnxproc_timeval();

    data->map_time = lnxproc_timeval_diff(&start, &end);
    return LNXPROC_OK;
}

static int
array_new(_LNXPROC_BASE_DATA_T *data, _LNXPROC_LIMITS_T *limits)
{
    if (limits) {
        int ret = _lnxproc_array_new(&data->array, limits);

        if (ret) {
            return ret;
        }
    }
    else {
        data->array = NULL;
    }
    return LNXPROC_OK;
}

static int
base_null_method(_LNXPROC_BASE_T *base)
{
    return LNXPROC_OK;
}

int
_lnxproc_base_normalize(_LNXPROC_BASE_T *base)
{

    _LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    int ret;

    _LNXPROC_BASE_DATA_T *data = base->current;

    if (base->normalize) {

        base_map(base);

        _LNXPROC_DEBUG("Execute normalize method\n");
        struct timeval start = lnxproc_timeval();

        ret = base->normalize(base);
        struct timeval end = lnxproc_timeval();

        data->normalize_time = lnxproc_timeval_diff(&start, &end);
        if (ret) {
            return ret;
        }
        start = end;
        _lnxproc_results_hash(data->results);
        end = lnxproc_timeval();
        data->hash_time = lnxproc_timeval_diff(&start, &end);
        if (base->type == _LNXPROC_BASE_TYPE_MEMOIZE) {
            ret = _lnxproc_base_memoize(base);
            if (ret) {
                return ret;
            }
        }
        base->count++;
        if (base->count == 0)
            base->count = 1;
    }
    else {
        data->map_time = 0.0;
        data->hash_time = 0.0;
        data->normalize_time = 0.0;
    }
    return LNXPROC_OK;
}

int
_lnxproc_base_read(_LNXPROC_BASE_T *base)
{

    _LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    int ret;

    ret = _lnxproc_base_rawread(base);
    if (ret) {
        return ret;
    }
    ret = _lnxproc_base_normalize(base);
    if (ret) {
        return ret;
    }

    return LNXPROC_OK;
}

static int
_base_data_new(_LNXPROC_BASE_DATA_T *data, int id, char *tag,
               size_t buflen, _LNXPROC_LIMITS_T *limits)
{
    _LNXPROC_DEBUG("New base data at %p index %d\n", data, id);
    data->id = id;

    int ret = base_new_rawread_buffer(data, buflen);

    if (ret) {
        return ret;
    }

    ret = array_new(data, limits);
    if (ret) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Array");
        return LNXPROC_ERROR_MALLOC;
    }

    data->results = NULL;
    ret = _lnxproc_results_new(&data->results, tag);
    if (ret) {
        return ret;
    }
    return LNXPROC_OK;

}

int
_lnxproc_base_store_previous(_LNXPROC_BASE_T *base)
{
    if (base) {
        if (!base->previous) {
            int ret = _base_data_new(base->data + 1, 1,
                                     base->current->results->tag,
                                     base->current->buflen,
                                     base->current->array->limits);

            if (ret) {
                return ret;
            }
            base->previous = base->data + 1;
        }
        _LNXPROC_BASE_DATA_T *tmp = base->current;

        base->current = base->previous;
        base->previous = tmp;
    }
    return LNXPROC_OK;
}

int
_lnxproc_base_set_optional(_LNXPROC_BASE_T *base, LNXPROC_OPT_T *optional)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if (base->optional) {
        LNXPROC_OPT_FREE(base->optional);
    }
    lnxproc_opt_new(&optional);
    base->optional = optional;
    return LNXPROC_OK;
}

int
_lnxproc_base_set_fileprefix(_LNXPROC_BASE_T *base, char *fileprefix)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    char *testroot = getenv("LNXPROC_TESTROOT");

    DESTROY(base->fileprefix);
    if (fileprefix) {
        if (testroot) {
            char fname[FILENAME_MAX];

            int n = 0;

            STRLCAT(fname, testroot, n, sizeof fname);
            STRLCAT(fname, fileprefix, n, sizeof fname);
            base->fileprefix = strdup(fname);
        }
        else {
            base->fileprefix = strdup(fileprefix);
        }
        if (!base->fileprefix) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base fileprefix");
            return LNXPROC_ERROR_MALLOC;
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_base_set_fileglob(_LNXPROC_BASE_T *base, char *fileglob)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    DESTROY(base->fileglob);
    if (fileglob) {
        base->fileglob = strdup(fileglob);
        if (!base->fileglob) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base fileglob");
            return LNXPROC_ERROR_MALLOC;
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_base_set_filesuffix(_LNXPROC_BASE_T *base, char *filesuffix)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    DESTROY(base->filesuffix);
    if (filesuffix) {
        base->filesuffix = strdup(filesuffix);
        if (!base->filesuffix) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base filesuffix");
            return LNXPROC_ERROR_MALLOC;
        }
    }

    return LNXPROC_OK;
}

int
_lnxproc_base_set_filenames(_LNXPROC_BASE_T *base,
                            char **filenames, size_t nfiles)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    if ((filenames && (nfiles < 1)) || (!filenames && (nfiles > 0))) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Filenames %p Nfiles %zd", filenames, nfiles);
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (base->filenames) {
        _LNXPROC_DEBUG("Free Base filenames\n");
        int i;

        for (i = 0; i < base->nfiles; i++) {
            DESTROY(base->filenames[i]);
        }
        DESTROY(base->filenames);
    }

    char *testroot = getenv("LNXPROC_TESTROOT");

    base->nfiles = 0;
    if (filenames) {
        base->filenames = Allocate(NULL, nfiles * sizeof(char *));
        if (!base->filenames) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base filenames");
            return LNXPROC_ERROR_MALLOC;
        }
        int i;

        for (i = 0; i < nfiles; i++) {
            if (testroot) {
                char fname[FILENAME_MAX];

                int n = 0;

                STRLCAT(fname, testroot, n, sizeof fname);
                STRLCAT(fname, filenames[i], n, sizeof fname);
                base->filenames[i] = strdup(fname);
            }
            else {
                base->filenames[i] = strdup(filenames[i]);
            }
            if (!base->filenames[i]) {
                _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC,
                                     "Base filenames %d", i);
                int j;

                for (j = i - 1; j >= 0; j--) {
                    DESTROY(base->filenames[j]);
                }
                return LNXPROC_ERROR_MALLOC;
            }
            base->nfiles++;
        }
    }
    else {
        base->filenames = NULL;
    }

    return LNXPROC_OK;
}

int
_lnxproc_base_new(_LNXPROC_BASE_T **base,
                  char *tag,
                  _LNXPROC_BASE_TYPE_T type,
                  _LNXPROC_BASE_METHOD rawread,
                  _LNXPROC_BASE_METHOD normalize,
                  _LNXPROC_BASE_METHOD read,
                  size_t buflen, _LNXPROC_LIMITS_T *limits)
{
    int ret;

    _LNXPROC_DEBUG("tag %s\n", tag);
    _LNXPROC_DEBUG("rawread %p, normalize %p, read %p\n", rawread,
                   normalize, read);
    _LNXPROC_DEBUG("buflen %zd\n", buflen);
    _LNXPROC_DEBUG("limits %p\n", limits);
    _LNXPROC_DEBUG("sizeof ptr %lu\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof LNXPROC_BASE_T %lu\n", sizeof(_LNXPROC_BASE_T));

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    _LNXPROC_BASE_T *p = Allocate(NULL, sizeof(_LNXPROC_BASE_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_MALLOC, "Base");
        return LNXPROC_ERROR_MALLOC;
    }

    _LNXPROC_DEBUG("Allocate data area\n");
    ret = _base_data_new(p->data + 0, 0, tag, buflen, limits);
    if (ret) {
        _LNXPROC_BASE_FREE(p);
        return ret;
    }

    p->current = p->data + 0;
    p->previous = NULL;
    p->normalize = normalize;
    if (!read) {
        p->read = _lnxproc_base_read;
    }
    else {
        p->read = read;
    }
    if (!rawread) {
        p->rawread = base_rawread_default;
    }
    else {
        p->rawread = rawread;
    }
    p->type = type;
    p->count = 0;
    *base = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

int
_lnxproc_base_size(_LNXPROC_BASE_T *base, size_t * size)
{
    if (!size) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Size");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    *size += sizeof(*base);
    int i;

    for (i = 0; i < 2; i++) {
        _LNXPROC_BASE_DATA_T *d = base->data + i;

        if (d->lines)
            *size += d->buflen;
        if (d->array) {
            size_t s;

            _lnxproc_array_size(d->array, &s);
            *size += s;
        }
        if (d->results) {
            size_t s;

            _lnxproc_results_size(d->results, &s);
            *size += s;
        }

    }
    for (i = 0; i < base->nfiles; i++) {
        if (base->filenames[i])
            *size += strlen(base->filenames[i]) + 1;
    }
    if (base->fileprefix)
        *size += strlen(base->fileprefix) + 1;
    if (base->fileglob)
        *size += strlen(base->fileglob) + 1;
    if (base->filesuffix)
        *size += strlen(base->filesuffix) + 1;
    return LNXPROC_OK;
}

static void
base_data_free(_LNXPROC_BASE_DATA_T *data)
{
    if (data) {
        _LNXPROC_ARRAY_FREE(data->array);
        DESTROY(data->lines);
        _LNXPROC_RESULTS_FREE(data->results);
    }
}

int
_lnxproc_base_free(_LNXPROC_BASE_T **baseptr)
{
    _LNXPROC_DEBUG("Base %p\n", baseptr);

    if (baseptr && *baseptr) {
        _LNXPROC_BASE_T *base = *baseptr;

        base_data_free(base->current);
        base_data_free(base->previous);
        _lnxproc_base_set_filenames(base, NULL, 0);
        DESTROY(base->fileprefix);
        DESTROY(base->fileglob);
        DESTROY(base->filesuffix);
        LNXPROC_OPT_FREE(base->optional);
        _LNXPROC_DEBUG("Free Base\n");
        DESTROY(base);
        *baseptr = NULL;
    }

    return LNXPROC_OK;
}

int
_lnxproc_base_memoize(_LNXPROC_BASE_T *base)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!base->memoize_read) {
        base->memoize_read = base->read;
        base->read = base_null_method;
    }
    if (!base->memoize_rawread) {
        base->memoize_rawread = base->rawread;
        base->rawread = NULL;
    }
    if (!base->memoize_normalize) {
        base->memoize_normalize = base->normalize;
        base->normalize = NULL;
    }
    return LNXPROC_OK;
}

int
_lnxproc_base_unmemoize(_LNXPROC_BASE_T *base)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (base->memoize_read) {
        base->read = base->memoize_read;
        base->memoize_read = NULL;
    }
    if (base->memoize_rawread) {
        base->rawread = base->memoize_rawread;
        base->memoize_rawread = NULL;
    }
    if (base->memoize_normalize) {
        base->normalize = base->memoize_normalize;
        base->memoize_normalize = NULL;
    }
    return LNXPROC_OK;
}

int
_lnxproc_base_timeval_diff(_LNXPROC_BASE_T *base, float *tdiff)
{
    if (!tdiff) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Time difference");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }
    *tdiff = 0.0;
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG, "Base");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!base->previous || !base->current) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Base current/previous");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    if (!base->current->array || !base->previous->array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_ILLEGAL_ARG,
                             "Base current/previous array");
        return LNXPROC_ERROR_ILLEGAL_ARG;
    }

    *tdiff =
        1.e-6 * lnxproc_timeval_diff(&base->current->results->tv,
                                     &base->previous->results->tv);
    return LNXPROC_OK;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
