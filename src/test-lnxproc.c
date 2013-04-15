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
#include <stdlib.h>
#include <string.h>
#include "lnxproc.h"

#define TEST_ERROR 1
#define TEST_RESULTS 1
#define TEST_VECTOR 1
#define TEST_LIMITS 1
#define TEST_ARRAY 1
#define TEST_PROC_CGROUPS 1

/*----------------------------------------------------------------------------*/
#ifdef TEST_ERROR
static void
test_error(void)
{
    LNXPROC_ERROR_CHECK();

    int i;

    for (i = 0; i < LNXPROC_ERROR_SIZE; i++) {
        LNXPROC_SET_ERROR(lnxproc_error_print_callback, i);
    }
}
#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_RESULTS
static void
test_results(void)
{
    LNXPROC_RESULTS_T *results = lnxproc_results_new();

    lnxproc_results_print(results);
    results = lnxproc_results_free(results);

/*
    LNXPROC_DB_DATA_T key = {
        .dptr = (unsigned char *) "firstkey",
        .dsize = sizeof "firstkey",
    };
    printf("Key is %zd bytes with value %s\n", key.dsize, key.dptr);
    LNXPROC_DB_DATA_T data = {
        .dptr = (unsigned char *) "firstdata",
        .dsize = sizeof "firstdata",
    };
    printf("Data is %zd bytes with value %s\n", data.dsize, data.dptr);

    lnxproc_db_store(db, key, data);
    LNXPROC_DB_DATA_T data1 =
        lnxproc_db_fetch(db, (char *) key.dptr, key.dsize);
    printf("Data is %zd bytes with value %s ", data1.dsize, data1.dptr);
    if (data1.dptr) {
        printf("Success\n");
    }
    else {
        printf("FAIL\n");
    }

    free(data1.dptr);
    db = lnxproc_db_free(db);
*/
}
#endif
/*----------------------------------------------------------------------------*/
#ifdef TEST_LIMITS
static void
test_limits(void)
{
    printf("\nLimits 1\n");
    LNXPROC_LIMITS_T mylimits1[] = {
        {3, "\n", 1},
    };
    size_t dim1 = sizeof(mylimits1) / sizeof(mylimits1[0]);

    lnxproc_limits_print(mylimits1, dim1);

    LNXPROC_LIMITS_T *limits1 = lnxproc_limits_dup(lnxproc_error_print_callback,
                                                   mylimits1, dim1);

    int i;

    for (i = 0; i < dim1; i++) {
        char *c = lnxproc_limit_chr(limits1 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits1 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits1 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits1 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits1, dim1);

    limits1 = lnxproc_limits_free(limits1, dim1);

    printf("\nLimits 2\n");
    LNXPROC_LIMITS_T mylimits2[] = {
        {3, "\n", 1}
        ,
        {5, "\t", 1}
        ,
    };
    size_t dim2 = sizeof(mylimits2) / sizeof(mylimits2[0]);

    lnxproc_limits_print(mylimits2, dim2);

    LNXPROC_LIMITS_T *limits2 = lnxproc_limits_dup(lnxproc_error_print_callback,
                                                   mylimits2, dim2);

    for (i = 0; i < dim2; i++) {
        char *c = lnxproc_limit_chr(limits2 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits2 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits2 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits2 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits2, dim2);

    limits2 = lnxproc_limits_free(limits2, dim2);

    printf("\nLimits 3\n");
    LNXPROC_LIMITS_T mylimits3[] = {
        {3, "\n", 1}
        ,
        {5, "\t", 1}
        ,
        {5, "", 1}
        ,
    };
    size_t dim3 = sizeof(mylimits3) / sizeof(mylimits3[0]);

    lnxproc_limits_print(mylimits3, dim3);

    LNXPROC_LIMITS_T *limits3 = lnxproc_limits_dup(lnxproc_error_print_callback,
                                                   mylimits3, dim3);

    for (i = 0; i < dim3; i++) {
        char *c = lnxproc_limit_chr(limits3 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits3 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits3 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_limit_chr(limits3 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits3, dim3);

    limits3 = lnxproc_limits_free(limits3, dim3);

}

#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_VECTOR
static int
myvector_print(LNXPROC_VECTOR_DATA_T * val, int recursive, void *data,
               size_t idx)
{
    if (recursive) {
        printf("Child %p : Index %zd\n", lnxproc_data_child(val), idx);
    }
    else {
        printf("Val %p : Index %zd\n", lnxproc_data_value(val), idx);
    }
    return 0;
}

static void
test_vector(void)
{
    printf("Allocate vector for structs\n");
    struct lnxproc_vector_t *vector =
        lnxproc_vector_new(2, 1, lnxproc_error_print_callback);
    lnxproc_vector_print(vector, 1, NULL);

    printf("Allocate vector 1 for strings\n");
    struct lnxproc_vector_t *cvector1 =
        lnxproc_vector_new(2, 0, lnxproc_error_print_callback);
    lnxproc_vector_print(cvector1, 1, NULL);

    printf("Allocate vector 2 for strings\n");
    struct lnxproc_vector_t *cvector2 =
        lnxproc_vector_new(2, 0, lnxproc_error_print_callback);
    lnxproc_vector_print(cvector2, 1, NULL);

    printf("Allocate vector 3 for strings\n");
    struct lnxproc_vector_t *cvector3 =
        lnxproc_vector_new(2, 0, lnxproc_error_print_callback);
    lnxproc_vector_print(cvector3, 1, NULL);

    if (vector && cvector1 && cvector2 && cvector3) {

        printf("Add string vector to struct vector at position %d\n", 0);
        lnxproc_vector_set_child(vector, 0, cvector1);
        lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should fail)\n",
             2);
        lnxproc_vector_set_child(vector, 2, cvector2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("Add string vector to struct vector at position %d\n", 1);
        lnxproc_vector_set_child(vector, 1, cvector2);
        lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should resize)\n",
             2);
        lnxproc_vector_set_child(vector, 2, cvector3);
        lnxproc_vector_print(vector, 1, NULL);

        printf("Get string vector from struct vector at position %d\n", 0);
        LNXPROC_VECTOR_T *myvector = lnxproc_vector_child(vector, 0);

        lnxproc_vector_print(myvector, 1, NULL);

        printf
            ("Get string vector from struct vector at position %d (should fail)\n",
             3);
        myvector = lnxproc_vector_child(vector, 3);
        lnxproc_vector_print(myvector, 1, NULL);

        printf("Resize struct vector - add %d elements\n", 20);
        lnxproc_vector_resize(vector, 20);
        lnxproc_vector_print(vector, 1, NULL);

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

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        lnxproc_vector_set_value(cvector1, 0, val2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 0, val1);
        lnxproc_vector_set_value(cvector1, 0, val1);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 1, val4);
        lnxproc_vector_set_value(cvector1, 1, val4);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_vector_set_value(cvector1, 2, val7);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        lnxproc_vector_set_value(cvector2, 0, val2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 1, val5);
        lnxproc_vector_set_value(cvector2, 1, val5);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_vector_set_value(cvector2, 2, val7);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 0, val3);
        lnxproc_vector_set_value(cvector3, 0, val3);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 1, val6);
        lnxproc_vector_set_value(cvector3, 1, val6);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 2, val9);
        lnxproc_vector_set_value(cvector3, 2, val9);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Resize string vector - add %d elements\n", 20);
        lnxproc_vector_resize(cvector2, 20);
        lnxproc_vector_print(vector, 1, NULL);

        printf("Print elements %d to %d\n", 1, 2);
        lnxproc_vector_iterate(vector, NULL, 1, 3, myvector_print);
        printf("\n");

        vector = lnxproc_vector_free(vector);
    }

}
#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_ARRAY
static void
test_array(void)
{

    printf("Array 0\n");
    LNXPROC_LIMITS_T limits0[] = {
        {3, "\n", 1},
    };
    LNXPROC_ARRAY_T *array0 =
        lnxproc_array_new(limits0, 0, lnxproc_error_print_callback);

    lnxproc_array_set(array0, NULL, 0, "array0");
    char *val = lnxproc_array_get(array0, NULL, 0);

    printf("Got value %1$p '%1$s'\n", val);

    lnxproc_array_print(array0, 1, NULL);
    array0 = lnxproc_array_free(array0);

    printf("Array 1\n");
    LNXPROC_LIMITS_T limits1[] = {
        {3, "\n", 1},
    };
    size_t dim1 = sizeof(limits1) / sizeof(limits1[0]);

    LNXPROC_ARRAY_T *array1 =
        lnxproc_array_new(limits1, dim1, lnxproc_error_print_callback);

    size_t idx1[] = { 0 };
    lnxproc_array_set(array1, idx1, 1, "array1 0");
    size_t idx13[] = { 3 };
    lnxproc_array_set_last(array1, idx13, 1, "array1 3");
    size_t idx12[] = { 1 };
    lnxproc_array_set_last(array1, idx12, 1, "array1 1");

    lnxproc_array_print(array1, 1, NULL);
    array1 = lnxproc_array_free(array1);

    printf("Array 2\n");
    LNXPROC_LIMITS_T limits2[] = {
        {3, "\n", 1},
        {5, "\t", 1},
    };
    size_t dim2 = sizeof(limits2) / sizeof(limits2[0]);

    LNXPROC_ARRAY_T *array2 =
        lnxproc_array_new(limits2, dim2, lnxproc_error_print_callback);
    lnxproc_array_print(array2, 1, NULL);

    size_t idx2[] = { 0, 0 };
    lnxproc_array_set(array2, idx2, 2, "array2 0 0");
    lnxproc_array_print(array2, 1, NULL);

    size_t idx21[] = { 1, 1 };
    lnxproc_array_set_last(array2, idx21, 2, "array2 1 1");
    lnxproc_array_print(array2, 1, NULL);

    array2 = lnxproc_array_free(array2);

    printf("Array 3\n");
    LNXPROC_LIMITS_T limits3[] = {
        {3, "\n", 1},
        {5, "\t", 1},
        {5, "", 1},
    };
    size_t dim3 = sizeof(limits3) / sizeof(limits3[0]);

    LNXPROC_ARRAY_T *array3 =
        lnxproc_array_new(limits3, dim3, lnxproc_error_print_callback);

    size_t idx3[] = { 0, 0, 0 };
    lnxproc_array_set(array3, idx3, 3, "array3 0 0 0");
    size_t idx31[] = { 0, 0, 1 };
    lnxproc_array_set_last(array3, idx31, 3, "array3 0 0 1");
    size_t idx32[] = { 1, 2, 3 };
    lnxproc_array_set_last(array3, idx32, 3, "array3 1 2 3");

    lnxproc_array_print(array3, 1, NULL);
    array3 = lnxproc_array_free(array3);

}

