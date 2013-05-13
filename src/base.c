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

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <regex.h>

#include "util_private.h"
#include "error_private.h"
#include "limits_private.h"
#include "results_private.h"
#include "array_private.h"
#include "base_private.h"
#include "limit_chr.h"

LNXPROC_ERROR_T
_lnxproc_base_print(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    if (base) {
        printf("Rawread %p\n", base->rawread);
        printf("Normalize %p\n", base->normalize);
        printf("Read %p\n", base->read);
        printf("File prefix %1$p '%1$s'\n", base->fileprefix);
        printf("File suffix %1$p '%1$s'\n", base->filesuffix);
        printf("Filenames %p\n", base->filenames);
        int i;

        for (i = 0; i < base->nfiles; i++) {
            printf("Filename %s\n", base->filenames[i]);
        }
        printf("Timestamp %lu.%06lu\n",
               (unsigned long) base->current->tv.tv_sec,
               (unsigned long) base->current->tv.tv_usec);
        printf("Rawread duration %ld usecs\n", base->current->rawread_time);
        printf("Map duration %ld usecs\n", base->current->map_time);
        printf("Normalize duration %ld usecs\n", base->current->normalize_time);
        printf("Lines %p\n", base->current->lines);
        printf("Buflen %zd\n", base->current->buflen);
        printf("Nbytes %d\n", base->current->nbytes);
        _lnxproc_array_print(base->current->array, 0);

        if (base->previous) {
            printf("Previous Timestamp %lu.%06lu\n",
                   (unsigned long) base->previous->tv.tv_sec,
                   (unsigned long) base->previous->tv.tv_usec);
            printf("Previous Rawread duration %ld usecs\n",
                   base->previous->rawread_time);
            printf("Previous Map duration %ld usecs\n",
                   base->previous->map_time);
            printf("Previous Normalize duration %ld usecs\n",
                   base->previous->normalize_time);
            printf("Previous Lines %p\n", base->previous->lines);
            printf("Previous Buflen %zd\n", base->previous->buflen);
            printf("Previous Nbytes %d\n", base->previous->nbytes);
            _lnxproc_array_print(base->previous->array, 0);
        }
        _lnxproc_results_print(base->results);
        return LNXPROC_OK;
    }

    _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
    return LNXPROC_ERROR_BASE_NULL;
}

