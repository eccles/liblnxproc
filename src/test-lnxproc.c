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
#include <lnxproc/lnxproc.h>

/*----------------------------------------------------------------------------*/
static void
test_error(void)
{
    LNXPROC_ERROR_CHECK();

    int i;

    for (i = 0; i < LNXPROC_ERROR_SIZE; i++) {
        LNXPROC_ERROR_DEBUG(i, "Test\n");
    }
}

/*----------------------------------------------------------------------------*/
static void
test_results(void)
{
    LNXPROC_RESULTS_T *results = NULL;

    lnxproc_results_new(&results);

    lnxproc_results_print(results);
    LNXPROC_RESULTS_FREE(results);

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

/*----------------------------------------------------------------------------*/
static void
test_limits(void)
{
    printf("\nLimits 1\n");
    LNXPROC_LIMITS_T mylimits1[] = {
        {3, "\n", 1},
    };
    size_t dim1 = sizeof(mylimits1) / sizeof(mylimits1[0]);

    lnxproc_limits_print(mylimits1, dim1);

    LNXPROC_LIMITS_T *limits1 = NULL;

    lnxproc_limits_dup(&limits1, mylimits1, dim1);

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

    LNXPROC_LIMITS_FREE(limits1, dim1);

    printf("\nLimits 2\n");
    LNXPROC_LIMITS_T mylimits2[] = {
        {3, "\n", 1}
        ,
        {5, "\t", 1}
        ,
    };
    size_t dim2 = sizeof(mylimits2) / sizeof(mylimits2[0]);

    lnxproc_limits_print(mylimits2, dim2);

    LNXPROC_LIMITS_T *limits2 = NULL;

    lnxproc_limits_dup(&limits2, mylimits2, dim2);

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

    LNXPROC_LIMITS_FREE(limits2, dim2);

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

    LNXPROC_LIMITS_T *limits3 = NULL;

    lnxproc_limits_dup(&limits3, mylimits3, dim3);

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

    LNXPROC_LIMITS_FREE(limits3, dim3);

}

/*----------------------------------------------------------------------------*/
static LNXPROC_ERROR_T
myvector_print(LNXPROC_VECTOR_DATA_T * val, int recursive, void *data,
               size_t idx)
{
    if (recursive) {
        LNXPROC_VECTOR_T *child = NULL;

        lnxproc_data_child(val, &child);
        printf("Child %p : Index %zd\n", child, idx);
    }
    else {
        char *value = NULL;

        lnxproc_data_value(val, &value);
        printf("Val %p : Index %zd\n", value, idx);
    }
    return 0;
}

static void
test_vector(void)
{
    printf("Allocate vector for structs\n");
    LNXPROC_VECTOR_T *vector = NULL;

    lnxproc_vector_new(&vector, 2, 1);
    lnxproc_vector_print(vector, 1, NULL);

    printf("Allocate vector 1 for strings\n");
    LNXPROC_VECTOR_T *cvector1 = NULL;

    lnxproc_vector_new(&cvector1, 2, 0);
    lnxproc_vector_print(cvector1, 1, NULL);

    printf("Allocate vector 2 for strings\n");
    LNXPROC_VECTOR_T *cvector2 = NULL;

    lnxproc_vector_new(&cvector2, 2, 0);
    lnxproc_vector_print(cvector2, 1, NULL);

    printf("Allocate vector 3 for strings\n");
    LNXPROC_VECTOR_T *cvector3 = NULL;

    lnxproc_vector_new(&cvector3, 2, 0);
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
        LNXPROC_VECTOR_T *myvector = NULL;

        lnxproc_vector_child(vector, 0, &myvector);

        lnxproc_vector_print(myvector, 1, NULL);

        printf
            ("Get string vector from struct vector at position %d (should fail)\n",
             3);
        myvector = NULL;
        lnxproc_vector_child(vector, 3, &myvector);
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

        LNXPROC_VECTOR_FREE(vector);
    }

}

