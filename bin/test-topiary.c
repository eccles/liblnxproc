/*
 *  'topiary' - gather stats on linux performance
 *  Copyright (C) 2013  Paul Hewlett phewlett76@gmail.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

#include "config.h"

#include <sys/types.h>          // getpid()
#include <unistd.h>             // sleep(),getpid()

#include "reference.h"
#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "print_private.h"
#include "util_private.h"
#include "vector_private.h"
#include "limits_private.h"
#include "limit_chr.h"
#include "array_private.h"
#include "base_private.h"
#include "interface_private.h"
#include "results_private.h"
#include "opt_private.h"
#include "interface_private.h"
#include "modules.h"

static char pid[16];

/*----------------------------------------------------------------------------*/
static void
test_error(void)
{
    _TOPIARY_ERROR_CHECK();

    int i;

    for (i = 0; i < TOPIARY_ERROR_SIZE; i++) {
        _TOPIARY_ERROR_DEBUG(i, "Test\n");
    }
    //_TOPIARY_ERROR_DEBUG(i, "Test\n");
}

/*----------------------------------------------------------------------------*/
static void
test_util(void)
{
    char buf[32];
    struct timeval tv1 = {.tv_sec = 2,.tv_usec = 12456 };
    printf("Tv1 = %s\n", topiary_timeval_print(&tv1, buf, sizeof buf));
    struct timeval tv2 = {.tv_sec = 1,.tv_usec = 32456 };
    printf("Tv2 = %s\n", topiary_timeval_print(&tv2, buf, sizeof buf));
    long diff = topiary_timeval_diff(&tv1, &tv2);

    printf("Difference = %ld\n", diff);
}

