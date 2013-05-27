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

#include <stdio.h>              // printf
#include <string.h>             // strrchr()
#include <sys/types.h>          // getpid()
#include <unistd.h>             // getpid()

#include <lnxproc/lnxproc.h>

static const int ntimes = 1000;
static int testtype = 0;        // timing test

static void
test_module(LNXPROC_MODULE_T * modules, char *str)
{
    if (modules) {
        char buf[96];
        int ret = lnxproc_read(modules);

        ret = lnxproc_read(modules);
        if (ret) {
            printf("Error %s\n", lnxproc_strerror(ret, buf, sizeof buf));
        }
        ret = lnxproc_read(modules);
        if (ret) {
            printf("Error %s\n", lnxproc_strerror(ret, buf, sizeof buf));
        }

        if (testtype == 0) {
            long rawread_time;
            long map_time;
            long normalize_time;
            long hash_time;
            float sum_rawread_time = 0;
            float sum_map_time = 0;
            float sum_normalize_time = 0;
            float sum_hash_time = 0;
            struct timeval start = lnxproc_timeval();

            int i;

            for (i = 0; i < ntimes; i++) {
                ret = lnxproc_read(modules);
                if (ret) {
                    printf("%s:Error %s\n", str,
                           lnxproc_strerror(ret, buf, sizeof buf));
                    break;
                }
                lnxproc_performance(modules, &rawread_time, &map_time,
                                    &normalize_time, &hash_time);
                sum_rawread_time += rawread_time;
                sum_map_time += map_time;
                sum_normalize_time += normalize_time;
                sum_hash_time += hash_time;
            }

            struct timeval end = lnxproc_timeval();
            long timediff = lnxproc_timeval_diff(&start, &end);

            printf("%s:Elapsed time = %.1f usecs (%.1f,%.1f,%.1f,%.1f)\n", str,
                   (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
                   sum_map_time / ntimes,
                   sum_normalize_time / ntimes, sum_hash_time / ntimes);
        }
        else if (testtype == 1) {
            lnxproc_read(modules);
            printf("%s:Results 1\n", str);
            lnxproc_print(modules);

            lnxproc_read(modules);
            printf("%s:Results 2\n", str);
            lnxproc_print(modules);
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
test_all(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 0);
    test_module(modules, "All");
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_proc_cgroups(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_CGROUPS, NULL, 0);
    test_module(modules, "proc_cgroups");
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_proc_diskstats(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_DISKSTATS, NULL, 0);
    test_module(modules, "proc_diskstats");
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_proc_domainname(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_DOMAINNAME, NULL, 0);
    test_module(modules, "proc_domainname");
    LNXPROC_FREE(modules);
}

/*----------------------------------------------------------------------------*/
static void
test_proc_hostname(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_HOSTNAME, NULL, 0);
    test_module(modules, "proc_hostname");
    LNXPROC_FREE(modules);
}

/*----------------------------------------------------------------------------*/
static void
test_proc_osrelease(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_OSRELEASE, NULL, 0);
    test_module(modules, "proc_osrelease");
    LNXPROC_FREE(modules);
}

/*----------------------------------------------------------------------------*/
static void
test_sys_cpufreq(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_SYS_CPUFREQ, NULL, 0);
    test_module(modules, "sys_cpufreq");
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_sys_disksectors(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, NULL, 0);
    test_module(modules, "sys_disksectors");
    LNXPROC_FREE(modules);

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, "sd*", 4);
    test_module(modules, "sys_sd_disksectors");
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_proc_pid_environ(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_PID_ENVIRON, NULL, 0);
    test_module(modules, "proc_pid_environ");
    LNXPROC_FREE(modules);

    char buf[32];

    snprintf(buf, sizeof buf, "%d", getpid());
    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_PID_ENVIRON, buf, 1 + strlen(buf));
    snprintf(buf, sizeof buf, "proc_%d_environ", getpid());
    test_module(modules, buf);
    LNXPROC_FREE(modules);

}

/*----------------------------------------------------------------------------*/
static void
test_proc_pid_stat(void)
{
    LNXPROC_MODULE_T *modules = NULL;

    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_PID_STAT, NULL, 0);
    test_module(modules, "proc_pid_stat");
    LNXPROC_FREE(modules);

    char buf[32];

    snprintf(buf, sizeof buf, "%d", getpid());
    lnxproc_new(&modules, 1);
    lnxproc_set(modules, 0, LNXPROC_PROC_PID_STAT, buf, 1 + strlen(buf));
    snprintf(buf, sizeof buf, "proc_%d_stat", getpid());
    test_module(modules, buf);
    LNXPROC_FREE(modules);
}

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    char *c = strrchr(argv[0], '/');

    c++;

    printf("Command %s\n", c);

    if (!strcmp(c, "timing") ||
        !strcmp(c, "timing-dbg") || !strcmp(c, "timing-prf")) {
        testtype = 0;
    }
    else if (!strcmp(c, "testing") ||
             !strcmp(c, "testing-dbg") || !strcmp(c, "testing-prf")) {
        testtype = 1;
    }
    else {
        printf("Illegal command %s\n", c);
        return 1;
    }

    if (argc < 2) {

        test_all();
        test_proc_cgroups();
        test_proc_diskstats();
        test_proc_domainname();
        test_proc_hostname();
        test_proc_osrelease();
        test_sys_cpufreq();
        test_sys_disksectors();
        test_proc_pid_environ();
        test_proc_pid_stat();

    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_proc_cgroups();
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        test_proc_diskstats();
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
    else if (!strcmp(argv[1], "sys_disksectors")) {
        test_sys_disksectors();
    }
    else if (!strcmp(argv[1], "proc_pid_environ")) {
        test_proc_pid_environ();
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {
        test_proc_pid_stat();
    }
    return 0;
}