#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_PROC_CGROUPS

static void
execute_base(LNXPROC_BASE_T *base)
{
    if (base) {
        LNXPROC_RESULTS_T *res = lnxproc_base_read(base);

        if (!res) {
            printf("Failure reading proc_cgroups %s\n",
                   lnxproc_base_filename(base));
        }
        else {
            printf("Proccgroups: %s\n", lnxproc_base_filename(base));
            printf("Proccgroups: %d bytes\n", lnxproc_base_nbytes(base));
            printf("Proccgroups: %1$d %2$*1$s\n",
                   lnxproc_base_nbytes(base), lnxproc_base_lines(base));
        }
    }
}

static void
test_proc_cgroups(void)
{
    LNXPROC_BASE_T *proc_cgroups = lnxproc_proc_cgroups_new();

    if (proc_cgroups) {
        execute_base(proc_cgroups);
        execute_base(proc_cgroups);
        proc_cgroups = lnxproc_base_free(proc_cgroups);
    }
}
#endif

int
main(int argc, char *argv[])
{

#ifdef TEST_ERROR
    test_error();
#endif
#ifdef TEST_RESULTS
    test_results();
#endif
#ifdef TEST_VECTOR
    test_vector();
#endif
#ifdef TEST_LIMITS
    test_limits();
#endif
#ifdef TEST_ARRAY
    test_array();
#endif
#ifdef TEST_PROC_CGROUPS
    test_proc_cgroups();
#endif

    return 0;
}