static LNXPROC_ERROR_T
base_rawread(char *filename, char **readbuf, int *nbytes)
{
    _LNXPROC_DEBUG("Filename %s Readbuf %p Nbytes %d\n", filename, *readbuf,
                   *nbytes);

    if (*nbytes < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s\n",
                             filename);
        return LNXPROC_ERROR_BASE_READ_OVERFLOW;
    }

    _LNXPROC_DEBUG("Open %s\n", filename);

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        int myerrno = -errno;

        _LNXPROC_ERROR_DEBUG(myerrno, "Open %s\n", filename);
        return myerrno;
    }

    _LNXPROC_DEBUG("Read %s\n", filename);
    int inbytes = read(fd, *readbuf, *nbytes);

    if (inbytes < 0) {
        int myerrno = -errno;

        _LNXPROC_ERROR_DEBUG(myerrno, "Read %s\n", filename);
        return myerrno;
    }

    _LNXPROC_DEBUG("Nbytes %d read\n", inbytes);

    /* we do not have to check return status of close as we are only
     * reading the file
     */
    _LNXPROC_DEBUG("Close %s\n", filename);
    close(fd);

    (*readbuf)[inbytes] = '\n';
    *readbuf += inbytes;
    *nbytes -= inbytes;
    if (*nbytes < 1) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s\n",
                             filename);
        return LNXPROC_ERROR_BASE_READ_OVERFLOW;
    }
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
base_read_glob_files(LNXPROC_BASE_T *base, char **readbuf, int *nbytes)
{

    const char *globwild = "*";
    const char *regwild = "([[:alnum:]_.+-]+)";
    char globpat[FILENAME_MAX];
    char regpat[FILENAME_MAX];

    _LNXPROC_ARRAY_T *array = base->current->array;
    size_t dim = array->dim;
    _LNXPROC_LIMITS_T *limits = array->limits;
    char separator = limits[dim - 1].chars[0];

    if (base->fileprefix) {
        if (base->filesuffix) {
            snprintf(globpat, sizeof globpat, "%s/%s/%s", base->fileprefix,
                     globwild, base->filesuffix);
            snprintf(regpat, sizeof regpat, "%s/%s/%s", base->fileprefix,
                     regwild, base->filesuffix);
        }
        else {
            snprintf(globpat, sizeof globpat, "%s/%s", base->fileprefix,
                     globwild);
            snprintf(regpat, sizeof regpat, "%s/%s", base->fileprefix, regwild);
        }
    }
    else {
        if (base->filesuffix) {
            snprintf(globpat, sizeof globpat, "/%s/%s", globwild,
                     base->filesuffix);
            snprintf(regpat, sizeof regpat, "/%s/%s", regwild,
                     base->filesuffix);
        }
    }
    _LNXPROC_DEBUG("Glob pattern '%s'\n", globpat);
    _LNXPROC_DEBUG("Regex pattern '%s'\n", regpat);
#ifdef DEBUG
    char errbuf[128];
#endif

    glob_t globbuf;

    globbuf.gl_offs = 0;

    regex_t reg;
    regmatch_t pmatch[3];
    size_t pmlen = sizeof(pmatch) / sizeof(pmatch[0]);

    int ret = regcomp(&reg, regpat, REG_EXTENDED);

#ifdef DEBUG
    regerror(ret, &reg, errbuf, sizeof errbuf);
    _LNXPROC_DEBUG("Regex compilation returns %s\n", errbuf);
#endif
    if (ret) {
        return ret;
    }

    ret = glob(globpat, GLOB_MARK | GLOB_NOSORT, NULL, &globbuf);
    _LNXPROC_DEBUG("Glob returns %d\n", ret);

    int i, j;

    for (i = 0; i < globbuf.gl_pathc; i++) {
        _LNXPROC_DEBUG("%d:File %s\n", i, globbuf.gl_pathv[i]);
        ret = regexec(&reg, globbuf.gl_pathv[i], pmlen, pmatch, 0);
#ifdef DEBUG
        regerror(ret, &reg, errbuf, sizeof errbuf);
        _LNXPROC_DEBUG("Regex exec returns %s\n", errbuf);
#endif
        if (ret == 0) {
            j = 1;
            if (pmatch[j].rm_so > -1) {
                char *s = globbuf.gl_pathv[i] + (int) pmatch[j].rm_so;
                int len = (int) pmatch[j].rm_eo - (int) pmatch[j].rm_so;

                _LNXPROC_DEBUG("%d:%d:Match from %d to %d '%.*s'\n", i, j,
                               (int) pmatch[j].rm_so, (int) pmatch[j].rm_eo,
                               len, s);
                int n =
                    snprintf(*readbuf, *nbytes, "%.*s%c", len, s, separator);

                if (n > (*nbytes) - 1) {
                    n = (*nbytes) - 1;
                }

                *readbuf += n;
                *nbytes -= n;
                _LNXPROC_DEBUG("Readbuf %p Nbytes %d\n", *readbuf, *nbytes);
                ret = base_rawread(globbuf.gl_pathv[i], readbuf, nbytes);
                if (ret) {
                    globfree(&globbuf);
                    regfree(&reg);
                    return ret;
                }
            }
        }
    }
    globfree(&globbuf);
    regfree(&reg);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_rawread(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    _LNXPROC_DEBUG("Execute default rawread method\n");
    int i;
    char *readbuf = base->current->lines;
    int nbytes = base->current->buflen;
    LNXPROC_ERROR_T ret;

    for (i = 0; i < base->nfiles; i++) {
        ret = base_rawread(base->filenames[i], &readbuf, &nbytes);

        if (ret) {
            return ret;
        }
    }
    if (base->fileprefix || base->filesuffix) {
        ret = base_read_glob_files(base, &readbuf, &nbytes);
        if (ret) {
            return ret;
        }
    }
    base->current->nbytes = base->current->buflen - nbytes;

    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;

}

LNXPROC_ERROR_T
_lnxproc_base_normalize(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    return LNXPROC_OK;
}

static int
base_map(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    _LNXPROC_ARRAY_T *array = base->current->array;

    _LNXPROC_DEBUG("Array %p\n", array);

    char *lines = base->current->lines;
    int nbytes = base->current->nbytes;

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

            _LNXPROC_LIMITS_T *limits = base->current->array->limits;
            int dim = base->current->array->dim;

            _LNXPROC_DEBUG("Limits %p Dim %d\n", limits, dim);

            size_t idx[dim];

            memset(idx, 0, dim * sizeof(size_t));

            _LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, 0, idx[0]);
            size_t i;

            for (i = 0; i < dim && c < d; i++) {
                _LNXPROC_DEBUG("At Char %p '%c'\n", c, *c);
                while (limit_chr(limits + i, *c) && (++c < d));
            }

            char *saveptr = c;

            while (c < d) {
                int increment = 1;

                for (i = 0; i < dim && c < d; i++) {
                    _LNXPROC_DEBUG("Depth : %d: At Char %p '%c'\n", i, c, *c);
                    if (limit_chr(limits + i, *c)) {
                        *c = '\0';

                        _LNXPROC_DEBUG("Saveptr %1$p '%1$s'\n", saveptr);
                        int ret =
                            _lnxproc_array_set_last(array, idx, dim, saveptr);

                        if (ret) {
                            return ret;
                        }

                        while ((++c < d) && limit_chr(limits + i, *c));

                        if (c < d) {
                            idx[i]++;
                            _LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, i, idx[i]);

                            int j;

                            for (j = i + 1; j < dim; j++) {
                                idx[j] = 0;
                                while (limit_chr(limits + j, *c)
                                       && (++c < d));
                            }
                            saveptr = c;
                            _LNXPROC_DEBUG("New Saveptr %p\n", saveptr);

                        }
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

    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
base_new_rawread_buffer(LNXPROC_BASE_DATA_T * data, size_t newlen)
{
    _LNXPROC_DEBUG("Malloc lines buffer to %d bytes\n", newlen);
    char *p = realloc(data->lines, newlen + 1);

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BUFFER,
                             "Malloc buffer\n");
        return LNXPROC_ERROR_BASE_MALLOC_BUFFER;
    }
    memset(p, 0, newlen);
    p[newlen] = '\n';
    data->lines = p;
    data->buflen = newlen;
    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
base_resize_rawread_buffer(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Resize lines buffer from %d bytes\n",
                   base->current->buflen);
    return base_new_rawread_buffer(base->current, base->current->buflen * 2);
}

static LNXPROC_ERROR_T
array_new(LNXPROC_BASE_DATA_T * data, _LNXPROC_LIMITS_T limits[], size_t dim)
{
    if (limits && dim > 0) {
        LNXPROC_ERROR_T ret = _lnxproc_array_new(&data->array, limits, dim);

        if (ret) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAY,
                                 "Malloc array\n");
            return LNXPROC_ERROR_BASE_MALLOC_ARRAY;
        }
    }
    else {
        data->array = NULL;
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_read(LNXPROC_BASE_T *base)
{
    struct timeval start = lnxproc_timeval();

    _LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    _LNXPROC_DEBUG("Execute default read method\n");
    LNXPROC_ERROR_T ret;

    do {
        ret = base->rawread(base);
        if (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW) {
            base_resize_rawread_buffer(base);
        }
    } while (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW);

    base->current->tv = lnxproc_timeval();
    base->current->rawread_time =
        lnxproc_timeval_diff(&start, &base->current->tv);
#ifdef DEBUG
    char buf[32];

    _LNXPROC_DEBUG("Current timestamp %s\n",
                   lnxproc_timeval_print(&base->current->tv, buf, sizeof buf));
#endif

    if (ret) {
        _LNXPROC_ERROR_DEBUG(ret, "\n");
        return ret;
    }

    start = lnxproc_timeval();
    base_map(base);
    struct timeval end = lnxproc_timeval();

    base->current->map_time = lnxproc_timeval_diff(&start, &end);

    start = lnxproc_timeval();
    _LNXPROC_DEBUG("Execute normalize method\n");
    ret = base->normalize(base);
    end = lnxproc_timeval();
    base->current->normalize_time = lnxproc_timeval_diff(&start, &end);
    if (ret) {
        _LNXPROC_ERROR_DEBUG(ret, "\n");
        return ret;
    }

    return LNXPROC_OK;
}

static LNXPROC_ERROR_T
_base_data_new(LNXPROC_BASE_DATA_T * data,
               size_t buflen, _LNXPROC_LIMITS_T limits[], size_t dim)
{
    data->tv = lnxproc_timeval();
#ifdef DEBUG
    char buf[32];

    _LNXPROC_DEBUG("Current timestamp %s\n",
                   lnxproc_timeval_print(&data->tv, buf, sizeof buf));
#endif

    LNXPROC_ERROR_T ret = base_new_rawread_buffer(data, buflen);

    if (ret) {
        return ret;
    }

    ret = array_new(data, limits, dim);
    if (ret) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAY, "Malloc array\n");
        return LNXPROC_ERROR_BASE_MALLOC_ARRAY;
    }

    return LNXPROC_OK;

}

