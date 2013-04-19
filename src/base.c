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
#include <sys/types.h>
#include <regex.h>

#include <lnxproc/error.h>
#include <lnxproc/limits.h>
#include <lnxproc/results.h>
#include "array_private.h"
#include "base_private.h"

LNXPROC_ERROR_T
lnxproc_base_filename(LNXPROC_BASE_T *base, char **filename)
{
    LNXPROC_DEBUG("Base %p\n", base);
    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }
    if (base->nfiles > 0) {
        *filename = base->filenames[0];
    }
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", *filename);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_base_lines(LNXPROC_BASE_T *base, char **lines)
{
    LNXPROC_DEBUG("Base %p\n", base);
    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }
    *lines = base->lines;

    LNXPROC_DEBUG("lines %1$p '%1$s'\n", *lines);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_base_nbytes(LNXPROC_BASE_T *base, int *nbytes)
{
    LNXPROC_DEBUG("Base %p\n", base);
    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }
    *nbytes = base->nbytes;
    LNXPROC_DEBUG("nbytes %d\n", *nbytes);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_base_array(LNXPROC_BASE_T *base, LNXPROC_ARRAY_T **array)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }
    *array = base->array;
    LNXPROC_DEBUG("Array %p\n", *array);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_base_print(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

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
        printf("Lines %p\n", base->lines);
        printf("Buflen %zd\n", base->buflen);
        printf("Nbytes %d\n", base->nbytes);
        lnxproc_results_print(base->results);
        return lnxproc_array_print(base->array, 0);
    }

    LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
    return LNXPROC_ERROR_BASE_NULL;
}

static LNXPROC_ERROR_T
base_rawread(char *filename, char **readbuf, int *nbytes)
{
    LNXPROC_DEBUG("Filename %s Readbuf %p Nbytes %d\n", filename, *readbuf,
                  *nbytes);

    if (*nbytes < 1) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s\n",
                            filename);
        return LNXPROC_ERROR_BASE_READ_OVERFLOW;
    }

    LNXPROC_DEBUG("Open %s\n", filename);
    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        int myerrno = -errno;

        LNXPROC_ERROR_DEBUG(myerrno, "Open %s\n", filename);
        return myerrno;
    }

    LNXPROC_DEBUG("Read %s\n", filename);
    int inbytes = read(fd, *readbuf, *nbytes);

    if (inbytes < 0) {
        int myerrno = -errno;

        LNXPROC_ERROR_DEBUG(myerrno, "Read %s\n", filename);
        return myerrno;
    }

    LNXPROC_DEBUG("Nbytes %d read\n", inbytes);

    /* we do not have to check return status of close as we are only
     * reading the file
     */
    LNXPROC_DEBUG("Close %s\n", filename);
    close(fd);

    (*readbuf)[inbytes] = '\n';
    *readbuf += inbytes;
    *nbytes -= inbytes;
    if (*nbytes < 1) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_READ_OVERFLOW, "Raw Read %s\n",
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
    char globpat[128];
    char regpat[128];

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
    LNXPROC_DEBUG("Glob pattern '%s'\n", globpat);
    LNXPROC_DEBUG("Regex pattern '%s'\n", regpat);
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
    LNXPROC_DEBUG("Regex compilation returns %s\n", errbuf);
#endif
    if (ret) {
        return ret;
    }

    ret = glob(globpat, GLOB_MARK | GLOB_NOSORT, NULL, &globbuf);
    LNXPROC_DEBUG("Glob returns %d\n", ret);

    int i, j;

    for (i = 0; i < globbuf.gl_pathc; i++) {
        LNXPROC_DEBUG("%d:File %s\n", i, globbuf.gl_pathv[i]);
        ret = regexec(&reg, globbuf.gl_pathv[i], pmlen, pmatch, 0);
#ifdef DEBUG
        regerror(ret, &reg, errbuf, sizeof errbuf);
        LNXPROC_DEBUG("Regex exec returns %s\n", errbuf);
#endif
        if (ret == 0) {
//            for (j = 0; j < pmlen; j++) {
            j = 1;
            if (pmatch[j].rm_so > -1) {
                char *s = globbuf.gl_pathv[i] + (int) pmatch[j].rm_so;
                int len = (int) pmatch[j].rm_eo - (int) pmatch[j].rm_so;

                LNXPROC_DEBUG("%d:%d:Match from %d to %d '%.*s'\n", i, j,
                              (int) pmatch[j].rm_so, (int) pmatch[j].rm_eo,
                              len, s);
                int n = snprintf(*readbuf, *nbytes, "%.*s\t", len, s);

                *readbuf += n;
                *nbytes -= n;
                LNXPROC_DEBUG("Readbuf %p Nbytes %d\n", *readbuf, *nbytes);
                ret = base_rawread(globbuf.gl_pathv[i], readbuf, nbytes);
                if (ret) {
                    globfree(&globbuf);
                    regfree(&reg);
                    return ret;
                }
            }
//           }
        }
    }
    globfree(&globbuf);
    regfree(&reg);
    return LNXPROC_OK;
}

