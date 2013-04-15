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

#include "error.h"
#include "limits.h"
#include "results.h"
#include "array_private.h"
#include "base_private.h"

const char *
lnxproc_base_filename(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    const char *filename = NULL;

    if (base) {
        filename = base->filename;
    }

    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    return filename;
}

char *
lnxproc_base_lines(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    char *lines = NULL;

    if (base) {
        lines = base->lines;
    }

    LNXPROC_DEBUG("lines %1$p '%1$s'\n", lines);
    return lines;
}

int
lnxproc_base_nbytes(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    int nbytes = 0;

    if (base) {
        nbytes = base->nbytes;
        LNXPROC_DEBUG("nbytes %d\n", nbytes);
        return nbytes;
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return -1;
}

LNXPROC_ARRAY_T *
lnxproc_base_array(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    LNXPROC_ARRAY_T *array = NULL;

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return array;
    }
    array = base->array;
    LNXPROC_DEBUG("Array %p\n", array);
    return array;
}

int
lnxproc_base_print(LNXPROC_BASE_T *base, int allocated, void *data)
{
    LNXPROC_DEBUG("Base %p Data %p\n", base, data);

    if (base) {
        printf("Rawread %p\n", base->rawread);
        printf("Normalize %p\n", base->normalize);
        printf("Read %p\n", base->read);
        printf("Callback %p\n", base->callback);
        printf("Filename %s\n", base->filename);
        printf("Lines %p\n", base->lines);
        printf("Buflen %zd\n", base->buflen);
        printf("Nbytes %d\n", base->nbytes);
        lnxproc_results_print(base->results);
        return lnxproc_array_print(base->array, allocated, data);
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return LNXPROC_ERROR_BASE_NULL;
}

LNXPROC_ERROR_CALLBACK
lnxproc_base_callback(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    LNXPROC_ERROR_CALLBACK callback = NULL;

    if (base) {
        callback = base->callback;
    }

    LNXPROC_DEBUG("callback %p\n", callback);
    return callback;
}

LNXPROC_RESULTS_T *
lnxproc_base_rawread(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return NULL;
    }

    lnxproc_results_timeval(base->results);
    if (base->rawread) {
        LNXPROC_DEBUG("Execute specified rawread method %p\n", base->rawread);
        return base->rawread(base);
    }

    else {
        LNXPROC_DEBUG("Execute default rawread method\n");
        LNXPROC_DEBUG("Open %s\n", base->filename);
        int fd = open(base->filename, O_RDONLY);

        if (fd < 0) {
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            LNXPROC_ERROR_DEBUG(-errno, "Open %s\n", base->filename);
            return NULL;
        }

        LNXPROC_DEBUG("Read %s\n", base->filename);
        base->nbytes = read(fd, base->lines, base->buflen);

        if (base->nbytes < 0) {
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            LNXPROC_ERROR_DEBUG(-errno, "Open %s\n", base->filename);
            return NULL;
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
    return base->results;

}

LNXPROC_RESULTS_T *
lnxproc_base_normalize(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return NULL;
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
    LNXPROC_LIMITS_T *limits = base->array->limits;
    int dim = base->array->dim;
    char *lines = base->lines;
    int nbytes = base->nbytes;

    LNXPROC_DEBUG("Lines %p Nbytes %d\n", base, nbytes);

#ifdef DEBUG
    char buf[128];

    lnxproc_chars_print(lines, nbytes, buf, sizeof buf);
    LNXPROC_DEBUG("Chars %s\n", buf);
#endif

    if (nbytes > 0) {
        char *c = lines;
        char *d = c + nbytes;

        if (dim > 0) {
            char *saveptr = c;

/*
 * use malloced array when testing with valgrind
 * For some raeson valgrind reports use of an uninitialised variable if using
 * stack allocated arrays here
 */

            size_t *idx = calloc(dim, sizeof(size_t));

//            size_t idx[dim];
//            memset(idx, 0, dim * sizeof(int));

            while (c < d) {
                size_t i;
                int increment = 1;

                for (i = 0; i < dim; i++) {
                    if (lnxproc_limit_chr(limits + i, *c)) {
                        *c = '\0';

                        int ret =
                            lnxproc_array_set_last(array, idx, dim, saveptr);

                        if (ret) {
                            free(idx);
                            return ret;
                        }

                        while ((++c < d) && lnxproc_limit_chr(limits + i, *c));

                        saveptr = c;
                        idx[i]++;
                        int j = i + 1;

                        if (dim > j) {
                            memset(idx + j, 0, (dim - j) * sizeof(int));
                        }

                        increment = 0;
                    }
                }
                if (increment) {
                    c++;
                }
            }
            free(idx);
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
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return NULL;
    }

    if (base->read) {
        LNXPROC_DEBUG("Execute specified read method %p\n", base->read);
        return base->read(base);

    }

    else {
        LNXPROC_DEBUG("Execute default read method\n");
        LNXPROC_RESULTS_T *res = lnxproc_base_rawread(base);

        if (!res) {
            return res;
        }
        base_map(base);
        if (base->normalize) {
            LNXPROC_DEBUG("Execute default normalize method\n");
            return base->normalize(base);
        }
    }

    return base->results;
}

LNXPROC_BASE_T *
lnxproc_base_new(const char *filename,
                 LNXPROC_BASE_METHOD rawread,
                 LNXPROC_BASE_METHOD normalize,
                 LNXPROC_BASE_METHOD read,
                 LNXPROC_ERROR_CALLBACK callback,
                 size_t buflen, LNXPROC_LIMITS_T limits[], size_t dim)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    LNXPROC_DEBUG("rawread %p, normalize %p, read %p, callback %p\n", rawread,
                  normalize, read, callback);
    LNXPROC_DEBUG("buflen %zd\n", buflen);
    LNXPROC_DEBUG("limits %p dim %d\n", limits, dim);

    LNXPROC_BASE_T *base = calloc(1, sizeof(LNXPROC_BASE_T));

    if (!base) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BASE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BASE, "Malloc base\n");
        return base;
    }

    LNXPROC_DEBUG("Calloc lines buffer\n");
    base->lines = calloc(1, buflen + 1);
    if (!base->lines) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BUFFER);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BUFFER,
                            "Malloc buffer\n");
        return lnxproc_base_free(base);
    }
    base->lines[buflen] = '\n';

    base->array = lnxproc_array_new(limits, dim, callback);
    if (!base->array) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_ARRAY);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAY, "Malloc array\n");
        return lnxproc_base_free(base);
    }

    base->results = lnxproc_results_new();
    if (!base->results) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_RESULTS);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_RESULTS,
                            "Malloc results\n");
        return lnxproc_base_free(base);
    }

    base->prev = NULL;
    base->rawread = rawread;
    base->normalize = normalize;
    base->read = read;
    base->callback = callback;
    base->filename = filename;
    base->buflen = buflen;
    base->nbytes = 0;
    lnxproc_base_print(base, 1, NULL);
    LNXPROC_DEBUG("Successful\n");
    return base;
}

LNXPROC_BASE_T *
lnxproc_base_free(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (base) {
        if (base->results) {
            LNXPROC_DEBUG("Free results \n");
            base->results = lnxproc_results_free(base->results);
        }
        if (base->array) {
            LNXPROC_DEBUG("Free Array \n");
            base->array = (void *) lnxproc_array_free(base->array);
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