LNXPROC_ERROR_T
_lnxproc_base_store_previous(LNXPROC_BASE_T *base)
{
    if (base) {
        if (!base->previous) {
            LNXPROC_ERROR_T ret = _base_data_new(base->data + 1,
                                                 base->current->buflen,
                                                 base->current->array->limits,
                                                 base->current->array->dim);

            if (ret) {
                return ret;
            }
            base->previous = base->data + 1;
        }
        LNXPROC_BASE_DATA_T *tmp = base->current;

        base->current = base->previous;
        base->previous = tmp;
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_new(LNXPROC_BASE_T **base,
                  char **filenames,
                  size_t nfiles,
                  char *fileprefix,
                  char *filesuffix,
                  LNXPROC_BASE_METHOD rawread,
                  LNXPROC_BASE_METHOD normalize,
                  LNXPROC_BASE_METHOD read,
                  size_t buflen, _LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_ERROR_T ret;

    _LNXPROC_DEBUG("nfiles %d\n", nfiles);
    _LNXPROC_DEBUG("filenames %p\n", filenames);
    _LNXPROC_DEBUG("fileprefix %1$p '%1$s'\n", fileprefix);
    _LNXPROC_DEBUG("filesuffix %1$p '%1$s'\n", filesuffix);
    _LNXPROC_DEBUG("rawread %p, normalize %p, read %p\n", rawread,
                   normalize, read);
    _LNXPROC_DEBUG("buflen %zd\n", buflen);
    _LNXPROC_DEBUG("limits %p dim %d\n", limits, dim);
    _LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    _LNXPROC_DEBUG("sizeof LNXPROC_BASE_T %d\n", sizeof(LNXPROC_BASE_T));

    if ((filenames && (nfiles < 1)) || (!filenames && (nfiles > 0))) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_BAD_FILENAME,
                             "Filenames %p Nfiles %zd\n", filenames, nfiles);
        return LNXPROC_ERROR_BASE_BAD_FILENAME;
    }
    if (!filenames && !fileprefix && !filesuffix) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_BAD_FILENAME,
                             "Filenames %p Fileprefix %p Filesuffix %p\n",
                             filenames, fileprefix, filesuffix);
        return LNXPROC_ERROR_BASE_BAD_FILENAME;
    }

    LNXPROC_BASE_T *p = calloc(1, sizeof(LNXPROC_BASE_T));

    if (!p) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BASE, "Malloc base\n");
        return LNXPROC_ERROR_BASE_MALLOC_BASE;
    }

    _LNXPROC_DEBUG("Malloc data area\n");
    ret = _base_data_new(p->data + 0, buflen, limits, dim);
    if (ret) {
        _LNXPROC_BASE_FREE(p);
        return ret;
    }

    p->current = p->data + 0;
    p->previous = NULL;

    p->nfiles = 0;
    if (filenames) {
        p->filenames = calloc(nfiles, sizeof(char *));
        if (!p->filenames) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILENAME,
                                 "Malloc filenames\n");
            _LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILENAME;
        }
        int i;

        char *testroot = getenv("LNXPROC_TESTROOT");

        for (i = 0; i < nfiles; i++) {
            if (testroot) {
                char fname[FILENAME_MAX];

                snprintf(fname, sizeof fname, "%s%s", testroot, filenames[i]);
                p->filenames[i] = strdup(fname);
            }
            else {
                p->filenames[i] = strdup(filenames[i]);
            }
            if (!p->filenames[i]) {
                _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILENAME,
                                     "Malloc filenames %d\n", i);
                _LNXPROC_BASE_FREE(p);
                return LNXPROC_ERROR_BASE_MALLOC_FILENAME;
            }
            p->nfiles++;
        }
    }
    else {
        p->filenames = NULL;
    }
    if (fileprefix) {
        p->fileprefix = strdup(fileprefix);
        if (!p->fileprefix) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILEPREFIX,
                                 "Malloc fileprefix\n");
            _LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILEPREFIX;
        }
    }
    else {
        p->fileprefix = NULL;
    }
    if (filesuffix) {
        p->filesuffix = strdup(filesuffix);
        if (!p->filesuffix) {
            _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILESUFFIX,
                                 "Malloc filesuffix\n");
            _LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILESUFFIX;
        }
    }
    else {
        p->filesuffix = NULL;
    }
    p->results = NULL;
    ret = _lnxproc_results_new(&p->results);
    if (ret) {
        _LNXPROC_ERROR_DEBUG(ret, "Malloc results\n");
        _LNXPROC_BASE_FREE(p);
        return ret;
    }
    if (!normalize) {
        p->normalize = _lnxproc_base_normalize;
    }
    else {
        p->normalize = normalize;
    }
    if (!read) {
        p->read = _lnxproc_base_read;
    }
    else {
        p->read = read;
    }
    if (!rawread) {
        p->rawread = _lnxproc_base_rawread;
    }
    else {
        p->rawread = rawread;
    }
    *base = p;
    _LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

