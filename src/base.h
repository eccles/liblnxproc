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
#ifndef LIBLNXPROC_BASE_H
#define LIBLNXPROC_BASE_H 1

#ifdef __cplusplus
extern "C" {
#endif

struct base_t;

const char *base_filename(struct base_t *base);
char *base_lines(struct base_t *base);
void *base_map(struct base_t *base);
int base_map_set(struct base_t *base, void *map);
int base_nbytes(struct base_t *base);
int base_rawread(struct base_t *base);
int base_read(struct base_t *base);
int base_normalize(struct base_t *base);
struct base_t *base_init( const char *filename, 
                          int (*rawread)(struct base_t *base),
                          int (*normalize)(struct base_t *base),
                          int (*read)(struct base_t *base),
                          void *data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LIBLNXPROC_BASE_H

