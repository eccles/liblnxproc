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

#include <stdio.h>
#include <string.h>
#include "lnxproc.h"

static int myarray_print(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val) {
    printf("Array %p : Index %d\n",array,idx);
    return 0;
}
int main(int argc, char *argv[]) {

    printf("Allocate array for structs\n");
    struct array_t *array = array_new(2,LNXPROC_ARRAY_TYPE_STRUCTPTR);
    array_print(array,NULL);
    printf("\n");

    printf("Allocate array for strings\n");
    struct array_t *carray = array_new(2,LNXPROC_ARRAY_TYPE_CHARPTR);
    array_print(carray,NULL);
    printf("\n");

    if ( array && carray) {
        printf("Add string array to struct array at position %d\n",0);
        array = array_set(array,0,carray);
        array_print(array,NULL);
        printf("\n");

        printf("Resize struct array - add %d elements\n",20);
        array = array_resize(array,20);
        array_print(array,NULL);
        printf("\n");

        printf("Add string array to struct array at position %d (should fail silently)\n",2);
        array = array_set(array,2,carray);
        array_print(array,NULL);
        printf("\n");

        printf("Get string array from struct array at position %d\n",0);
        struct array_t *myarray = array_get(array,0);
        array_print(myarray,NULL);
        printf("\n");

        printf("Get string array from struct array at position %d (should fail silently)\n",2);
        myarray = array_get(array,2);
        array_print(myarray,NULL);
        printf("\n");

        char *val1 = "value1";
        char *val2 = "value2";
        char *val3 = "value3";

        printf("Val1 at %1$p with value %1$s\n", val1);
        printf("Val2 at %1$p with value %1$s\n", val2);
        printf("Val3 at %1$p with value %1$s\n", val3);

        printf("Set idx %d to value %p\n", 0, val1);
        carray = array_set(carray,0,val1);
        array_print(carray,NULL);
        printf("\n");

        printf("Set idx %d to value %p\n", 0, val2);
        carray = array_set(carray,0,val2);
        array_print(carray,NULL);
        printf("\n");

        printf("Set idx %d to value %p\n", 1, val1);
        carray = array_set(carray,1,val1);
        array_print(carray,NULL);
        printf("\n");

        printf("Set idx %d to value %p\n", 2, val3);
        carray = array_set(carray,2,val3);
        array_print(carray,NULL);
        printf("\n");

        printf("Print elements %d to %d\n", 1, 2);
        array_iterate(carray, NULL, 1, 3, myarray_print);
        printf("\n");

        printf("Clear string array\n");
        array_clear(carray);
        array_print(carray,NULL);
        printf("\n");
    }

    struct base_t *proccgroups = proccgroups_init();
    if ( proccgroups ) {
        int state = base_read(proccgroups);
        if ( state < 0 ) {
            char errbuf[128];
            strerror_r(state, errbuf, sizeof(errbuf));
            printf("Failure reading proccgroups %s : %s\n", 
                                            base_filename(proccgroups),
                                            errbuf);
        }
        else {
            printf("Proccgroups: %s\n", base_filename(proccgroups));
            printf("Proccgroups: %d bytes\n", base_nbytes(proccgroups));
            printf("Proccgroups: %1$d %2$*1$s\n", 
                                         base_nbytes(proccgroups), 
                                         base_lines(proccgroups));
        }
    } 
    return 0;
}
/*
int array_clear(struct array_t *array);
void *array_get(struct array_t *array, size_t idx);

int array_iterate(struct array_t *array, 
                  void *data,
                  size_t start,
                  size_t end,
                  int (*func)(struct array_t *array,
                              void *data, 
                              int idx, 
                              void *val));

int array_print(struct array_t *array);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LIBLNXPROC_ARRAY_H
*/