static void
base_data_free(LNXPROC_BASE_DATA_T * data)
{
    if (data) {
        if (data->array) {
            _LNXPROC_DEBUG("Free array\n");
            _LNXPROC_ARRAY_FREE(data->array);
        }
        if (data->lines) {
            _LNXPROC_DEBUG("Free Base buffer\n");
            free(data->lines);
            data->lines = NULL;
        }
    }
}

LNXPROC_BASE_T *
_lnxproc_base_free(LNXPROC_BASE_T *base)
{
    _LNXPROC_DEBUG("Base %p\n", base);

    if (base) {
        base_data_free(base->current);
        base_data_free(base->previous);
        if (base->filenames) {
            _LNXPROC_DEBUG("Free Base filenames\n");
            int i;

            for (i = 0; i < base->nfiles; i++) {
                if (base->filenames[i])
                    free(base->filenames[i]);
            }
            free(base->filenames);
            base->filenames = NULL;
        }
        if (base->fileprefix) {
            _LNXPROC_DEBUG("Free Base file prefix\n");
            free(base->fileprefix);
            base->fileprefix = NULL;
        }
        if (base->filesuffix) {
            _LNXPROC_DEBUG("Free Base file suffix\n");
            free(base->filesuffix);
            base->filesuffix = NULL;
        }
        if (base->results) {
            _LNXPROC_DEBUG("Free results \n");
            _LNXPROC_RESULTS_FREE(base->results);
        }

        _LNXPROC_DEBUG("Free Base\n");
        free(base);
        base = NULL;
    }

    return base;
}

