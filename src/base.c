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

#include "array.h"
#include "base.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct lnxproc_base_t {
    LNXPROC_BASE_METHOD rawread;
    LNXPROC_BASE_METHOD normalize;
    LNXPROC_BASE_METHOD read;
    const char *filename;
    char *lines;
    size_t buflen;
    int nbytes;
    char *map;
    void *data;
};

static char *errstr[] = {
    "No error",
    "Base arg is null",
    "System error",
    "Fail to malloc base",
    "Fail to malloc buffer",
};

void lnxproc_base_error_print_callback(const char *func,
                                       enum lnxproc_base_error_t err)
{
    if (err > 0) {
        printf("Error: %s -> %s\n", func, errstr[err]);
    }
}

void (*lnxproc_base_error_callback) (const char *func,
                                     enum lnxproc_base_error_t err) = NULL;

static void base_set_error(const char *func, enum lnxproc_base_error_t err)
{
    if (lnxproc_base_error_callback) {
        lnxproc_base_error_callback(func, err);
    }
}

const char *lnxproc_base_filename(struct lnxproc_base_t *base)
{
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_NULL);
        return NULL;
    }
    return base->filename;
}

char *lnxproc_base_lines(struct lnxproc_base_t *base)
{
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_NULL);
        return NULL;
    }
    return base->lines;
}

int lnxproc_base_nbytes(struct lnxproc_base_t *base)
{
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_NULL);
        return -1;
    }
    return base->nbytes;
}

void *lnxproc_base_map(struct lnxproc_base_t *base)
{
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_NULL);
        return NULL;
    }
    return base->map;
}

int lnxproc_base_map_set(struct lnxproc_base_t *base, void *map)
{
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_NULL);
        return 1;
    }
    base->map = map;
    return 0;
}

int lnxproc_base_rawread(struct lnxproc_base_t *base)
{

    if (base->rawread) {
        return base->rawread(base);
    }
    else {
        int fd = open(base->filename, O_RDONLY);
        if (fd < 0) {
            base_set_error(__func__, LNXPROC_BASE_ERROR_SYSTEM);
            return -errno;
        }

        base->nbytes = read(fd, base->lines, base->buflen);
        if (base->nbytes < 0) {
            base_set_error(__func__, LNXPROC_BASE_ERROR_SYSTEM);
            return -errno;
        }
        base->lines[base->nbytes] = '\n';

        /* we do not have to check return status of close as we are only
         * reading the file 
         */
        close(fd);
    }
    return 0;

}

int lnxproc_base_normalize(struct lnxproc_base_t *base)
{
    if (base->normalize) {
        return base->normalize(base);
    }
    return 0;
}

int lnxproc_base_read(struct lnxproc_base_t *base)
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

    return 0;
}

struct lnxproc_base_t *lnxproc_base_init(const char *filename,
                                         LNXPROC_BASE_METHOD rawread,
                                         LNXPROC_BASE_METHOD normalize,
                                         LNXPROC_BASE_METHOD read,
                                         size_t buflen, void *data)
{

    struct lnxproc_base_t *base = malloc(sizeof(struct lnxproc_base_t));
    if (!base) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_MALLOC_BASE);
        return base;
    }
    base->lines = calloc(1, buflen + 1);
    if (!base->lines) {
        base_set_error(__func__, LNXPROC_BASE_ERROR_MALLOC_BUFFER);
        free(base);
        return NULL;
    }
    base->lines[buflen] = '\n';
    base->rawread = rawread;
    base->normalize = normalize;
    base->read = read;
    base->filename = filename;
    base->buflen = buflen;
    base->nbytes = 0;
    base->map = NULL;
    base->data = data;
    return base;
}

struct lnxproc_base_t *lnxproc_base_free(struct lnxproc_base_t *base)
{
    if (base) {
        if (base->map) {
            base->map = (void *) lnxproc_array_free((struct lnxproc_array_t *)
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
