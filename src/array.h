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

/* This file is a re-implementation of the 'list' type from Python
*/

#ifndef LIBLNXPROC_ARRAY_H
#define LIBLNXPROC_ARRAY_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

enum array_type_t {
    LNXPROC_ARRAY_TYPE_CHARPTR = 0,
    LNXPROC_ARRAY_TYPE_STRUCTPTR,
};

struct array_t {
    size_t length;
    size_t size;
    enum array_type_t type;
    size_t fsize;
    void *data;
};

#define WARN_UNUSED __attribute__((warn_unused_result))

struct array_t *array_new(size_t size, enum array_type_t type) WARN_UNUSED;
struct array_t *array_resize(struct array_t *array, size_t size) WARN_UNUSED;
struct array_t *array_free(struct array_t *array) WARN_UNUSED;
struct array_t *array_set(struct array_t *array, size_t idx, void *val) WARN_UNUSED;

int array_clear(struct array_t *array);
void *array_get(struct array_t *array, size_t idx);

int array_iterate(struct array_t *array, 
                  void *data,
                  int start,
                  int end,
                  int (*func)(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val));

int array_print(struct array_t *array, void *data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LIBLNXPROC_ARRAY_H

