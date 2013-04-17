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

#include <lnxproc/error.h>
#include <lnxproc/limits.h>
#include <lnxproc/results.h>
#include "array_private.h"
#include "base_private.h"

LNXPROC_ERROR_T
lnxproc_base_filename(LNXPROC_BASE_T *base, const char **filename)
{
    LNXPROC_DEBUG("Base %p\n", base);
    if (!base) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
        return LNXPROC_ERROR_BASE_NULL;
    }
    *filename = base->filename;

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
        printf("Filename %s\n", base->filename);
        printf("Lines %p\n", base->lines);
        printf("Buflen %zd\n", base->buflen);
        printf("Nbytes %d\n", base->nbytes);
        lnxproc_results_print(base->results);
        return lnxproc_array_print(base->array, 0);
    }

    LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_NULL, "\n");
    return LNXPROC_ERROR_BASE_NULL;
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
        LNXPROC_DEBUG("Open %s\n", base->filename);
        int fd = open(base->filename, O_RDONLY);

        if (fd < 0) {
            int myerrno = -errno;

            LNXPROC_ERROR_DEBUG(myerrno, "Open %s\n", base->filename);
            return myerrno;
        }

        LNXPROC_DEBUG("Read %s\n", base->filename);
        base->nbytes = read(fd, base->lines, base->buflen);

        if (base->nbytes < 0) {
            int myerrno = -errno;

            LNXPROC_ERROR_DEBUG(myerrno, "Read %s\n", base->filename);
            return myerrno;
        }

        LNXPROC_DEBUG("Nbytes %d read\n", base->nbytes);
        base->lines[base->nbytes] = '\n';

        /* we do not have to check return status of close as we are only
         * reading the file
         */
        LNXPROC_DEBUG("Close %s\n", base->filename);
        close(fd);
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
    LNXPROC_LIMITS_T *limits = base->array->limits;

    LNXPROC_DEBUG("Limits %p\n", limits);
    int dim = base->array->dim;
    char *lines = base->lines;
    int nbytes = base->nbytes;

    LNXPROC_DEBUG("Dim %d Lines %p Nbytes %d\n", dim, lines, nbytes);

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

        if (dim > 0) {
            char *saveptr = c;

            size_t idx[dim];

            memset(idx, 0, dim * sizeof(size_t));

            LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, 0, idx[0]);
            while (c < d) {
                size_t i;
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
                            saveptr = c;
                            LNXPROC_DEBUG("New Saveptr %p\n", saveptr);
                            idx[i]++;
                            LNXPROC_DEBUG("%p:Idx[%d] = %d\n", idx, i, idx[i]);
                            int j = i + 1;

                            if (dim > j) {
                                memset(idx + j, 0, (dim - j) * sizeof(size_t));
                            }

                        }
                        increment = 0;
                    }
                }
                if (increment) {
                    c++;
                }
            }
        }

/*
        else {

            base->data = c;
            while (c < d) {
                if (strchr("\n", *c)) {
                    *c = '\0';
                    break;
                }
                c++;
            }
        }
*/
    }

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
        LNXPROC_ERROR_T ret = lnxproc_base_rawread(base);

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
                 const char *filename,
                 LNXPROC_BASE_METHOD rawread,
                 LNXPROC_BASE_METHOD normalize,
                 LNXPROC_READ_METHOD read,
                 size_t buflen, LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    LNXPROC_DEBUG("rawread %p, normalize %p, read %p\n", rawread,
                  normalize, read);
    LNXPROC_DEBUG("buflen %zd\n", buflen);
    LNXPROC_DEBUG("limits %p dim %d\n", limits, dim);
    LNXPROC_DEBUG("sizeof ptr %d\n", sizeof(void *));
    LNXPROC_DEBUG("sizeof LNXPROC_BASE_T %d\n", sizeof(LNXPROC_BASE_T));

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

    ret = lnxproc_array_new(&p->array, limits, dim);
    if (ret) {
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAY, "Malloc array\n");
        LNXPROC_BASE_FREE(p);
        return LNXPROC_ERROR_BASE_MALLOC_ARRAY;
    }

//    ret = lnxproc_results_new(&p->results);
//    if (ret) {
//        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_RESULTS,
//                            "Malloc results\n");
//        LNXPROC_BASE_FREE(p);
//        return LNXPROC_ERROR_BASE_MALLOC_RESULTS;
//    }

    p->results = NULL;
    p->prev = NULL;
    p->rawread = rawread;
    p->normalize = normalize;
    p->read = read;
    p->filename = filename;
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

        LNXPROC_DEBUG("Free Base\n");
        free(base);
        base = NULL;
    }

    return base;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
