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

#include <sys/types.h>          // getpid()
#include <unistd.h>             // sleep(),getpid()

#include "reference.h"
#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "util_private.h"
#include "vector_private.h"
#include "limits_private.h"
#include "limit_chr.h"
#include "array_private.h"
#include "base_private.h"
#include "interface_private.h"
#include "results_private.h"
#include "modules.h"

/*----------------------------------------------------------------------------*/
static void
test_error(void)
{
    _LNXPROC_ERROR_CHECK();

    int i;

    for (i = 0; i < LNXPROC_ERROR_SIZE; i++) {
        _LNXPROC_ERROR_DEBUG(i, "Test\n");
    }
}

/*----------------------------------------------------------------------------*/
static void
test_util(void)
{
    char buf[32];
    struct timeval tv1 = {.tv_sec = 2,.tv_usec = 12456 };
    printf("Tv1 = %s\n", lnxproc_timeval_print(&tv1, buf, sizeof buf));
    struct timeval tv2 = {.tv_sec = 1,.tv_usec = 32456 };
    printf("Tv2 = %s\n", lnxproc_timeval_print(&tv2, buf, sizeof buf));
    long diff = lnxproc_timeval_diff(&tv1, &tv2);

    printf("Difference = %ld\n", diff);
}

/*----------------------------------------------------------------------------*/
static void
test_results(void)
{
    _LNXPROC_RESULTS_T *results = NULL;

    _lnxproc_results_new(&results, "Test");

    _lnxproc_results_print(results);
    _LNXPROC_RESULTS_FREE(results);

}

