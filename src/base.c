/*
This file is part of liblnxproc.

    liblnxproc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    liblnxproc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2013 Paul Hewlett, phewlett76@gmail.com

*/
#include "base.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

struct base_t {
    int (*rawread)(struct base_t *base);
    int (*normalize)(struct base_t *base);
    int (*read)(struct base_t *base);
    const char *filename;
    char lines[256];
    int nbytes;
    char *map;
    void *data;
};

const char *base_filename(struct base_t *base) {
    return base->filename;
}

char *base_lines(struct base_t *base) {
    return base->lines;
}

int base_nbytes(struct base_t *base) {
    return base->nbytes;
}

void *base_map(struct base_t *base) {
    return base->map;
}

int base_map_set(struct base_t *base, void *map) {
    base->map = map;
    return 0;
}

int base_rawread(struct base_t *base) {
    
    if ( base->rawread ) {
        return base->rawread(base);
    }
    else {
        int fd = open(base->filename,O_RDONLY);
        if ( fd < 0 ) {
            return -errno;
        }

        base->nbytes = read(fd,base->lines,sizeof(base->lines));
        if ( base->nbytes < 0 ) {
        return -errno;
        }
        /* we do not have to check return status of close as we are only i
           reading the file*/
        close(fd);
    }
    return 0;

}

int base_normalize(struct base_t *base) {
    if ( base->normalize ) {
        return base->normalize(base);
    }
    return 0;
}

int base_read(struct base_t *base) {
    if ( base->read ) {
        return base->read(base);
    }
    else {
        int state = base_rawread(base);
        if ( state < 0 ) {
            return -state;
        }

        if( base->normalize ) {
            state = base->normalize(base);
            if ( state < 0 ) {
                return -state;
            }
        }
    }

    return 0;
}

struct base_t *base_init(const char *filename, 
                         int (*rawread)(struct base_t *base),
                         int (*normalize)(struct base_t *base),
                         int (*read)(struct base_t *base),
                         void *data ) {

    struct base_t *base = malloc(sizeof(struct base_t));
    if( base ) {
        base->rawread = rawread;
        base->normalize = normalize;
        base->read = read;
        base->filename = filename;
        base->lines[0] = '\n';
        base->nbytes = 0;
        base->map = NULL;
        base->data = data;
    }
    return base;
}