static LNXPROC_ERROR_T
base_null_method(LNXPROC_BASE_T *base)
{
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_memoize(LNXPROC_BASE_T *base)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (!base->memoize_read) {
        base->memoize_read = base->read;
        base->read = base_null_method;
    }
    if (!base->memoize_rawread) {
        base->memoize_rawread = base->rawread;
        base->rawread = base_null_method;
    }
    if (!base->memoize_normalize) {
        base->memoize_normalize = base->normalize;
        base->normalize = base_null_method;
    }
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_unmemoize(LNXPROC_BASE_T *base)
{
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
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

#ifdef LNXPROC_UNUSED
static LNXPROC_ERROR_T
base_variable_rate(LNXPROC_BASE_T *base,
                   size_t idx[], size_t dim, long tdiff, float scale, char *buf,
                   size_t len)
{

    int diff;

    LNXPROC_ERROR_T ret =
        _lnxproc_array_diff(base->previous->array, base->current->array, idx,
                            dim,
                            &diff);

    if (ret) {
        return ret;
    }
    float rate = (scale * diff) / (tdiff * 1.e-6);

    snprintf(buf, len, "%f", rate);

    return LNXPROC_OK;
}

LNXPROC_ERROR_T
_lnxproc_base_variable_rate(LNXPROC_BASE_T *base,
                            size_t idx[], size_t dim, long tdiff, float scale,
                            char *buf, size_t len)
{
    *buf = '\0';
    snprintf(buf, len, "0");

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (!base->previous) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_PREVIOUS_NULL, "\n");
        return LNXPROC_ERROR_BASE_PREVIOUS_NULL;
    }

    if (!base->current->array || !base->previous->array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_BASE_ARRAY_NULL;
    }

    return base_variable_rate(base, idx, dim, tdiff, scale, buf, len);
}
#endif

