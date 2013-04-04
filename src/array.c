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

#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct array_t {
//    size_t length;
//    size_t size;
//    void *data[];
//};

static size_t array_type_field_size[2] = { sizeof(char *), sizeof(struct array_t *), };

struct array_t *array_new(size_t size, enum array_type_t type) {
    struct array_t *array = NULL;
    void *p = malloc( sizeof(struct array_t));
    if( p ) {
        array = p;
        array->length = 0;
        array->size = size;
        array->fsize = array_type_field_size[type];
        array->type = type;
        /* We have to malloc this separately as other objects may hold 
           references to 'array' and we do not want 'array' to change on a 
           realloc */
        array->data = malloc( size * array->fsize);
        if( !array->data ) {
            free(array);
            array = NULL;
        }
    }
    return array;   
}


struct array_t *array_resize(struct array_t *array, size_t size) {
    if( array ) {
        void *p = realloc(array->data, array->size + (size * array->fsize));
        if ( p ) {
            array->data = p;
            array->size += size;
        }
    }
    return array;   
}

void *array_get(struct array_t *array, size_t idx) {
    void *val = NULL;
    if( array && ( idx < array->length ) ) {
        switch ( array->type ) {
            case LNXPROC_ARRAY_TYPE_CHARPTR:
                val = ((char **)array->data)[idx];
                break;
            case LNXPROC_ARRAY_TYPE_STRUCTPTR:
                val = ((struct array_t **)array->data)[idx];
                break;
        }
    }
    return val;
}

struct array_t *array_set(struct array_t *array, size_t idx, void *val) {
    if( array ) {
        if( idx < array->length ) {
            switch ( array->type ) {
                case LNXPROC_ARRAY_TYPE_CHARPTR:
                    ((char **)array->data)[idx] = val;
                    break;
                case LNXPROC_ARRAY_TYPE_STRUCTPTR:
                    ((struct array_t **)array->data)[idx] = val;
                    break;
            }
        }
        else if( idx == array->length ) {
            if( idx > array->size ) {
                array = array_resize(array,1);
            }
            switch ( array->type ) {
                case LNXPROC_ARRAY_TYPE_CHARPTR:
                    ((char **)array->data)[idx] = val;
                    break;
                case LNXPROC_ARRAY_TYPE_STRUCTPTR:
                    ((struct array_t **)array->data)[idx] = val;
                    break;
            }
            array->length++;
        }
    }
    return array;
}

int array_iterate(struct array_t *array, 
                  void *data,
                  int start,
                  int end,
                  int (*func)(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val)) {
    if( array ) {
        int i;
        if ( start < 0 ) start = 0;
        else if ( start > array->length ) start = array->length;
        if ( end < 0 ) end = array->length;
        else if ( end > array->length ) end = array->length;
        for( i = start; i < end; i++ ) {
            func(array,data,i,array->data + (i * array->fsize));
        }
        return 0;
    }
    return 1;
}

static int array_destroy_internal(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val) {
    if( array ) {
        void *p = array_get(array, idx);
        if ( array->type == LNXPROC_ARRAY_TYPE_STRUCTPTR ) {
            array_iterate(p,NULL,-1,-1,array_destroy_internal);
        }
        return 0;
    }
    return 1;
}
struct array_t *array_destroy(struct array_t *array) {
    if( array ) {
        if ( array->type == LNXPROC_ARRAY_TYPE_STRUCTPTR ) {
            array_iterate(array,NULL,-1,-1,array_destroy_internal);
        }
        if ( array->data ) {
            free(array->data);
            array->data = NULL;
        }
        free(array);
    }
    return NULL;
}

int array_clear(struct array_t *array) {
    if( array ) {
        if ( array->type == LNXPROC_ARRAY_TYPE_STRUCTPTR ) {
        }
        array->length = 0;
        return 0;
    }
    return 1;
}

struct array_print_var_t {
    int depth;
};

static int array_print_depth( struct array_print_var_t *printvar ) {
    int depth = 0;
    if( printvar ) depth = printvar->depth;
    return depth;
}
static void array_print_indent( int depth ) {
    int i;
    for (i = 0; i < depth; i++ ) {
        printf("    ");
    }
}
static int array_print_internal(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val) {
    if( array ) {
        void *p = array_get(array, idx);
        int depth = 0;
        switch ( array->type ) {
            case LNXPROC_ARRAY_TYPE_CHARPTR:
                depth = array_print_depth( data);
                array_print_indent( depth);
                printf( "--> Value %d:%s\n", idx, (char *)p );
                break;
            case LNXPROC_ARRAY_TYPE_STRUCTPTR:
                depth = array_print_depth( data);
                array_print_indent( depth);
                struct array_print_var_t printvar = {
                    .depth = depth + 1,
                };
                printf( "--> Value %d:%p\n", idx, p );
                array_print(p,&printvar);
                break;
        }
        return 0;
    }
    return 1;
}

int array_print(struct array_t *array, void *data) {
    int depth = array_print_depth(data);
    array_print_indent( depth);
    printf( "Array at %p\n", array);
    if( array ) {
        array_print_indent( depth);
        printf( "Array size %zd\n", array->size );
        array_print_indent( depth);
        printf( "Array length %zd\n", array->length );
        array_print_indent( depth);
        printf( "Array type %d\n", array->type );
        array_print_indent( depth);
        printf( "Array fsize %zd\n", array->fsize );
        array_print_indent( depth);
        printf( "Array data at %p\n", array->data );
        array_print_indent( depth);
        array_iterate(array,data,-1,-1,array_print_internal);
        return 0;
    }
    return 1;
}
