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
#include "array.h"
#include "base.h"

struct lnxproc_base_t {
    LNXPROC_BASE_METHOD rawread;
    LNXPROC_BASE_METHOD normalize;
    LNXPROC_BASE_METHOD read;
    LNXPROC_ERROR_CALLBACK callback;
    const char *filename;
    char *lines;
    size_t buflen;
    int nbytes;
    LNXPROC_ARRAY_T *map;
    void *data;
};

const char *lnxproc_base_filename(LNXPROC_BASE_T *base)
{
    const char *filename = NULL;
    if (base) {
        filename = base->filename;
    }
    return filename;
}

char *lnxproc_base_lines(LNXPROC_BASE_T *base)
{
    char *lines = NULL;
    if (base) {
        lines = base->lines;
    }
    return lines;
}

int lnxproc_base_nbytes(LNXPROC_BASE_T *base)
{
    int nbytes = 0;
    if (base) {
        nbytes = base->nbytes;
        return nbytes;
    }
    return -1;
}

LNXPROC_ARRAY_T *lnxproc_base_map(LNXPROC_BASE_T *base)
{
    LNXPROC_ARRAY_T *map = NULL;
    if (base) {
        map = base->map;
    }
    return map;
}

int lnxproc_base_map_set(LNXPROC_BASE_T *base, LNXPROC_ARRAY_T *map)
{
    if (base) {
        base->map = map;
        return LNXPROC_OK;
    }
    return LNXPROC_ERROR_BASE_NULL;
}

int lnxproc_base_rawread(LNXPROC_BASE_T *base)
{

    if (base->rawread) {
        return base->rawread(base);
    }
    else {
        int fd = open(base->filename, O_RDONLY);
        if (fd < 0) {
            lnxproc_system_error(base->callback,__func__, errno);
            return -errno;
        }

        base->nbytes = read(fd, base->lines, base->buflen);
        if (base->nbytes < 0) {
            lnxproc_system_error(base->callback,__func__, errno);
            return -errno;
        }
        base->lines[base->nbytes] = '\n';

        /* we do not have to check return status of close as we are only
         * reading the file 
         */
        close(fd);
    }
    return LNXPROC_OK;

}

int lnxproc_base_normalize(LNXPROC_BASE_T *base)
{
    if (base->normalize) {
        return base->normalize(base);
    }
    return LNXPROC_OK;
}

int lnxproc_base_read(LNXPROC_BASE_T *base)
{
    if (base->read) {
        return base->read(base);
    }
    else {
        int state = lnxproc_base_rawread(base);
        if (state) {
            return state;
        }

        if (base->normalize) {
            state = base->normalize(base);
            if (state) {
                return state;
            }
        }
    }

    return LNXPROC_OK;
}

LNXPROC_BASE_T *lnxproc_base_init(const char *filename,
                                         LNXPROC_BASE_METHOD rawread,
                                         LNXPROC_BASE_METHOD normalize,
                                         LNXPROC_BASE_METHOD read,
                                         LNXPROC_ERROR_CALLBACK callback,
                                         size_t buflen, void *data)
{

    LNXPROC_BASE_T *base = malloc(sizeof(LNXPROC_BASE_T));
    if (!base) {
        lnxproc_set_error(callback,__func__, LNXPROC_ERROR_BASE_MALLOC_BASE);
        return base;
    }
    base->lines = calloc(1, buflen + 1);
    if (!base->lines) {
        lnxproc_set_error(callback,__func__, LNXPROC_ERROR_BASE_MALLOC_BUFFER);
        free(base);
        return NULL;
    }
    base->lines[buflen] = '\n';
    base->rawread = rawread;
    base->normalize = normalize;
    base->read = read;
    base->callback = callback;
    base->filename = filename;
    base->buflen = buflen;
    base->nbytes = 0;
    base->map = NULL;
    base->data = data;
    return base;
}

LNXPROC_BASE_T *lnxproc_base_free(LNXPROC_BASE_T *base)
{
    if (base) {
        if (base->map) {
            base->map = (void *) lnxproc_array_free((LNXPROC_ARRAY_T *)
                                                    base->map);
        }
        if (base->lines) {
            free(base->lines);
            base->lines = NULL;
        }
        free(base);
        base = NULL;
    }
    return base;
}

/* 
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab 
 */