/*----------------------------------------------------------------------------*/
static void
test_array(void)
{
    LNXPROC_ERROR_T ret;
    char buf[64];

    printf("Array 0\n");
    LNXPROC_LIMITS_T limits0[] = {
        {3, "\n", 1},
    };

    LNXPROC_ARRAY_T *array0 = NULL;

    ret = lnxproc_array_new(&array0, limits0, 0);
    printf("Array 0 created : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    char *in = "array0";

    ret = lnxproc_array_set(array0, NULL, 0, in);
    printf("Put value %1$p '%1$s' : %2$s\n", in,
           lnxproc_strerror(ret, buf, sizeof buf));

    char *val = NULL;

    ret = lnxproc_array_get(array0, NULL, 0, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    ret = lnxproc_array_print(array0, 1);
    printf("Print Array 0 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));
    LNXPROC_ARRAY_FREE(array0);

    printf("Array 1\n");
    LNXPROC_LIMITS_T limits1[] = {
        {3, "\n", 1}
        ,
    };
    size_t dim1 = sizeof(limits1) / sizeof(limits1[0]);

    LNXPROC_ARRAY_T *array1 = NULL;

    ret = lnxproc_array_new(&array1, limits1, dim1);
    printf("Array 1 created : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx1[] = { 0 };
    char *in0 = "array1 0";

    ret = lnxproc_array_set(array1, idx1, dim1, in0);
    printf("Put value %1$p '%1$s' : %2$s\n", in0,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx12[] = { 1 };
    char *in2 = "array1 1";

    ret = lnxproc_array_set_last(array1, idx12, dim1, in2);
    printf("Put value %1$p '%1$s' : %2$s\n", in2,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx13[] = { 3 };
    char *in3 = "array1 3";

    ret = lnxproc_array_set_last(array1, idx13, dim1, in3);
    printf("Put value %1$p '%1$s' : %2$s\n", in3,
           lnxproc_strerror(ret, buf, sizeof buf));

    val = NULL;
    ret = lnxproc_array_get(array1, idx1, dim1, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    val = NULL;
    ret = lnxproc_array_get(array1, idx12, dim1, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    val = NULL;
    ret = lnxproc_array_get(array1, idx13, dim1, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    ret = lnxproc_array_print(array1, 1);
    printf("Print Array 1 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));
    LNXPROC_ARRAY_FREE(array1);

    printf("Array 2\n");
    LNXPROC_LIMITS_T limits2[] = {
        {3, "\n", 1}
        ,
        {5, "\t", 1}
        ,
    };
    size_t dim2 = sizeof(limits2) / sizeof(limits2[0]);

    LNXPROC_ARRAY_T *array2 = NULL;

    ret = lnxproc_array_new(&array2, limits2, dim2);
    printf("Array 2 created : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx2[] = { 0, 0 };
    char *in00 = "array2 0 0";

    ret = lnxproc_array_set(array2, idx2, dim2, in00);
    printf("Put value %1$p '%1$s' : %2$s\n", in00,
           lnxproc_strerror(ret, buf, sizeof buf));
    size_t idx21[] = { 0, 1 };
    char *in10 = "array2 1 0";

    ret = lnxproc_array_set_last(array2, idx21, dim2, in10);
    printf("Put value %1$p '%1$s' : %2$s\n", in10,
           lnxproc_strerror(ret, buf, sizeof buf));

    val = NULL;
    ret = lnxproc_array_get(array2, idx2, dim2, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    val = NULL;
    ret = lnxproc_array_get(array2, idx21, dim2, &val);
    printf("Got value %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf));

    ret = lnxproc_array_print(array2, 1);
    printf("Print Array 2 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    LNXPROC_ARRAY_FREE(array2);

    printf("Array 3\n");
    LNXPROC_LIMITS_T limits3[] = {
        {3, "\n", 1}
        ,
        {5, "\t", 1}
        ,
        {5, "", 1}
        ,
    };
    size_t dim3 = sizeof(limits3) / sizeof(limits3[0]);

    LNXPROC_ARRAY_T *array3 = NULL;

    ret = lnxproc_array_new(&array3, limits3, dim3);

    size_t idx3[] = { 0, 0, 0 };
    char *in000 = "array3 0 0 0";

    ret = lnxproc_array_set(array3, idx3, dim3, in000);
    printf("Put value %1$p '%1$s' : %2$s\n", in000,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx31[] = { 0, 0, 1 };
    char *in001 = "array3 0 0 1";

    ret = lnxproc_array_set_last(array3, idx31, dim3, in001);
    printf("Put value %1$p '%1$s' : %2$s\n", in001,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx32[] = { 1, 2, 3 };
    char *in123 = "array3 1 2 3";

    ret = lnxproc_array_set_last(array3, idx32, dim3, in123);
    printf("Put value %1$p '%1$s' : %2$s\n", in123,
           lnxproc_strerror(ret, buf, sizeof buf));

    ret = lnxproc_array_print(array3, 1);
    printf("Print Array 3 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));
    LNXPROC_ARRAY_FREE(array3);

}

/*----------------------------------------------------------------------------*/
static void
execute_base(LNXPROC_BASE_T *base)
{
    if (base) {
        LNXPROC_RESULTS_T *res = lnxproc_base_read(base);

        if (!res) {
            printf("Failure reading proc_cgroups\n");
        }
        else {
            const char *filename = NULL;

            lnxproc_base_filename(base, &filename);
            printf("Filename : %s\n", filename);

            int nbytes = 0;

            lnxproc_base_nbytes(base, &nbytes);

            char *lines = NULL;

            lnxproc_base_lines(base, &lines);
            printf("Data : %1$d %2$*1$s\n", nbytes, lines);

            lnxproc_results_print(res);
            LNXPROC_RESULTS_FREE(res);
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_cgroups(void)
{
    LNXPROC_BASE_T *proc_cgroups = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_cgroups_new(&proc_cgroups);

    if (ret == LNXPROC_OK) {
        printf("Execute 1\n");
        execute_base(proc_cgroups);
        printf("Execute 2\n");
        execute_base(proc_cgroups);
        printf("Execute 3\n");
        execute_base(proc_cgroups);
        printf("Execute 4\n");
        execute_base(proc_cgroups);
        printf("Execute 5\n");
        execute_base(proc_cgroups);
        LNXPROC_BASE_FREE(proc_cgroups);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_domainname(void)
{
    LNXPROC_BASE_T *proc_domainname = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_domainname_new(&proc_domainname);

    if (ret == LNXPROC_OK) {
        printf("Execute 1\n");
        execute_base(proc_domainname);
        printf("Execute 2\n");
        execute_base(proc_domainname);
        printf("Execute 3\n");
        execute_base(proc_domainname);
        printf("Execute 4\n");
        execute_base(proc_domainname);
        printf("Execute 5\n");
        execute_base(proc_domainname);
        LNXPROC_BASE_FREE(proc_domainname);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_hostname(void)
{
    LNXPROC_BASE_T *proc_hostname = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_hostname_new(&proc_hostname);

    if (ret == LNXPROC_OK) {
        printf("Execute 1\n");
        execute_base(proc_hostname);
        printf("Execute 2\n");
        execute_base(proc_hostname);
        printf("Execute 3\n");
        execute_base(proc_hostname);
        printf("Execute 4\n");
        execute_base(proc_hostname);
        printf("Execute 5\n");
        execute_base(proc_hostname);
        LNXPROC_BASE_FREE(proc_hostname);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_osrelease(void)
{
    LNXPROC_BASE_T *proc_osrelease = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_osrelease_new(&proc_osrelease);

    if (ret == LNXPROC_OK) {
        printf("Execute 1\n");
        execute_base(proc_osrelease);
        printf("Execute 2\n");
        execute_base(proc_osrelease);
        printf("Execute 3\n");
        execute_base(proc_osrelease);
        printf("Execute 4\n");
        execute_base(proc_osrelease);
        printf("Execute 5\n");
        execute_base(proc_osrelease);
        LNXPROC_BASE_FREE(proc_osrelease);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_sys_cpufreq(void)
{
    LNXPROC_BASE_T *sys_cpufreq = NULL;
    LNXPROC_ERROR_T ret = lnxproc_sys_cpufreq_new(&sys_cpufreq);

    if (ret == LNXPROC_OK) {
        printf("Execute 1\n");
        execute_base(sys_cpufreq);
        printf("Execute 2\n");
        execute_base(sys_cpufreq);
        printf("Execute 3\n");
        execute_base(sys_cpufreq);
        printf("Execute 4\n");
        execute_base(sys_cpufreq);
        printf("Execute 5\n");
        execute_base(sys_cpufreq);
        LNXPROC_BASE_FREE(sys_cpufreq);
    }
}

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    if (argc < 2) {
        test_error();
        test_results();
        test_vector();
        test_limits();
        test_array();
        test_proc_cgroups();
        test_proc_domainname();
        test_proc_hostname();
        test_proc_osrelease();
        test_sys_cpufreq();
    }
    else if (!strcmp(argv[1], "error")) {
        test_error();
    }
    else if (!strcmp(argv[1], "results")) {
        test_results();
    }
    else if (!strcmp(argv[1], "vector")) {
        test_vector();
    }
    else if (!strcmp(argv[1], "limits")) {
        test_limits();
    }
    else if (!strcmp(argv[1], "array")) {
        test_array();
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_proc_cgroups();
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        test_proc_domainname();
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        test_proc_hostname();
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        test_proc_osrelease();
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        test_sys_cpufreq();
    }
    return 0;
}