LNXPROC_ERROR_T
_lnxproc_base_timeval_diff(LNXPROC_BASE_T *base, float *tdiff)
{
    *tdiff = 0.0;
    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (!base->previous) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_PREVIOUS_NULL, "\n");
        return LNXPROC_ERROR_BASE_PREVIOUS_NULL;
    }

    if (!base->current->array || !base->previous->array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_BASE_ARRAY_NULL;
    }

    *tdiff =
        1.e-6 * lnxproc_timeval_diff(&base->previous->tv, &base->current->tv);
    return LNXPROC_OK;
}

#ifdef LNXPROC_UNUSED
LNXPROC_ERROR_T
_lnxproc_base_variable_sample_rate(LNXPROC_BASE_T *base,
                                   size_t idx[], size_t dim, float scale,
                                   char *buf, size_t len)
{
    *buf = '\0';
    snprintf(buf, len, "0");

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (!base->previous) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_PREVIOUS_NULL, "\n");
        return LNXPROC_ERROR_BASE_PREVIOUS_NULL;
    }

    if (!base->current->array || !base->previous->array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_BASE_ARRAY_NULL;
    }

    long tdiff = lnxproc_timeval_diff(&base->previous->tv, &base->current->tv);

    return base_variable_rate(base, idx, dim, tdiff, scale, buf, len);
}

LNXPROC_ERROR_T
_lnxproc_base_variable_usage(LNXPROC_BASE_T *base,
                             size_t idx[], size_t dim, float scale, char *buf,
                             size_t len)
{
    *buf = '\0';
    snprintf(buf, len, "0");

    if (!base) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (!base->previous) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_PREVIOUS_NULL, "\n");
        return LNXPROC_ERROR_BASE_PREVIOUS_NULL;
    }

    if (!base->current->array || !base->previous->array) {
        _LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_ARRAY_NULL, "\n");
        return LNXPROC_ERROR_BASE_ARRAY_NULL;
    }

    int diff;

    LNXPROC_ERROR_T ret =
        _lnxproc_array_diff(base->previous->array, base->current->array, idx,
                            dim,
                            &diff);

    if (ret) {
        return ret;
    }

    long tdiff = lnxproc_timeval_diff(&base->previous->tv, &base->current->tv);

    float percent = (100.0 * scale * diff) / (tdiff * 1.e-6);

    snprintf(buf, len, "%f", percent);
    return LNXPROC_OK;
}
#endif

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
