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
    LNXPROC_NORMALIZE_METHOD normalize;
    LNXPROC_BASE_METHOD read;
    LNXPROC_ERROR_CALLBACK callback;
    const char *filename;
    char *lines;
    size_t buflen;
    int nbytes;
    LNXPROC_ARRAY_T *map;
    int *arraydims;
    LNXPROC_MAP_LIMITS_T *maplimits;
    size_t mapdim;
    void *data;
};

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

int
lnxproc_base_map_dim(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    int mapdim = 0;

    if (base) {
        mapdim = base->mapdim;
        LNXPROC_DEBUG("mapdim %d\n", mapdim);
        return mapdim;
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return -1;
}

LNXPROC_MAP_LIMITS_T *
lnxproc_base_map_limits(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    LNXPROC_MAP_LIMITS_T *maplimits = NULL;

    if (base) {
        maplimits = base->maplimits;
        LNXPROC_DEBUG("maplimits %p\n", maplimits);
        return maplimits;
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return maplimits;
}

int *
lnxproc_base_array_dims(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    int *arraydims = NULL;

    if (base) {
        arraydims = base->arraydims;
        LNXPROC_DEBUG("arraydims %p\n", arraydims);
        return arraydims;
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return arraydims;
}

LNXPROC_ARRAY_T *
lnxproc_base_map(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);
    LNXPROC_ARRAY_T *map = NULL;

    if (base) {
        map = base->map;
    }

    LNXPROC_DEBUG("map %p\n", map);
    return map;
}

int
lnxproc_base_print(LNXPROC_BASE_T *base, void *data)
{
    LNXPROC_DEBUG("Base %p Data %p\n", base);

    if (base) {
        printf("Rawread %p\n", base->rawread);
        printf("Normalize %p\n", base->normalize);
        printf("Read %p\n", base->read);
        printf("Callback %p\n", base->callback);
        printf("Filename %s\n", base->filename);
        printf("Lines %p\n", base->lines);
        printf("Buflen %zd\n", base->buflen);
        printf("Nbytes %d\n", base->nbytes);
        printf("Map %p\n", base->map);
        printf("Map dimension %zd\n", base->mapdim);
        printf("Map limits %p\n", base->maplimits);
        int i;

        for (i = 0; i < base->mapdim; i++) {
            char buf[64];
            char *p = lnxproc_print_map_limit(base->maplimits + i, buf,
                                              sizeof buf);

            printf("Map limit %d :%s:\n", i, p);
        }

        printf("Array dims %p\n", base->arraydims);
        for (i = 0; i < base->mapdim; i++) {
            printf("Array dim %d %d\n", i, base->arraydims[i]);
        }

        printf("Data %p\n", base->data);
        return lnxproc_array_print(base->map, data);
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return LNXPROC_ERROR_BASE_NULL;
}

int
lnxproc_base_map_print(LNXPROC_BASE_T *base, void *data)
{
    LNXPROC_DEBUG("Base %p Data %p\n", base);

    if (base) {
        return lnxproc_array_print(base->map, data);
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

int
lnxproc_base_map_set(LNXPROC_BASE_T *base, LNXPROC_ARRAY_T *map)
{
    LNXPROC_DEBUG("Base %p Map %p\n", base);

    if (base) {
        base->map = map;
        LNXPROC_DEBUG("set map %p\n", map);
        return LNXPROC_OK;
    }

    LNXPROC_DEBUG("WARNING: Base is null\n");
    return LNXPROC_ERROR_BASE_NULL;
}

int
lnxproc_base_rawread(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return LNXPROC_ERROR_BASE_NULL;
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
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            LNXPROC_ERROR_DEBUG(-errno, "Open %s\n", base->filename);
            return -errno;
        }

        LNXPROC_DEBUG("Read %s\n", base->filename);
        base->nbytes = read(fd, base->lines, base->buflen);

        if (base->nbytes < 0) {
            LNXPROC_SYSTEM_ERROR(base->callback, errno);
            LNXPROC_ERROR_DEBUG(-errno, "Open %s\n", base->filename);
            return -errno;
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

int
lnxproc_base_normalize(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (base->normalize) {
        LNXPROC_DEBUG("Execute specified normalize method %p\n",
                      base->normalize);
        return base->normalize(base, base->arraydims, base->maplimits,
                               base->mapdim);
    }

    return LNXPROC_OK;
}

int
lnxproc_base_read(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

    if (!base) {
        LNXPROC_SET_ERROR(base->callback, LNXPROC_ERROR_BASE_NULL);
        return LNXPROC_ERROR_BASE_NULL;
    }

    if (base->read) {
        LNXPROC_DEBUG("Execute specified read method %p\n", base->read);
        return base->read(base);

    }

    else {
        LNXPROC_DEBUG("Execute default read method\n");
        int state = lnxproc_base_rawread(base);

        if (state) {
            LNXPROC_ERROR_DEBUG(state, "Rawread\n");
            return state;
        }

        if (base->normalize) {
            LNXPROC_DEBUG("Execute default normalize method\n");
            state =
                base->normalize(base, base->arraydims, base->maplimits,
                                base->mapdim);

            if (state) {
                LNXPROC_ERROR_DEBUG(state, "Normalize\n");
                return state;
            }
        }
    }

    LNXPROC_DEBUG("Successful\n");
    return LNXPROC_OK;
}

LNXPROC_BASE_T *
lnxproc_base_init(const char *filename,
                  LNXPROC_BASE_METHOD rawread,
                  LNXPROC_NORMALIZE_METHOD normalize,
                  LNXPROC_BASE_METHOD read,
                  LNXPROC_ERROR_CALLBACK callback,
                  size_t buflen,
                  int *arraydims,
                  LNXPROC_MAP_LIMITS_T maplimits[], size_t mapdim, void *data)
{
    LNXPROC_DEBUG("filename %1$p '%1$s'\n", filename);
    LNXPROC_DEBUG("rawread %p, normalize %p, read %p, callback %p\n", rawread,
                  normalize, read, callback);
    LNXPROC_DEBUG("buflen %zd Data %p\n", buflen, data);
    LNXPROC_DEBUG("arraydims %p\n", arraydims);
    LNXPROC_DEBUG("maplimits %p mapdim %d\n", maplimits, mapdim);

    LNXPROC_BASE_T *base = malloc(sizeof(LNXPROC_BASE_T));

    if (!base) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BASE);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BASE, "Malloc base\n");
        return base;
    }

    LNXPROC_DEBUG("Calloc lines buffer\n");
    base->lines = calloc(1, buflen + 1);

    if (!base->lines) {
        LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_BUFFER);
        LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_BASE, "Malloc buffer\n");
        free(base);
        return NULL;
    }

    if (mapdim > 0) {
        LNXPROC_DEBUG("Malloc arraydims %zd\n", mapdim);
        base->arraydims = malloc(mapdim * sizeof(int));
        if (!base->arraydims) {
            LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_ARRAYDIMS);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_ARRAYDIMS,
                                "Malloc array dims\n");
            free(base->lines);
            free(base);
            return NULL;
        }
        LNXPROC_DEBUG("Malloc maplimits %zd\n", mapdim);
        base->maplimits = malloc(mapdim * sizeof(LNXPROC_MAP_LIMITS_T));
        if (!base->maplimits) {
            LNXPROC_SET_ERROR(callback, LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS);
            LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS,
                                "Malloc maplimits\n");
            free(base->arraydims);
            free(base->lines);
            free(base);
            return NULL;
        }
        LNXPROC_DEBUG("Malloc maplimits %p\n", base->maplimits);
        int i;

        for (i = 0; i < mapdim; i++) {
            base->arraydims[i] = arraydims[i];

            char buf[64];

            char *p = lnxproc_print_map_limit(maplimits + i, buf, sizeof buf);

            base->maplimits[i].chars = strdup(maplimits[i].chars);
            if (!base->maplimits[i].chars) {
                LNXPROC_SET_ERROR(callback,
                                  LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS_ENTRY);
                LNXPROC_ERROR_DEBUG(LNXPROC_ERROR_BASE_MALLOC_MAPLIMITS_ENTRY,
                                    "Malloc maplimits entry\n");
                int j;

                for (j = i - 1; j >= 0; j--) {
                    free(base->maplimits[j].chars);
                }
                free(base->maplimits);
                free(base->arraydims);
                free(base->lines);
                free(base);
                return NULL;
            }
            base->maplimits[i].len = maplimits[i].len;

            p = lnxproc_print_map_limit(base->maplimits + i, buf, sizeof buf);

            LNXPROC_DEBUG("Malloc maplimit %zd :%s:\n", i, p);
        }
        base->mapdim = mapdim;
    }
    else {
        base->arraydims = NULL;
        base->maplimits = NULL;
        base->mapdim = 0;
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

LNXPROC_BASE_T *
lnxproc_base_free(LNXPROC_BASE_T *base)
{
    LNXPROC_DEBUG("Base %p\n", base);

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
        if (base->arraydims) {
            LNXPROC_DEBUG("Free Array dims\n");
            free(base->arraydims);
            base->arraydims = NULL;
        }
        if (base->maplimits) {
            LNXPROC_DEBUG("Free Maplimits buffer %zd\n", base->mapdim);
            int i;

            for (i = 0; i < base->mapdim; i++) {
                LNXPROC_DEBUG("Free Maplimit buffer %d\n", i);
                free(base->maplimits[i].chars);
                base->maplimits[i].chars = NULL;
            }
            LNXPROC_DEBUG("Free Maplimits buffer %p\n", base->maplimits);
            free(base->maplimits);
            base->maplimits = NULL;
        }
        base->mapdim = 0;

        LNXPROC_DEBUG("Free Base\n");
        free(base);
        base = NULL;
    }

    return base;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
