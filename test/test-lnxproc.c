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

static int myarray_print(struct lnxproc_array_t *array,
                              void *data, 
                              int idx) {
    printf("Array %p : Index %d\n",array,idx);
    return 0;
}
static void execute_base(LNXPROC_BASE_T *base) {
    if( base ) {
        int state = lnxproc_base_read(base);
        if ( state < 0 ) {
            char errbuf[128];
            strerror_r(state, errbuf, sizeof(errbuf));
            printf("Failure reading proccgroups %s : %s\n", 
                                            lnxproc_base_filename(base),
                                            errbuf);
        }
        else {
            printf("Proccgroups: %s\n", lnxproc_base_filename(base));
            printf("Proccgroups: %d bytes\n", lnxproc_base_nbytes(base));
            printf("Proccgroups: %1$d %2$*1$s\n", 
                                         lnxproc_base_nbytes(base), 
                                         lnxproc_base_lines(base));
        }
    }
}

int main(int argc, char *argv[]) {

    printf("Allocate array for structs\n");
    struct lnxproc_array_t *array = lnxproc_array_new(2,1,lnxproc_error_print_callback);
    lnxproc_array_print(array,NULL);

    printf("Allocate array 1 for strings\n");
    struct lnxproc_array_t *carray1 = lnxproc_array_new(2,0,lnxproc_error_print_callback);
    lnxproc_array_print(carray1,NULL);

    printf("Allocate array 2 for strings\n");
    struct lnxproc_array_t *carray2 = lnxproc_array_new(2,0,lnxproc_error_print_callback);
    lnxproc_array_print(carray2,NULL);

    printf("Allocate array 3 for strings\n");
    struct lnxproc_array_t *carray3 = lnxproc_array_new(2,0,lnxproc_error_print_callback);
    lnxproc_array_print(carray3,NULL);

    if ( array && carray1 && carray2 && carray3 ) {

        printf("Add string array to struct array at position %d\n",0);
        lnxproc_array_set(array,0,carray1);
        lnxproc_array_print(array,NULL);

        printf("Add string array to struct array at position %d (should fail)\n",2);
        lnxproc_array_set(array,2,carray2);
        lnxproc_array_print(array,NULL);

        printf("Add string array to struct array at position %d\n",1);
        lnxproc_array_set(array,1,carray2);
        lnxproc_array_print(array,NULL);

        printf("Add string array to struct array at position %d (should resize)\n",2);
        lnxproc_array_set(array,2,carray3);
        lnxproc_array_print(array,NULL);

        printf("Get string array from struct array at position %d\n",0);
        struct lnxproc_array_t *myarray = lnxproc_array_get(array,0);
        lnxproc_array_print(myarray,NULL);

        printf("Get string array from struct array at position %d (should fail)\n",3);
        myarray = lnxproc_array_get(array,3);
        lnxproc_array_print(myarray,NULL);

        printf("Resize struct array - add %d elements\n",20);
        lnxproc_array_resize(array,20);
        lnxproc_array_print(array,NULL);

        char *val1 = "value1";
        char *val2 = "value2";
        char *val3 = "value3";
        char *val4 = "value4";
        char *val5 = "value5";
        char *val6 = "value6";
        char *val7 = "value7";
        char *val8 = "value8";
        char *val9 = "value9";

        printf("Val1 at %1$p with value %1$s\n", val1);
        printf("Val2 at %1$p with value %1$s\n", val2);
        printf("Val3 at %1$p with value %1$s\n", val3);
        printf("Val4 at %1$p with value %1$s\n", val4);
        printf("Val5 at %1$p with value %1$s\n", val5);
        printf("Val6 at %1$p with value %1$s\n", val6);
        printf("Val7 at %1$p with value %1$s\n", val7);
        printf("Val8 at %1$p with value %1$s\n", val8);
        printf("Val9 at %1$p with value %1$s\n", val9);

        printf("carray1: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        lnxproc_array_set(carray1,0,val2);
        lnxproc_array_print(array,NULL);

        printf("carray1: Set idx %1$d to value %2$p '%2$s'\n", 0, val1);
        lnxproc_array_set(carray1,0,val1);
        lnxproc_array_print(array,NULL);

        printf("carray1: Set idx %1$d to value %2$p '%2$s'\n", 1, val4);
        lnxproc_array_set(carray1,1,val4);
        lnxproc_array_print(array,NULL);

        printf("carray1: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_array_set(carray1,2,val7);
        lnxproc_array_print(array,NULL);

        printf("carray2: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        lnxproc_array_set(carray2,0,val2);
        lnxproc_array_print(array,NULL);

        printf("carray2: Set idx %1$d to value %2$p '%2$s'\n", 1, val5);
        lnxproc_array_set(carray2,1,val5);
        lnxproc_array_print(array,NULL);

        printf("carray2: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_array_set(carray2,2,val7);
        lnxproc_array_print(array,NULL);

        printf("carray3: Set idx %1$d to value %2$p '%2$s'\n", 0, val3);
        lnxproc_array_set(carray3,0,val3);
        lnxproc_array_print(array,NULL);

        printf("carray3: Set idx %1$d to value %2$p '%2$s'\n", 1, val6);
        lnxproc_array_set(carray3,1,val6);
        lnxproc_array_print(array,NULL);

        printf("carray3: Set idx %1$d to value %2$p '%2$s'\n", 2, val9);
        lnxproc_array_set(carray3,2,val9);
        lnxproc_array_print(array,NULL);

        printf("carray2: Resize string array - add %d elements\n",20);
        lnxproc_array_resize(carray2,20);
        lnxproc_array_print(array,NULL);

        printf("Print elements %d to %d\n", 1, 2);
        lnxproc_array_iterate(array, NULL, 1, 3, myarray_print);
        printf("\n");

        array = lnxproc_array_free(array);
    }

    LNXPROC_BASE_T *proccgroups = proccgroups_init();
    if ( proccgroups ) {
        execute_base(proccgroups);
        execute_base(proccgroups);
        proccgroups = lnxproc_base_free(proccgroups);
    } 

    return 0;
}