/*----------------------------------------------------------------------------*/
static void
test_results(void)
{
    _TOPIARY_RESULTS_T *results = NULL;

    _topiary_results_new(&results, "Test");

    _topiary_results_print(results, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    _TOPIARY_RESULTS_FREE(results);

}

/*----------------------------------------------------------------------------*/
static void
test_limits(void)
{
    printf("\nLimits 1\n");
    _TOPIARY_LIMITS_T *mylimits1 = NULL;

    _topiary_limits_new(&mylimits1, 1);
    _topiary_limits_set(mylimits1, 0, 3, "\n", 1);
    _topiary_limits_print(mylimits1);

    _TOPIARY_LIMITS_T *limits1 = NULL;

    //_topiary_limits_dup(&limits1, mylimits1);
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
    _topiary_limits_print(limits1);
    _TOPIARY_LIMITS_FREE(mylimits1);
    _TOPIARY_LIMITS_FREE(limits1);

    printf("\nLimits 2\n");
    _TOPIARY_LIMITS_T *mylimits2 = NULL;

    _topiary_limits_new(&mylimits2, 2);
    _topiary_limits_set(mylimits2, 0, 3, "\n", 1);
    _topiary_limits_set(mylimits2, 1, 5, "\t", 1);
    _topiary_limits_print(mylimits2);

    _TOPIARY_LIMITS_T *limits2 = NULL;

    //_topiary_limits_dup(&limits2, mylimits2);
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
    _topiary_limits_print(limits2);
    _TOPIARY_LIMITS_FREE(mylimits2);
    _TOPIARY_LIMITS_FREE(limits2);

    printf("\nLimits 3\n");
    _TOPIARY_LIMITS_T *mylimits3 = NULL;

    _topiary_limits_new(&mylimits3, 3);
    _topiary_limits_set(mylimits3, 0, 3, "\n", 1);
    _topiary_limits_set(mylimits3, 1, 5, "\t", 1);
    _topiary_limits_set(mylimits3, 2, 5, "", 1);

    _topiary_limits_print(mylimits3);

    _TOPIARY_LIMITS_T *limits3 = NULL;

    //_topiary_limits_dup(&limits3, mylimits3);
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
    _topiary_limits_print(limits3);
    _TOPIARY_LIMITS_FREE(mylimits3);
    _TOPIARY_LIMITS_FREE(limits3);

}

/*----------------------------------------------------------------------------*/
static int
myvector_print(_TOPIARY_VECTOR_T *vector, int idx, int depth, void *data)
{
    printf("Vector %p Idx %d Depth %d\n", vector, idx, depth);
    return 0;
}

static void
test_vector(void)
{
    printf("Allocate vector for structs\n");
    _TOPIARY_VECTOR_T *vector = NULL;

    _topiary_vector_new(&vector, 2, 1);
    _topiary_vector_print(vector, 1, NULL);

    printf("Allocate vector 1 for strings\n");
    _TOPIARY_VECTOR_T *cvector1 = NULL;

    _topiary_vector_new(&cvector1, 2, 0);
    _topiary_vector_print(cvector1, 1, NULL);

    printf("Allocate vector 2 for strings\n");
    _TOPIARY_VECTOR_T *cvector2 = NULL;

    _topiary_vector_new(&cvector2, 2, 0);
    _topiary_vector_print(cvector2, 1, NULL);

    printf("Allocate vector 3 for strings\n");
    _TOPIARY_VECTOR_T *cvector3 = NULL;

    _topiary_vector_new(&cvector3, 2, 0);
    _topiary_vector_print(cvector3, 1, NULL);

    if (vector && cvector1 && cvector2 && cvector3) {

        printf("Add string vector to struct vector at position %d\n", 0);
        _topiary_vector_set_child(vector, 0, cvector1);
        _topiary_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should fail)\n",
             2);
        _topiary_vector_set_child(vector, 2, cvector2);
        _topiary_vector_print(vector, 1, NULL);

        printf("Add string vector to struct vector at position %d\n", 1);
        _topiary_vector_set_child(vector, 1, cvector2);
        _topiary_vector_print(vector, 1, NULL);

        printf
            ("Add string vector to struct vector at position %d (should resize)\n",
             2);
        _topiary_vector_set_child(vector, 2, cvector3);
        _topiary_vector_print(vector, 1, NULL);

        printf("Get string vector from struct vector at position %d\n", 0);
        _TOPIARY_VECTOR_T *myvector = NULL;

        _topiary_vector_child(vector, 0, &myvector);

        _topiary_vector_print(myvector, 1, NULL);

        printf
            ("Get string vector from struct vector at position %d (should fail)\n",
             3);
        myvector = NULL;
        _topiary_vector_child(vector, 3, &myvector);
        _topiary_vector_print(myvector, 1, NULL);

        printf("Resize struct vector - add %d elements\n", 20);
        _topiary_vector_resize(vector, 20);
        _topiary_vector_print(vector, 1, NULL);

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
        _topiary_vector_set_value(cvector1, 0, val2);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 0, val1);
        _topiary_vector_set_value(cvector1, 0, val1);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 1, val4);
        _topiary_vector_set_value(cvector1, 1, val4);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector1: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        _topiary_vector_set_value(cvector1, 2, val7);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 0, val2);
        _topiary_vector_set_value(cvector2, 0, val2);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 1, val5);
        _topiary_vector_set_value(cvector2, 1, val5);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector2: Set idx %1$d to value %2$p '%2$s'\n", 2, val7);
        _topiary_vector_set_value(cvector2, 2, val7);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 0, val3);
        _topiary_vector_set_value(cvector3, 0, val3);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 1, val6);
        _topiary_vector_set_value(cvector3, 1, val6);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector3: Set idx %1$d to value %2$p '%2$s'\n", 2, val9);
        _topiary_vector_set_value(cvector3, 2, val9);
        _topiary_vector_print(vector, 1, NULL);

        printf("cvector2: Resize string vector - add %d elements\n", 20);
        _topiary_vector_resize(cvector2, 20);
        _topiary_vector_print(vector, 1, NULL);

        printf("Print elements %d to %d\n", 1, 2);
        _topiary_vector_iterate(vector, 0, 0, 0, NULL, myvector_print);
        printf("\n");

        _TOPIARY_VECTOR_FREE(vector);
    }

}