LNXPROC_ERROR_T
lnxproc_base_rawread(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    LNXPROC_RESULTS_FREE(base->results);
    int ret = lnxproc_results_new(&base->results);

    if (ret) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_RESULTS,
                            "Malloc results\n");
        return LNXPROC_ERROR_BASE_MALLOC_RESULTS;
    }

    if (base->rawread) {
        LNXPROC_DEBUG("Execute specified rawread method %p\n", base->rawread);
        return base->rawread(base);
    }

    else {
        LNXPROC_DEBUG("Execute default rawread method\n");
        int i;
        char *readbuf = base->lines;
        int nbytes = base->buflen;

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
        base->nbytes = base->buflen - nbytes;
    }

    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;

}

LNXPROC_ERROR_T
lnxproc_base_normalize(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (base->normalize) {
        LNXPROC_DEBUG("Execute specified normalize method %p\n",
                      base->normalize);
        return base->normalize(base);
    }

    return LNXPROC_OK;
}

static int
base_map(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    LNXPROC_ARRAY_T *array = base->array;

    LNXPROC_DEBUG("Array %p\n", array);

    char *lines = base->lines;
    int nbytes = base->nbytes;

    LNXPROC_DEBUG("Lines %p Nbytes %d\n", lines, nbytes);

#ifdef DEBUG
    char basebuf[128];

    lnxproc_chars_print(lines, nbytes, basebuf, sizeof basebuf);
    LNXPROC_DEBUG("Chars %s\n", basebuf);
#endif

    if (nbytes > 0) {
        char *c = lines;

        LNXPROC_DEBUG("Start at Chars %p\n", c);
        char *d = c + nbytes;

        LNXPROC_DEBUG("End at Chars %p\n", d);

        if (array) {

            LNXPROC_LIMITS_T *limits = base->array->limits;
            int dim = base->array->dim;

            LNXPROC_DEBUG("Limits %p Dim %d\n", limits, dim);

            size_t idx[dim];

            memset(idx, 0, dim * sizeof(size_t));

            LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, 0, idx[0]);
            size_t i;

            for (i = 0; i < dim && c < d; i++) {
                LNXPROC_DEBUG("At Char %p '%c'\n", c, *c);
                while (lnxproc_limit_chr(limits + i, *c) && (++c < d));
            }

            char *saveptr = c;

            while (c < d) {
                int increment = 1;

                for (i = 0; i < dim && c < d; i++) {
                    LNXPROC_DEBUG("Depth : %d: At Char %p '%c'\n", i, c, *c);
                    if (lnxproc_limit_chr(limits + i, *c)) {
                        *c = '\0';

                        LNXPROC_DEBUG("Saveptr %1$p '%1$s'\n", saveptr);
                        int ret =
                            lnxproc_array_set_last(array, idx, dim, saveptr);

                        if (ret) {
                            return ret;
                        }

                        while ((++c < d) && lnxproc_limit_chr(limits + i, *c));

                        if (c < d) {
                            idx[i]++;
                            LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, i, idx[i]);

                            int j;

                            for (j = i + 1; j < dim; j++) {
                                idx[j] = 0;
                                while (lnxproc_limit_chr(limits + j, *c)
                                       && (++c < d));
                            }
                            saveptr = c;
                            LNXPROC_DEBUG("New Saveptr %p\n", saveptr);

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
base_resize_rawread_buffer(LNXPROC_BASE_T *base)
{

    LNXPROC_DEBUG("Resize lines buffer from %d bytes\n", base->buflen);
    size_t newlen = base->buflen * 2;
    char *p = realloc(base->lines, newlen + 1);

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BUFFER,
                            "Malloc buffer\n");
        return LNXPROC_ERROR_BASE_MALLOC_BUFFER;
    }
    memset(p, 0, newlen);
    p[newlen] = '\n';
    base->lines = p;
    base->buflen = newlen;
    LNXPROC_DEBUG("Resize lines buffer to %d bytes\n", base->buflen);
    return LNXPROC_OK;
}

LNXPROC_RESULTS_T *
lnxproc_base_read(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return NULL;
    }

    if (base->read) {
        LNXPROC_DEBUG("Execute specified read method %p\n", base->read);
        return base->read(base);

    }

    else {
        LNXPROC_DEBUG("Execute default read method\n");
        LNXPROC_ERROR_T ret;

        do {
            ret = lnxproc_base_rawread(base);
            if (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW) {
                base_resize_rawread_buffer(base);
            }
        } while (ret == LNXPROC_ERROR_BASE_READ_OVERFLOW);

        if (ret) {
            LNXPROC_ERROR_DEBUG(ret, "\n");
            return NULL;
        }
        base_map(base);
        if (base->normalize) {
            LNXPROC_DEBUG("Execute default normalize method\n");
            ret = base->normalize(base);
            if (ret) {
                LNXPROC_ERROR_DEBUG(ret, "\n");
                return NULL;
            }
        }
    }

    LNXPROC_RESULTS_T *res = base->results;

    base->results = NULL;
    return res;
}

LNXPROC_ERROR_T
lnxproc_base_new(LNXPROC_BASE_T **base,
                 char **filenames,
                 size_t nfiles,
                 char *fileprefix,
                 char *filesuffix,
                 LNXPROC_BASE_METHOD rawread,
                 LNXPROC_BASE_METHOD normalize,
                 LNXPROC_READ_METHOD read,
                 size_t buflen, LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_DEBUG("nfiles %d\n", nfiles);
    LNXPROC_DEBUG("filenames %p\n", filenames);
    LNXPROC_DEBUG("fileprefix %1$p '%1$s'\n", fileprefix);
    LNXPROC_DEBUG("filesuffix %1$p '%1$s'\n", filesuffix);
    LNXPROC_DEBUG("rawread %p, normalize %p, read %p\n", rawread,
                  normalize, read);
    LNXPROC_DEBUG("buflen %zd\n", buflen);
    LNXPROC_DEBUG("limits %p dim %d\n", limits, dim);
    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof LNXPROC_BASE_T %d\n", sizeof(LNXPROC_BASE_T));

    if ((filenames && (nfiles < 1)) || (!filenames && (nfiles > 0))) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_BAD_FILENAME,
                            "Filenames %p Nfiles %zd\n", filenames, nfiles);
        return LNXPROC_ERROR_BASE_BAD_FILENAME;
    }
    if (!filenames && !fileprefix && !filesuffix) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_BAD_FILENAME,
                            "Filenames %p Fileprefix %p Filesuffix %p\n",
                            filenames, fileprefix, filesuffix);
        return LNXPROC_ERROR_BASE_BAD_FILENAME;
    }

    LNXPROC_BASE_T *p = calloc(1, sizeof(LNXPROC_BASE_T));

    if (!p) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BASE, "Malloc base\n");
        return LNXPROC_ERROR_BASE_MALLOC_BASE;
    }

    LNXPROC_DEBUG("Calloc lines buffer\n");
    p->lines = calloc(1, buflen + 1);
    if (!p->lines) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BUFFER,
                            "Malloc buffer\n");
        LNXPROC_BASE_FREE(p);
        return LNXPROC_ERROR_BASE_MALLOC_BUFFER;
    }
    p->lines[buflen] = '\n';

    LNXPROC_ERROR_T ret;

    if (limits && dim > 0) {
        ret = lnxproc_array_new(&p->array, limits, dim);
        if (ret) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAY,
                                "Malloc array\n");
            LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_ARRAY;
        }
    }
    else {
        p->array = NULL;
    }
    p->nfiles = 0;
    if (filenames) {
        p->filenames = calloc(nfiles, sizeof(char *));
        if (!p->filenames) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILENAME,
                                "Malloc filenames\n");
            LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILENAME;
        }
        int i;

        for (i = 0; i < nfiles; i++) {
            p->filenames[i] = strdup(filenames[i]);
            if (!p->filenames[i]) {
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILENAME,
                                    "Malloc filenames %d\n", i);
                LNXPROC_BASE_FREE(p);
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
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILEPREFIX,
                                "Malloc fileprefix\n");
            LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILEPREFIX;
        }
    }
    else {
        p->fileprefix = NULL;
    }
    if (filesuffix) {
        p->filesuffix = strdup(filesuffix);
        if (!p->filesuffix) {
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_FILESUFFIX,
                                "Malloc filesuffix\n");
            LNXPROC_BASE_FREE(p);
            return LNXPROC_ERROR_BASE_MALLOC_FILESUFFIX;
        }
    }
    else {
        p->filesuffix = NULL;
    }
    p->results = NULL;
    p->prev = NULL;
    p->rawread = rawread;
    p->normalize = normalize;
    p->read = read;
    p->buflen = buflen;
    p->nbytes = 0;
    *base = p;
    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_BASE_T *