/*----------------------------------------------------------------------------*/
static void
test_limits(void)
{
    printf("\nLimits 1\n");
    _LNXPROC_LIMITS_T *mylimits1 = NULL;

    _lnxproc_limits_new(&mylimits1, 1);
    _lnxproc_limits_set(mylimits1, 0, 3, "\n", 1);
    _lnxproc_limits_print(mylimits1);

    _LNXPROC_LIMITS_T *limits1 = NULL;

    //_lnxproc_limits_dup(&limits1, mylimits1);
    limits1 = Acquire(mylimits1, 0);

    int i;

    for (i = 0; i < limits1->dim; i++) {
        char *c = limit_chr(limits1->row + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits1->row + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits1->row + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits1->row + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    _lnxproc_limits_print(limits1);
    _LNXPROC_LIMITS_FREE(mylimits1);
    _LNXPROC_LIMITS_FREE(limits1);

    printf("\nLimits 2\n");
    _LNXPROC_LIMITS_T *mylimits2 = NULL;

    _lnxproc_limits_new(&mylimits2, 2);
    _lnxproc_limits_set(mylimits2, 0, 3, "\n", 1);
    _lnxproc_limits_set(mylimits2, 1, 5, "\t", 1);
    _lnxproc_limits_print(mylimits2);

    _LNXPROC_LIMITS_T *limits2 = NULL;

    //_lnxproc_limits_dup(&limits2, mylimits2);
    limits2 = Acquire(mylimits2, 0);

    for (i = 0; i < limits2->dim; i++) {
        char *c = limit_chr(limits2->row + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits2->row + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits2->row + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits2->row + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    _lnxproc_limits_print(limits2);
    _LNXPROC_LIMITS_FREE(mylimits2);
    _LNXPROC_LIMITS_FREE(limits2);

    printf("\nLimits 3\n");
    _LNXPROC_LIMITS_T *mylimits3 = NULL;

    _lnxproc_limits_new(&mylimits3, 3);
    _lnxproc_limits_set(mylimits3, 0, 3, "\n", 1);
    _lnxproc_limits_set(mylimits3, 1, 5, "\t", 1);
    _lnxproc_limits_set(mylimits3, 2, 5, "", 1);

    _lnxproc_limits_print(mylimits3);

    _LNXPROC_LIMITS_T *limits3 = NULL;

    //_lnxproc_limits_dup(&limits3, mylimits3);
    limits3 = Acquire(mylimits3, 0);

    for (i = 0; i < limits3->dim; i++) {
        char *c = limit_chr(limits3->row + i, ' ');

        printf("Compare ' ' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits3->row + i, '\n');
        printf("Compare '\\n' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits3->row + i, '\0');
        printf("Compare '\\0' to entry %d : Result = %p\n", i, c);
        c = limit_chr(limits3->row + i, '\t');
        printf("Compare '\\t' to entry %d : Result = %p\n", i, c);
    }
    _lnxproc_limits_print(limits3);
    _LNXPROC_LIMITS_FREE(mylimits3);
    _LNXPROC_LIMITS_FREE(limits3);

}

/*----------------------------------------------------------------------------*/
static int
myvector_print(_LNXPROC_VECTOR_T * vector, int idx, int depth, void *data)
{
    printf("Vector %p Idx %d Depth %d\n", vector, idx, depth);
    return 0;
}

static void
test_vector(void)
{
    printf("Allocate vector for structs\n");
    _LNXPROC_VECTOR_T *vector = NULL;

    _lnxproc_vector_new(&vector, 2, 1);
    _lnxproc_vector_print(vector, 1, NULL);

    printf("Allocate vector 1 for strings\n");
    _LNXPROC_VECTOR_T *cvector1 = NULL;

    _lnxproc_vector_new(&cvector1, 2, 0);
    _lnxproc_vector_print(cvector1, 1, NULL);

    printf("Allocate vector 2 for strings\n");
    _LNXPROC_VECTOR_T *cvector2 = NULL;

    _lnxproc_vector_new(&cvector2, 2, 0);
    _lnxproc_vector_print(cvector2, 1, NULL);

    printf("Allocate vector 3 for strings\n");
    _LNXPROC_VECTOR_T *cvector3 = NULL;

    _lnxproc_vector_new(&cvector3, 2, 0);
    _lnxproc_vector_print(cvector3, 1, NULL);

    if (vector && cvector1 && cvector2 && cvector3) {

        printf("Add string vector to struct vector at position %d\n", 0);
        _lnxproc_vector_set_child(vector, 0, cvector1);
        _lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should fail)\n",
             2);
        _lnxproc_vector_set_child(vector, 2, cvector2);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("Add string vector to struct vector at position %d\n", 1);
        _lnxproc_vector_set_child(vector, 1, cvector2);
        _lnxproc_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should resize)\n",
             2);
        _lnxproc_vector_set_child(vector, 2, cvector3);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("Get string vector from struct vector at position %d\n", 0);
        _LNXPROC_VECTOR_T *myvector = NULL;

        _lnxproc_vector_child(vector, 0, &myvector);

        _lnxproc_vector_print(myvector, 1, NULL);

        printf
            ("Get string vector from struct vector at position %d (should fail)\n",
             3);
        myvector = NULL;
        _lnxproc_vector_child(vector, 3, &myvector);
        _lnxproc_vector_print(myvector, 1, NULL);

        printf("Resize struct vector - add %d elements\n", 20);
        _lnxproc_vector_resize(vector, 20);
        _lnxproc_vector_print(vector, 1, NULL);

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
        _lnxproc_vector_set_value(cvector1, 0, val2);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 0, val1);
        _lnxproc_vector_set_value(cvector1, 0, val1);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 1, val4);
        _lnxproc_vector_set_value(cvector1, 1, val4);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        _lnxproc_vector_set_value(cvector1, 2, val7);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        _lnxproc_vector_set_value(cvector2, 0, val2);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 1, val5);
        _lnxproc_vector_set_value(cvector2, 1, val5);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        _lnxproc_vector_set_value(cvector2, 2, val7);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 0, val3);
        _lnxproc_vector_set_value(cvector3, 0, val3);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 1, val6);
        _lnxproc_vector_set_value(cvector3, 1, val6);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 2, val9);
        _lnxproc_vector_set_value(cvector3, 2, val9);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("cvector2: Resize string vector - add %d elements\n", 20);
        _lnxproc_vector_resize(cvector2, 20);
        _lnxproc_vector_print(vector, 1, NULL);

        printf("Print elements %d to %d\n", 1, 2);
        _lnxproc_vector_iterate(vector, 0, 0, 0, NULL, myvector_print);
        printf("\n");

        _LNXPROC_VECTOR_FREE(vector);
    }

}