/*----------------------------------------------------------------------------*/
static void
test_array(void)
{
    int ret;
    char buf[64];

    printf("Array 1\n");
    _TOPIARY_LIMITS_T *limits1 = NULL;

    _topiary_limits_new(&limits1, 1);
    _topiary_limits_set(limits1, 0, 3, "\n", 1);

    _TOPIARY_ARRAY_T *array1 = NULL;

    ret = _topiary_array_new(&array1, limits1);
    printf("Array 1 created : %s\n", topiary_strerror(ret, buf, sizeof buf));

    size_t idx1[] = { 0 };
    char *in0 = "array1 0";

    ret = _topiary_array_set(array1, idx1, limits1->dim, in0);
    printf("Put value %1$p '%1$s' : %2$s\n", in0,
           topiary_strerror(ret, buf, sizeof buf));

    size_t idx12[] = { 1 };
    char *in2 = "array1 1";

    ret = _topiary_array_set_last(array1, idx12, limits1->dim, in2);
    printf("Put value %1$p '%1$s' : %2$s\n", in2,
           topiary_strerror(ret, buf, sizeof buf));

    size_t idx13[] = { 3 };
    char *in3 = "array1 3";

    ret = _topiary_array_set_last(array1, idx13, limits1->dim, in3);
    printf("Put value %1$p '%1$s' : %2$s\n", in3,
           topiary_strerror(ret, buf, sizeof buf));

    char **values1 = (char **) array1->vector->values;

    printf("Got values %1$p\n", values1);

    char *val = NULL;

    ret = _topiary_array_get(array1, idx1, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx1[0]);
    val = values1[idx1[0]];
    printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx1[0]);

    val = NULL;
    ret = _topiary_array_get(array1, idx12, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx12[0]);
    val = values1[idx12[0]];
    printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx12[0]);

    val = NULL;
    ret = _topiary_array_get(array1, idx13, limits1->dim, &val);
    printf("Got value %3$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx13[0]);
    //val = values1[idx13[0]];
    //printf("Direct value %2$zd -> %1$p '%1$s'\n", val, idx13[0]);

    ret = _topiary_array_print(array1, 1);
    printf("Print Array 1 : %s\n", topiary_strerror(ret, buf, sizeof buf));
    _TOPIARY_ARRAY_FREE(array1);
    _TOPIARY_LIMITS_FREE(limits1);

    printf("Array 2\n");
    _TOPIARY_LIMITS_T *limits2 = NULL;

    _topiary_limits_new(&limits2, 2);
    _topiary_limits_set(limits2, 0, 3, "\n", 1);
    _topiary_limits_set(limits2, 1, 5, "\t", 1);

    _TOPIARY_ARRAY_T *array2 = NULL;

    ret = _topiary_array_new(&array2, limits2);
    printf("Array 2 created : %s\n", topiary_strerror(ret, buf, sizeof buf));

    size_t idx2[] = { 0, 0 };
    char *in00 = "array2 0 0";

    ret = _topiary_array_set(array2, idx2, limits2->dim, in00);
    printf("Put value %1$p '%1$s' : %2$s\n", in00,
           topiary_strerror(ret, buf, sizeof buf));
    size_t idx21[] = { 0, 1 };
    char *in10 = "array2 1 0";

    ret = _topiary_array_set_last(array2, idx21, limits2->dim, in10);
    printf("Put value %1$p '%1$s' : %2$s\n", in10,
           topiary_strerror(ret, buf, sizeof buf));

    char ***values2 = (char ***) array2->vector->values;

    printf("Got values %1$p\n", values2);

    val = NULL;
    ret = _topiary_array_get(array2, idx2, limits2->dim, &val);
    printf("Got value %3$zd,%4$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx2[0], idx2[1]);
    val = values2[idx2[0]][idx2[1]];
    printf("Direct value %2$zd,%3$zd -> %1$p '%1$s'\n", val, idx2[0], idx2[1]);

    val = NULL;
    ret = _topiary_array_get(array2, idx21, limits2->dim, &val);
    printf("Got value %3$zd,%4$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx21[0], idx21[1]);
    val = values2[idx21[0]][idx21[1]];
    printf("Direct value %2$zd,%3$zd -> %1$p '%1$s'\n", val, idx21[0],
           idx21[1]);

    ret = _topiary_array_print(array2, 1);
    printf("Print Array 2 : %s\n", topiary_strerror(ret, buf, sizeof buf));

    _TOPIARY_ARRAY_FREE(array2);
    _TOPIARY_LIMITS_FREE(limits2);

    printf("Array 3\n");
    _TOPIARY_LIMITS_T *limits3 = NULL;

    _topiary_limits_new(&limits3, 3);
    _topiary_limits_set(limits3, 0, 3, "\n", 1);
    _topiary_limits_set(limits3, 1, 5, "\t", 1);
    _topiary_limits_set(limits3, 2, 5, "", 1);

    _TOPIARY_ARRAY_T *array3 = NULL;

    ret = _topiary_array_new(&array3, limits3);

    size_t idx3[] = { 0, 0, 0 };
    char *in000 = "array3 0 0 0";

    ret = _topiary_array_set(array3, idx3, limits3->dim, in000);
    printf("Put value %1$p '%1$s' : %2$s\n", in000,
           topiary_strerror(ret, buf, sizeof buf));

    size_t idx31[] = { 0, 0, 1 };
    char *in001 = "array3 0 0 1";

    ret = _topiary_array_set_last(array3, idx31, limits3->dim, in001);
    printf("Put value %1$p '%1$s' : %2$s\n", in001,
           topiary_strerror(ret, buf, sizeof buf));

    size_t idx32[] = { 1, 2, 3 };
    char *in123 = "array3 1 2 3";

    ret = _topiary_array_set_last(array3, idx32, limits3->dim, in123);
    printf("Put value %1$p '%1$s' : %2$s\n", in123,
           topiary_strerror(ret, buf, sizeof buf));

    char ****values3 = (char ****) array3->vector->values;

    printf("Got values %1$p\n", values3);

    val = NULL;
    ret = _topiary_array_get(array3, idx3, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx3[0], idx3[1], idx3[2]);
    val = values3[idx3[0]][idx3[1]][idx3[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx3[0],
           idx3[1], idx3[2]);

    val = NULL;
    ret = _topiary_array_get(array3, idx31, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx31[0], idx31[1],
           idx31[2]);
    val = values3[idx31[0]][idx31[1]][idx31[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx31[0],
           idx31[1], idx31[2]);

    val = NULL;
    ret = _topiary_array_get(array3, idx32, limits3->dim, &val);
    printf("Got value %3$zd,%4$zd,%5$zd -> %1$p '%1$s' : %2$s\n", val,
           topiary_strerror(ret, buf, sizeof buf), idx32[0], idx32[1],
           idx32[2]);
    val = values3[idx32[0]][idx32[1]][idx32[2]];
    printf("Direct value %2$zd,%3$zd,%4$zd -> %1$p '%1$s'\n", val, idx32[0],
           idx32[1], idx32[2]);

    ret = _topiary_array_print(array3, 1);
    printf("Print Array 3 : %s\n", topiary_strerror(ret, buf, sizeof buf));
    _TOPIARY_ARRAY_FREE(array3);
    _TOPIARY_LIMITS_FREE(limits3);

}

/*----------------------------------------------------------------------------*/
static int
interface_func(char *mod, char *key, char *value, void *data)
{
    printf("Mod %s key %s value %s data %s\n", mod, key, value, (char *) data);
    return TOPIARY_OK;
}

static void
test_all(void)
{
    TOPIARY_MODULE_T *modules = NULL;

    char errbuf[64];
    char buf[32];
    char *pbuf;

    topiary_new(&modules, 0);
    topiary_read(modules);
    topiary_read(modules);
    topiary_read(modules);
    int ret = topiary_fetch(modules, TOPIARY_PROC_CGROUPS, "hierarchy", buf,
                            sizeof buf, &pbuf);

    printf("PROC_CGROUPS key '%s' value '%s' Error '%s'\n", "hierarchy", pbuf,
           topiary_strerror(ret, errbuf, sizeof errbuf));
    ret =
        topiary_fetch(modules, TOPIARY_PROC_CGROUPS, "/cpuset/hierarchy", buf,
                      sizeof buf, &pbuf);
    printf("PROC_CGROUPS key '%s' value '%s' Error '%s'\n", "/cpuset/hierarchy",
           pbuf, topiary_strerror(ret, errbuf, sizeof errbuf));
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
    topiary_iterate(modules, interface_func, "All");
    topiary_free(&modules);

}

static void
test_faulty(void)
{
    TOPIARY_MODULE_T *modules = NULL;

    topiary_new(&modules, 2);
    topiary_set(modules, 0, TOPIARY_SYS_CPUFREQ, NULL);
    topiary_set(modules, 1, TOPIARY_PROC_NET_SNMP, NULL);
    topiary_read(modules);
    topiary_read(modules);
    topiary_read(modules);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
    topiary_free(&modules);

}

static void
test_interface(void)
{

    TOPIARY_MODULE_T *modules = NULL;
    TOPIARY_OPT_T *opt = NULL;

    test_all();

    topiary_opt_new(&opt);
    topiary_opt_set_fileglob(opt, pid);

    topiary_new(&modules, 1);
    topiary_set(modules, 0, TOPIARY_PROC_PID_STAT, opt);
    topiary_opt_free(&opt);

    topiary_read(modules);
    topiary_read(modules);
    topiary_read(modules);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
    topiary_iterate(modules, interface_func, "PID_STAT");
    topiary_free(&modules);

    topiary_opt_new(&opt);
    topiary_opt_set_master(opt, "chrome");
    topiary_new(&modules, 1);
    topiary_set(modules, 0, TOPIARY_PROC_PID_STAT, opt);
    topiary_opt_free(&opt);

    topiary_read(modules);
    topiary_read(modules);
    topiary_read(modules);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
    topiary_iterate(modules, interface_func, "PID_STAT");
    topiary_free(&modules);

    topiary_opt_new(&opt);
    topiary_opt_set_fileglob(opt, "sd*");
    topiary_new(&modules, 1);
    topiary_set(modules, 0, TOPIARY_SYS_DISKSECTORS, opt);
    topiary_opt_free(&opt);

    topiary_read(modules);
    topiary_read(modules);
    topiary_read(modules);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
    topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
    topiary_iterate(modules, interface_func, "SYS_DISKSECTORS");
    topiary_free(&modules);
}

/*----------------------------------------------------------------------------*/
void
test_module(TOPIARY_MODULE_TYPE_T type, TOPIARY_OPT_T *opt)
{
    char errbuf[128];
    TOPIARY_MODULE_T *modules = NULL;
    int ret = topiary_new(&modules, 1);

    if (ret) {
        printf("Error '%s'\n", topiary_strerror(ret, errbuf, sizeof errbuf));
    }
    else {
        const char *tag;

        topiary_tag(modules, 0, &tag);
        ret = topiary_set(modules, 0, type, opt);
        topiary_opt_free(&opt);
        if (ret) {
            printf("%s: Error '%s'\n", tag,
                   topiary_strerror(ret, errbuf, sizeof errbuf));
        }
        else {
            int i;

            for (i = 0; i < 5; i++) {
                ret = topiary_read(modules);
                if (ret) {
                    printf("%s: Error '%s'\n", tag,
                           topiary_strerror(ret, errbuf, sizeof errbuf));
                }
                //sleep(10);
            }
            topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_JSON);
            topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
        }
        topiary_free(&modules);
    }
}

void
test_glob_module(TOPIARY_MODULE_TYPE_T type, char *glob)
{
    TOPIARY_OPT_T *opt = NULL;

    topiary_opt_new(&opt);
    topiary_opt_set_fileglob(opt, glob);
    test_module(type, opt);
}

void
test_master_module(TOPIARY_MODULE_TYPE_T type, char *task)
{
    TOPIARY_OPT_T *opt = NULL;

    topiary_opt_new(&opt);
    topiary_opt_set_master(opt, task);
    test_module(type, opt);
}

void
test_mod_module(TOPIARY_MODULE_TYPE_T type, char *task,
                TOPIARY_MODULE_TYPE_T subtype)
{
    TOPIARY_MODULE_T *sub = NULL;

    topiary_new(&sub, 1);
    topiary_set(sub, 0, subtype, NULL);

    TOPIARY_OPT_T *opt = NULL;

    topiary_opt_new(&opt);
    topiary_opt_set_master(opt, task);
    topiary_opt_set_module(opt, sub);
    topiary_free(&sub);

    test_module(type, opt);
}

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    int2str(getpid(), pid, sizeof pid);

    if (argc < 2) {
        test_error();
        test_util();
        test_results();
        test_vector();
        test_limits();
        test_array();
        test_interface();
        test_module(TOPIARY_NTP_DRIFT, NULL);
        test_module(TOPIARY_PROC_BUDDYINFO, NULL);
        test_module(TOPIARY_PROC_CGROUPS, NULL);
        test_module(TOPIARY_PROC_CMDLINE, NULL);
        test_module(TOPIARY_PROC_CPUINFO, NULL);
        test_module(TOPIARY_PROC_DISKSTATS, NULL);
        test_module(TOPIARY_PROC_DOMAINNAME, NULL);
        test_module(TOPIARY_PROC_HOSTNAME, NULL);
        test_module(TOPIARY_PROC_INTERRUPTS, NULL);
        test_module(TOPIARY_PROC_LOADAVG, NULL);
        test_module(TOPIARY_PROC_MEMINFO, NULL);
        test_module(TOPIARY_PROC_MOUNTS, NULL);
        test_module(TOPIARY_PROC_NET_DEV, NULL);
        test_module(TOPIARY_PROC_NET_NETSTAT, NULL);
        test_module(TOPIARY_PROC_NET_RPC_NFS, NULL);
        test_module(TOPIARY_PROC_NET_RPC_NFSD, NULL);
        test_module(TOPIARY_PROC_NET_SNMP, NULL);
        test_module(TOPIARY_PROC_NET_SNMP6, NULL);
        test_module(TOPIARY_PROC_NET_SOCKSTAT, NULL);
        test_module(TOPIARY_PROC_OSRELEASE, NULL);
        test_module(TOPIARY_PROC_PARTITIONS, NULL);
        test_module(TOPIARY_PROC_PID_ENVIRON, NULL);

        test_glob_module(TOPIARY_PROC_PID_ENVIRON, pid);
        test_module(TOPIARY_PROC_PID_IO, NULL);
        test_glob_module(TOPIARY_PROC_PID_IO, pid);
        test_module(TOPIARY_PROC_PID_SMAPS, NULL);
        test_glob_module(TOPIARY_PROC_PID_SMAPS, pid);
        test_module(TOPIARY_PROC_PID_STAT, NULL);
        test_glob_module(TOPIARY_PROC_PID_STAT, pid);
        test_master_module(TOPIARY_PROC_PID_STAT, "chrome");
        test_mod_module(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
        test_module(TOPIARY_PROC_PID_STATM, NULL);
        test_glob_module(TOPIARY_PROC_PID_STATM, pid);
        test_module(TOPIARY_PROC_PID_STATUS, NULL);
        test_glob_module(TOPIARY_PROC_PID_STATUS, pid);
        test_module(TOPIARY_PROC_SOFTIRQS, NULL);
        test_module(TOPIARY_PROC_STAT, NULL);
        test_module(TOPIARY_PROC_SYS_FS_FILE_NR, NULL);
        test_module(TOPIARY_PROC_UPTIME, NULL);
        test_module(TOPIARY_PROC_VMSTAT, NULL);
        test_module(TOPIARY_SYS_CPUFREQ, NULL);
        test_module(TOPIARY_SYS_DISKSECTORS, NULL);
        test_glob_module(TOPIARY_SYS_DISKSECTORS, "sd*");
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
    else if (!strcmp(argv[1], "faulty")) {
        test_faulty();
    }
    else if (!strcmp(argv[1], "all")) {
        test_all();
    }
    else if (!strcmp(argv[1], "interface")) {
        test_interface();
    }
    else if (!strcmp(argv[1], "ntp_drift")) {
        test_module(TOPIARY_NTP_DRIFT, NULL);
    }
    else if (!strcmp(argv[1], "proc_buddyinfo")) {
        test_module(TOPIARY_PROC_BUDDYINFO, NULL);
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_module(TOPIARY_PROC_CGROUPS, NULL);
    }
    else if (!strcmp(argv[1], "proc_cmdline")) {
        test_module(TOPIARY_PROC_CMDLINE, NULL);
    }
    else if (!strcmp(argv[1], "proc_cpuinfo")) {
        test_module(TOPIARY_PROC_CPUINFO, NULL);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        test_module(TOPIARY_PROC_DISKSTATS, NULL);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        test_module(TOPIARY_PROC_DOMAINNAME, NULL);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        test_module(TOPIARY_PROC_HOSTNAME, NULL);
    }
    else if (!strcmp(argv[1], "proc_interrupts")) {
        test_module(TOPIARY_PROC_INTERRUPTS, NULL);
    }
    else if (!strcmp(argv[1], "proc_loadavg")) {
        test_module(TOPIARY_PROC_LOADAVG, NULL);
    }
    else if (!strcmp(argv[1], "proc_meminfo")) {
        test_module(TOPIARY_PROC_MEMINFO, NULL);
    }
    else if (!strcmp(argv[1], "proc_mounts")) {
        test_module(TOPIARY_PROC_MOUNTS, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_dev")) {
        test_module(TOPIARY_PROC_NET_DEV, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_netstat")) {
        test_module(TOPIARY_PROC_NET_NETSTAT, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_rpc_nfs")) {
        test_module(TOPIARY_PROC_NET_RPC_NFS, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_rpc_nfsd")) {
        test_module(TOPIARY_PROC_NET_RPC_NFSD, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_snmp")) {
        test_module(TOPIARY_PROC_NET_SNMP, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_snmp6")) {
        test_module(TOPIARY_PROC_NET_SNMP6, NULL);
    }
    else if (!strcmp(argv[1], "proc_net_sockstat")) {
        test_module(TOPIARY_PROC_NET_SOCKSTAT, NULL);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        test_module(TOPIARY_PROC_OSRELEASE, NULL);
    }
    else if (!strcmp(argv[1], "proc_partitions")) {
        test_module(TOPIARY_PROC_PARTITIONS, NULL);
    }
    else if (!strcmp(argv[1], "proc_pid_environ")) {
        test_module(TOPIARY_PROC_PID_ENVIRON, NULL);
        test_glob_module(TOPIARY_PROC_PID_ENVIRON, pid);
    }
    else if (!strcmp(argv[1], "proc_pid_io")) {
        test_module(TOPIARY_PROC_PID_IO, NULL);
        test_glob_module(TOPIARY_PROC_PID_IO, pid);
    }
    else if (!strcmp(argv[1], "proc_pid_smaps")) {
        test_module(TOPIARY_PROC_PID_SMAPS, NULL);
        test_glob_module(TOPIARY_PROC_PID_SMAPS, pid);
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {

        test_module(TOPIARY_PROC_PID_STAT, NULL);
        test_glob_module(TOPIARY_PROC_PID_STAT, pid);
        test_master_module(TOPIARY_PROC_PID_STAT, "chrome");
        test_mod_module(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
    }
    else if (!strcmp(argv[1], "proc_pid_statm")) {
        test_module(TOPIARY_PROC_PID_STATM, NULL);
        test_glob_module(TOPIARY_PROC_PID_STATM, pid);
    }
    else if (!strcmp(argv[1], "proc_pid_status")) {
        test_module(TOPIARY_PROC_PID_STATUS, NULL);
        test_glob_module(TOPIARY_PROC_PID_STATUS, pid);
    }
    else if (!strcmp(argv[1], "proc_softirqs")) {
        test_module(TOPIARY_PROC_SOFTIRQS, NULL);
    }
    else if (!strcmp(argv[1], "proc_stat")) {
        test_module(TOPIARY_PROC_STAT, NULL);
    }
    else if (!strcmp(argv[1], "proc_sys_fs_file_nr")) {
        test_module(TOPIARY_PROC_SYS_FS_FILE_NR, NULL);
    }
    else if (!strcmp(argv[1], "proc_uptime")) {
        test_module(TOPIARY_PROC_UPTIME, NULL);
    }
    else if (!strcmp(argv[1], "proc_vmstat")) {
        test_module(TOPIARY_PROC_VMSTAT, NULL);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        test_module(TOPIARY_SYS_CPUFREQ, NULL);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        test_module(TOPIARY_SYS_DISKSECTORS, NULL);
        test_glob_module(TOPIARY_SYS_DISKSECTORS, "sd*");
    }
    return 0;
}