lnxproc_base_free(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (base) {
        if (base->prev) {
            LNXPROC_DEBUG("Free previous results \n");
            LNXPROC_RESULTS_FREE(base->prev);
        }
        if (base->results) {
            LNXPROC_DEBUG("Free results \n");
            LNXPROC_RESULTS_FREE(base->results);
        }
        if (base->array) {
            LNXPROC_DEBUG("Free Array \n");
            LNXPROC_ARRAY_FREE(base->array);
        }
        if (base->lines) {
            LNXPROC_DEBUG("Free Base buffer\n");
            free(base->lines);
            base->lines = NULL;
        }
        if (base->filenames) {
            LNXPROC_DEBUG("Free Base filenames\n");
            int i;

            for (i = 0; i < base->nfiles; i++) {
                if (base->filenames[i])
                    free(base->filenames[i]);
            }
            free(base->filenames);
            base->filenames = NULL;
        }
        if (base->fileprefix) {
            LNXPROC_DEBUG("Free Base file prefix\n");
            free(base->fileprefix);
            base->fileprefix = NULL;
        }
        if (base->filesuffix) {
            LNXPROC_DEBUG("Free Base file suffix\n");
            free(base->filesuffix);
            base->filesuffix = NULL;
        }

        LNXPROC_DEBUG("Free Base\n");
        free(base);
        base = NULL;
    }

    return base;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