/*----------------------------------------------------------------------------*/
static void
test_array(void)
{
    int ret;
    char buf[64];

    printf("Array 1\n");
    _LNXPROC_LIMITS_T *limits1 = NULL;

    _lnxproc_limits_new(&limits1, 1);
    _lnxproc_limits_set(limits1, 0, 3, "\n", 1);

    _LNXPROC_ARRAY_T *array1 = NULL;

    ret = _lnxproc_array_new(&array1, limits1);
    printf("Array 1 created : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx1[] = { 0 };
    char *in0 = "array1 0";

    ret = _lnxproc_array_set(array1, idx1, limits1->dim, in0);
    printf("Put value %1$p '%1$s' : %2$s\n", in0,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx12[] = { 1 };
    char *in2 = "array1 1";

    ret = _lnxproc_array_set_last(array1, idx12, limits1->dim, in2);
    printf("Put value %1$p '%1$s' : %2$s\n", in2,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx13[] = { 3 };
    char *in3 = "array1 3";

    ret = _lnxproc_array_set_last(array1, idx13, limits1->dim, in3);
    printf("Put value %1$p '%1$s' : %2$s\n", in3,
           lnxproc_strerror(ret, buf, sizeof buf));

    char **values1 = (char **) array1->vector->values;

    printf("Got values %1$p\n", values1);

    char *val = NULL;

    ret = _lnxproc_array_get(array1, idx1, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx1[0]);
    val = values1[idx1[0]];
    printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx1[0]);

    val = NULL;
    ret = _lnxproc_array_get(array1, idx12, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx12[0]);
    val = values1[idx12[0]];
    printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx12[0]);

    val = NULL;
    ret = _lnxproc_array_get(array1, idx13, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx13[0]);
    //val = values1[idx13[0]];
    //printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx13[0]);

    ret = _lnxproc_array_print(array1, 1);
    printf("Print Array 1 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));
    _LNXPROC_ARRAY_FREE(array1);
    _LNXPROC_LIMITS_FREE(limits1);

    printf("Array 2\n");
    _LNXPROC_LIMITS_T *limits2 = NULL;

    _lnxproc_limits_new(&limits2, 2);
    _lnxproc_limits_set(limits2, 0, 3, "\n", 1);
    _lnxproc_limits_set(limits2, 1, 5, "\t", 1);

    _LNXPROC_ARRAY_T *array2 = NULL;

    ret = _lnxproc_array_new(&array2, limits2);
    printf("Array 2 created : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx2[] = { 0, 0 };
    char *in00 = "array2 0 0";

    ret = _lnxproc_array_set(array2, idx2, limits2->dim, in00);
    printf("Put value %1$p '%1$s' : %2$s\n", in00,
           lnxproc_strerror(ret, buf, sizeof buf));
    size_t idx21[] = { 0, 1 };
    char *in10 = "array2 1 0";

    ret = _lnxproc_array_set_last(array2, idx21, limits2->dim, in10);
    printf("Put value %1$p '%1$s' : %2$s\n", in10,
           lnxproc_strerror(ret, buf, sizeof buf));

    char ***values2 = (char ***) array2->vector->values;

    printf("Got values %1$p\n", values2);

    val = NULL;
    ret = _lnxproc_array_get(array2, idx2, limits2->dim, &val);
    printf("Got value %3$zd,%4$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx2[0], idx2[1]);
    val = values2[idx2[0]][idx2[1]];
    printf("Direct value %2$zd,%3$zd -> %1$p '%1$s'\n", val, idx2[0], idx2[1]);

    val = NULL;
    ret = _lnxproc_array_get(array2, idx21, limits2->dim, &val);
    printf("Got value %3$zd,%4$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx21[0], idx21[1]);
    val = values2[idx21[0]][idx21[1]];
    printf("Direct value %2$zd,%3$zd -> %1$p '%1$s'\n", val, idx21[0],
           idx21[1]);

    ret = _lnxproc_array_print(array2, 1);
    printf("Print Array 2 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));

    _LNXPROC_ARRAY_FREE(array2);
    _LNXPROC_LIMITS_FREE(limits2);

    printf("Array 3\n");
    _LNXPROC_LIMITS_T *limits3 = NULL;

    _lnxproc_limits_new(&limits3, 3);
    _lnxproc_limits_set(limits3, 0, 3, "\n", 1);
    _lnxproc_limits_set(limits3, 1, 5, "\t", 1);
    _lnxproc_limits_set(limits3, 2, 5, "", 1);

    _LNXPROC_ARRAY_T *array3 = NULL;

    ret = _lnxproc_array_new(&array3, limits3);

    size_t idx3[] = { 0, 0, 0 };
    char *in000 = "array3 0 0 0";

    ret = _lnxproc_array_set(array3, idx3, limits3->dim, in000);
    printf("Put value %1$p '%1$s' : %2$s\n", in000,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx31[] = { 0, 0, 1 };
    char *in001 = "array3 0 0 1";

    ret = _lnxproc_array_set_last(array3, idx31, limits3->dim, in001);
    printf("Put value %1$p '%1$s' : %2$s\n", in001,
           lnxproc_strerror(ret, buf, sizeof buf));

    size_t idx32[] = { 1, 2, 3 };
    char *in123 = "array3 1 2 3";

    ret = _lnxproc_array_set_last(array3, idx32, limits3->dim, in123);
    printf("Put value %1$p '%1$s' : %2$s\n", in123,
           lnxproc_strerror(ret, buf, sizeof buf));

    char ****values3 = (char ****) array3->vector->values;

    printf("Got values %1$p\n", values3);

    val = NULL;
    ret = _lnxproc_array_get(array3, idx3, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx3[0], idx3[1], idx3[2]);
    val = values3[idx3[0]][idx3[1]][idx3[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx3[0],
           idx3[1], idx3[2]);

    val = NULL;
    ret = _lnxproc_array_get(array3, idx31, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx31[0], idx31[1],
           idx31[2]);
    val = values3[idx31[0]][idx31[1]][idx31[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx31[0],
           idx31[1], idx31[2]);

    val = NULL;
    ret = _lnxproc_array_get(array3, idx32, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           lnxproc_strerror(ret, buf, sizeof buf), idx32[0], idx32[1],
           idx32[2]);
    val = values3[idx32[0]][idx32[1]][idx32[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx32[0],
           idx32[1], idx32[2]);

    ret = _lnxproc_array_print(array3, 1);
    printf("Print Array 3 : %s\n", lnxproc_strerror(ret, buf, sizeof buf));
    _LNXPROC_ARRAY_FREE(array3);
    _LNXPROC_LIMITS_FREE(limits3);

}

/*----------------------------------------------------------------------------*/
static int
interface_func(char *mod, char *key, char *value, void *data)
{
    printf("Mod %s key %s value %s data %s\n", mod, key, value, (char *) data);
    return LNXPROC_OK;
}

static void
test_interface(void)
{

    char errbuf[64];
    char buf[32];
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 0);
    lnxproc_read(modules);
    lnxproc_read(modules);
    lnxproc_read(modules);
    int ret = lnxproc_fetch(modules, LNXPROC_PROC_CGROUPS, "hierarchy", buf,
                            sizeof buf);

    printf("PROC_CGROUPS key '%s' value '%s' Error '%s'\n", "hierarchy", buf,
           lnxproc_strerror(ret, errbuf, sizeof errbuf));
    ret =
        lnxproc_fetch(modules, LNXPROC_PROC_CGROUPS, "/cpuset/hierarchy", buf,
                      sizeof buf);
    printf("PROC_CGROUPS key '%s' value '%s' Error '%s'\n", "/cpuset/hierarchy",
           buf, lnxproc_strerror(ret, errbuf, sizeof errbuf));
    lnxproc_print(modules);
    lnxproc_iterate(modules, interface_func, "All");
    LNXPROC_FREE(modules);

    int2str(getpid(), buf, sizeof buf);

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_PID_STAT, buf, 1 + strlen(buf));
    lnxproc_read(modules);
    lnxproc_read(modules);
    lnxproc_read(modules);
    lnxproc_print(modules);
    lnxproc_iterate(modules, interface_func, "PID_STAT");
    LNXPROC_FREE(modules);

    strlcpy(buf, "sd*", sizeof buf);
    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, buf, 1 + strlen(buf));
    lnxproc_read(modules);
    lnxproc_read(modules);
    lnxproc_read(modules);
    lnxproc_print(modules);
    lnxproc_iterate(modules, interface_func, "SYS_DISKSECTORS");
    LNXPROC_FREE(modules);
}

/*----------------------------------------------------------------------------*/
#define TEST_MODULE(type,buf,len) do {\
    char errbuf[128]; \
    LNXPROC_MODULE_T *modules = NULL; \
    int ret = lnxproc_new(&modules, 1); \
    if( ret ) { \
        printf(#type " Error '%s'\n", lnxproc_strerror(ret, errbuf, sizeof errbuf)); \
    } \
    else { \
        ret = lnxproc_set(modules, 0, (type), (buf), (len)); \
        if( ret ) { \
            printf(#type " Error '%s'\n", lnxproc_strerror(ret, errbuf, sizeof errbuf)); \
        } \
        else { \
            int i; \
            for (i = 0; i < 5; i++) { \
                ret = lnxproc_read(modules); \
                if( ret ) { \
                    printf(#type " Error '%s'\n", lnxproc_strerror(ret, errbuf, sizeof errbuf)); \
                } \
            } \
            lnxproc_print(modules); \
        } \
        LNXPROC_FREE(modules); \
    } \
} while(0)

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    char buf[16];

    int2str(getpid(), buf, sizeof buf);

    if (argc < 2) {
        test_error();
        test_util();
        test_results();
        test_vector();
        test_limits();
        test_array();
        test_interface();
        TEST_MODULE(LNXPROC_PROC_BUDDYINFO, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_CGROUPS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_CMDLINE, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_CPUINFO, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_DISKSTATS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_DOMAINNAME, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_HOSTNAME, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_INTERRUPTS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_LOADAVG, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_MEMINFO, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_MOUNTS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_OSRELEASE, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PARTITIONS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON, buf, 1 + strlen(buf));
        TEST_MODULE(LNXPROC_PROC_PID_IO, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_IO, buf, 1 + strlen(buf));
        TEST_MODULE(LNXPROC_PROC_PID_STAT, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_STAT, buf, 1 + strlen(buf));
        TEST_MODULE(LNXPROC_PROC_SOFTIRQS, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_STAT, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_UPTIME, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_VMSTAT, NULL, 0);
        TEST_MODULE(LNXPROC_SYS_CPUFREQ, NULL, 0);
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS, NULL, 0);
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS, "sd*", 4);
    }
    else if (!strcmp(argv[1], "util")) {
        test_util();
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
    else if (!strcmp(argv[1], "interface")) {
        test_interface();
    }
    else if (!strcmp(argv[1], "proc_buddyinfo")) {
        TEST_MODULE(LNXPROC_PROC_BUDDYINFO, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        TEST_MODULE(LNXPROC_PROC_CGROUPS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_cmdline")) {
        TEST_MODULE(LNXPROC_PROC_CMDLINE, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_cpuinfo")) {
        TEST_MODULE(LNXPROC_PROC_CPUINFO, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        TEST_MODULE(LNXPROC_PROC_DISKSTATS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        TEST_MODULE(LNXPROC_PROC_DOMAINNAME, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        TEST_MODULE(LNXPROC_PROC_HOSTNAME, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_interrupts")) {
        TEST_MODULE(LNXPROC_PROC_INTERRUPTS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_loadavg")) {
        TEST_MODULE(LNXPROC_PROC_LOADAVG, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_meminfo")) {
        TEST_MODULE(LNXPROC_PROC_MEMINFO, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_mounts")) {
        TEST_MODULE(LNXPROC_PROC_MOUNTS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        TEST_MODULE(LNXPROC_PROC_OSRELEASE, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_partitions")) {
        TEST_MODULE(LNXPROC_PROC_PARTITIONS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_pid_environ")) {
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON, buf, 1 + strlen(buf));
    }
    else if (!strcmp(argv[1], "proc_pid_io")) {
        TEST_MODULE(LNXPROC_PROC_PID_IO, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_IO, buf, 1 + strlen(buf));
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {
        TEST_MODULE(LNXPROC_PROC_PID_STAT, NULL, 0);
        TEST_MODULE(LNXPROC_PROC_PID_STAT, buf, 1 + strlen(buf));
    }
    else if (!strcmp(argv[1], "proc_softirqs")) {
        TEST_MODULE(LNXPROC_PROC_SOFTIRQS, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_stat")) {
        TEST_MODULE(LNXPROC_PROC_STAT, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_uptime")) {
        TEST_MODULE(LNXPROC_PROC_UPTIME, NULL, 0);
    }
    else if (!strcmp(argv[1], "proc_vmstat")) {
        TEST_MODULE(LNXPROC_PROC_VMSTAT, NULL, 0);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        TEST_MODULE(LNXPROC_SYS_CPUFREQ, NULL, 0);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS, NULL, 0);
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS, "sd*", 4);
    }
    return 0;
}
