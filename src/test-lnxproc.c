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

//#define TEST_ERROR 1
//#define TEST_VECTOR 1
//#define TEST_LIMITS 1
#define TEST_ARRAY 1
//#define TEST_PROCCGROUPS 1

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
#ifdef TEST_LIMITS
static void
test_limits(void)
{
    printf("\nMaplimits 1\n");
    LNXPROC_LIMITS_T mylimits1[] = {
        {3, "\n", 1},
    };
    size_t dim1 = sizeof(mylimits1) / sizeof(mylimits1[0]);

    lnxproc_limits_print(mylimits1, dim1);

    LNXPROC_LIMITS_T *limits1 = lnxproc_limits_dup(lnxproc_error_print_callback,
                                                   mylimits1, dim1);

    int i;

    for (i = 0; i < dim1; i++) {
        char *c = lnxproc_chr(limits1 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits1 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits1 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits1 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits1, dim1);

    limits1 = lnxproc_limits_free(limits1, dim1);

    printf("\nMaplimits 2\n");
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
        char *c = lnxproc_chr(limits2 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits2 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits2 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits2 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits2, dim2);

    limits2 = lnxproc_limits_free(limits2, dim2);

    printf("\nMaplimits 3\n");
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
        char *c = lnxproc_chr(limits3 + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits3 + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits3 + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = lnxproc_chr(limits3 + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    lnxproc_limits_print(limits3, dim3);

    limits3 = lnxproc_limits_free(limits3, dim3);

}

/*
    char *lnxproc_chr(LNXPROC_LIMITS_T * maplimit, char c);

 */
#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_VECTOR
static int
myvector_print(LNXPROC_VECTOR_T * vector, void *data, int idx)
{
    printf("Array %p : Index %d\n", vector, idx);
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
        lnxproc_vector_set(vector, 0, cvector1);
        lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should fail)\n",
             2);
        lnxproc_vector_set(vector, 2, cvector2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("Add string vector to struct vector at position %d\n", 1);
        lnxproc_vector_set(vector, 1, cvector2);
        lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should resize)\n",
             2);
        lnxproc_vector_set(vector, 2, cvector3);
        lnxproc_vector_print(vector, 1, NULL);

        printf("Get string vector from struct vector at position %d\n", 0);
        struct lnxproc_vector_t *myvector = lnxproc_vector_get(vector, 0);

        lnxproc_vector_print(myvector, 1, NULL);

        printf
            ("Get string vector from struct vector at position %d (should fail)\n",
             3);
        myvector = lnxproc_vector_get(vector, 3);
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
        lnxproc_vector_set(cvector1, 0, val2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 0, val1);
        lnxproc_vector_set(cvector1, 0, val1);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 1, val4);
        lnxproc_vector_set(cvector1, 1, val4);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_vector_set(cvector1, 2, val7);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        lnxproc_vector_set(cvector2, 0, val2);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 1, val5);
        lnxproc_vector_set(cvector2, 1, val5);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        lnxproc_vector_set(cvector2, 2, val7);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 0, val3);
        lnxproc_vector_set(cvector3, 0, val3);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 1, val6);
        lnxproc_vector_set(cvector3, 1, val6);
        lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 2, val9);
        lnxproc_vector_set(cvector3, 2, val9);
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
static int
myarray_print(LNXPROC_VECTOR_T * vector, void *data, int idx)
{
    printf("Array %p : Index %d\n", vector, idx);
    return 0;
}

static void
test_array(void)
{
    size_t dim0 = 0;
    LNXPROC_ARRAY_T *array0 =
        lnxproc_array_new(NULL, dim0, lnxproc_error_print_callback);
    lnxproc_array_print(array0, 1, NULL);
    array0 = lnxproc_array_free(array0);

    LNXPROC_LIMITS_T limits1[] = {
        {3, "\n", 1},
    };
    size_t dim1 = sizeof(limits1) / sizeof(limits1[0]);

    LNXPROC_ARRAY_T *array1 =
        lnxproc_array_new(limits1, dim1, lnxproc_error_print_callback);
    lnxproc_array_print(array1, 1, NULL);
    array1 = lnxproc_array_free(array1);

    LNXPROC_LIMITS_T limits2[] = {
        {3, "\n", 1},
        {5, "\t", 1},
    };
    size_t dim2 = sizeof(limits2) / sizeof(limits2[0]);

    LNXPROC_ARRAY_T *array2 =
        lnxproc_array_new(limits2, dim2, lnxproc_error_print_callback);
    lnxproc_array_print(array2, 1, NULL);
    array2 = lnxproc_array_free(array2);

    LNXPROC_LIMITS_T limits3[] = {
        {3, "\n", 1},
        {5, "\t", 1},
        {5, "", 1},
    };
    size_t dim3 = sizeof(limits3) / sizeof(limits3[0]);

    LNXPROC_ARRAY_T *array3 =
        lnxproc_array_new(limits3, dim3, lnxproc_error_print_callback);
    lnxproc_array_print(array3, 1, NULL);
    array3 = lnxproc_array_free(array3);
}

/*

    LNXPROC_VECTOR_T *lnxproc_array_create(LNXPROC_ERROR_CALLBACK callback,
                                           LNXPROC_LIMITS_T limits[],
                                           size_t dim, int depth);

    LNXPROC_ARRAY_T *lnxproc_array_new(LNXPROC_LIMITS_T limits[],
                                       size_t dim,
                                       LNXPROC_ERROR_CALLBACK callback)
     WARN_UNUSED;

    LNXPROC_ARRAY_T *lnxproc_array_free(LNXPROC_ARRAY_T *array) WARN_UNUSED;

    int lnxproc_array_set(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim,
                          char *val);
    int lnxproc_array_set_last(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim,
                               char *val);
    char *lnxproc_array_get(LNXPROC_ARRAY_T *array, size_t idx[], size_t dim);

    LNXPROC_ERROR_CALLBACK lnxproc_array_callback(LNXPROC_ARRAY_T *array);

    int lnxproc_array_iterate(LNXPROC_ARRAY_T *array,
                              void *data, LNXPROC_VECTOR_ITERATE_FUNC func);

    int lnxproc_array_print(LNXPROC_ARRAY_T *array, int allocated, void *data);

*/
#endif

/*----------------------------------------------------------------------------*/
#ifdef TEST_PROCCGROUPS

static void
execute_base(LNXPROC_BASE_T *base)
{
    if (base) {
        int state = lnxproc_base_read(base);

        if (state < 0) {
            char errbuf[128];

            strerror_r(state, errbuf, sizeof(errbuf));
            printf("Failure reading proccgroups %s : %s\n",
                   lnxproc_base_filename(base), errbuf);
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
test_proccgroups(void)
{
    LNXPROC_BASE_T *proccgroups = proccgroups_init();

    if (proccgroups) {
        execute_base(proccgroups);
        execute_base(proccgroups);
        proccgroups = lnxproc_base_free(proccgroups);
    }
}
#endif

int
main(int argc, char *argv[])
{

#ifdef TEST_VECTOR
    test_error();
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
#ifdef TEST_PROCCGROUPS
    test_proccgroups();
#endif

    return 0;
}
