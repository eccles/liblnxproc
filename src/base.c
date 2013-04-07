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

const char *lnxproc_base_filename(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    const char *filename = NULL;
    if (base) {
        filename = base->filename;
    }
    LNXPROC_DEBUG("filename %1$p '%1$s'\n",filename);
    return filename;
}

char *lnxproc_base_lines(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    char *lines = NULL;
    if (base) {
        lines = base->lines;
    }
    LNXPROC_DEBUG("lines %1$p '%1$s'\n",lines);
    return lines;
}

int lnxproc_base_nbytes(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    int nbytes = 0;
    if (base) {
        nbytes = base->nbytes;
        LNXPROC_DEBUG("nbytes %d\n",nbytes);
        return nbytes;
    }
    LNXPROC_DEBUG("WARNING: Base is null\n");
    return -1;
}

LNXPROC_ARRAY_T *lnxproc_base_map(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    LNXPROC_ARRAY_T *map = NULL;
    if (base) {
        map = base->map;
    }
    LNXPROC_DEBUG("map %p\n",map);
    return map;
}

int lnxproc_base_map_print(LNXPROC_BASE_T * base, void *data)
{
    LNXPROC_DEBUG("Base %p\n",base);
    if (base) {
        return lnxproc_array_print(base->map, data);
    }
    LNXPROC_DEBUG("WARNING: Base is null\n");
    return LNXPROC_ERROR_BASE_NULL;
}

LNXPROC_ERROR_CALLBACK lnxproc_base_callback(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    LNXPROC_ERROR_CALLBACK callback = NULL;
    if (base) {
        callback = base->callback;
    }
    LNXPROC_DEBUG("callback %p\n",callback);
    return callback;
}

int lnxproc_base_map_set(LNXPROC_BASE_T * base, LNXPROC_ARRAY_T * map)
{
    LNXPROC_DEBUG("Base %p\n",base);
    if (base) {
        base->map = map;
        LNXPROC_DEBUG("set map %p\n",map);
        return LNXPROC_OK;
    }
    LNXPROC_DEBUG("WARNING: Base is null\n");
    return LNXPROC_ERROR_BASE_NULL;
}

int lnxproc_base_rawread(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);

    if (base->rawread) {
        LNXPROC_DEBUG("Execute specified rawread method %p\n",base->rawread);
        return base->rawread(base);
    }
    else {
        LNXPROC_DEBUG("Execute default rawread method\n");
        LNXPROC_DEBUG("Open %s\n",base->filename);
        int fd = open(base->filename, O_RDONLY);
        if (fd < 0) {
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            return -errno;
        }

        LNXPROC_DEBUG("Read %s\n",base->filename);
        base->nbytes = read(fd, base->lines, base->buflen);
        if (base->nbytes < 0) {
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            return -errno;
        }
        LNXPROC_DEBUG("Nbytes %d read\n",base->nbytes);
        base->lines[base->nbytes] = '\n';

        /* we do not have to check return status of close as we are only
         * reading the file 
         */
        LNXPROC_DEBUG("Close %s\n",base->filename);
        close(fd);
    }
    return LNXPROC_OK;

}

int lnxproc_base_normalize(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    if (base->normalize) {
        LNXPROC_DEBUG("Execute specified normalize method %p\n",base->normalize);
        return base->normalize(base);
    }
    return LNXPROC_OK;
}

int lnxproc_base_read(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    if (base->read) {
        LNXPROC_DEBUG("Execute specified read method %p\n",base->read);
        return base->read(base);
    }
    else {
        LNXPROC_DEBUG("Execute default read method\n");
        int state = lnxproc_base_rawread(base);
        if (state) {
#ifdef DEBUG
            char buf[128];
            char *c = lnxproc_strerror(state,buf,sizeof buf);
            LNXPROC_DEBUG("Rawread error %d %s\n",state,c);
#endif
            return state;
        }

        if (base->normalize) {
            state = base->normalize(base);
            if (state) {
#ifdef DEBUG
            char buf[128];
            char *c = lnxproc_strerror(state,buf,sizeof buf);
            LNXPROC_DEBUG("Normalize error %d %s\n",state,c);
#endif
                return state;
            }
        }
    }

    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_BASE_T *lnxproc_base_init(const char *filename,
                                  LNXPROC_BASE_METHOD rawread,
                                  LNXPROC_BASE_METHOD normalize,
                                  LNXPROC_BASE_METHOD read,
                                  LNXPROC_ERROR_CALLBACK callback,
                                  size_t buflen, void *data)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n",filename);

    LNXPROC_BASE_T *base = malloc(sizeof(LNXPROC_BASE_T));
    if (!base) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BASE);
#ifdef DEBUG
        char buf[128];
        char *c = lnxproc_strerror(LNXPROC_ERROR_BASE_MALLOC_BASE,buf,sizeof buf);
        LNXPROC_DEBUG("Malloc %s\n",c);
#endif
        return base;
    }
    base->lines = calloc(1, buflen + 1);
    if (!base->lines) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BUFFER);
#ifdef DEBUG
        char buf[128];
        char *c = lnxproc_strerror(LNXPROC_ERROR_BASE_MALLOC_BUFFER,buf,sizeof buf);
        LNXPROC_DEBUG("Malloc %s\n",c);
#endif
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
    LNXPROC_DEBUG("Successful\n");
    return base;
}

LNXPROC_BASE_T *lnxproc_base_free(LNXPROC_BASE_T * base)
{
    LNXPROC_DEBUG("Base %p\n",base);
    if (base) {
        if (base->map) {
            LNXPROC_DEBUG("Free Base map\n");
            base->map = (void *) lnxproc_array_free((LNXPROC_ARRAY_T *)
                                                    base->map);
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
