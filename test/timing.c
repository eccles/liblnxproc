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
#include <sys/time.h>
#include <lnxproc/lnxproc.h>

static const int ntimes = 1000;
static int testtype = 0;        // timing test

static void
test_module(LNXPROC_MODULE_T * modules, char *str)
{
    if (modules) {
        char buf[96];
        LNXPROC_ERROR_T ret = lnxproc_read(modules);

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
            long hash_time;
            long normalize_time;
            float sum_rawread_time = 0;
            float sum_map_time = 0;
            float sum_hash_time = 0;
            float sum_normalize_time = 0;
            struct timeval start = lnxproc_timeval();

            int i;

            for (i = 0; i < ntimes; i++) {
                ret = lnxproc_read(modules);
                if (ret) {
                    printf("%s:Error %s\n",str,
                           lnxproc_strerror(ret, buf, sizeof buf));
                }
                lnxproc_performance(modules, &rawread_time, &map_time,
                                    &hash_time, &normalize_time);
                sum_rawread_time += rawread_time;
                sum_map_time += map_time;
                sum_hash_time += hash_time;
                sum_normalize_time += normalize_time;
            }

            struct timeval end = lnxproc_timeval();
            long timediff = lnxproc_timeval_diff(&start, &end);

            printf("%s:Elapsed time = %.1f usecs (%.1f,%.1f,%.1f,%.1f)\n",str,
                   (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
                   sum_map_time / ntimes, sum_hash_time / ntimes,
                   sum_normalize_time / ntimes);
        }
        else if (testtype == 1) {
            lnxproc_read(modules);
            printf("%s:Results 1\n",str);
            lnxproc_print(modules);

            lnxproc_read(modules);
            printf("%s:Results 2\n",str);
            lnxproc_print(modules);
        }
    }
}

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    printf("Command %s\n", argv[0]);

    if (!strcmp(argv[0], "./timing") ||
        !strcmp(argv[0], "./timing-dbg") || !strcmp(argv[0], "./timing-prf")) {
        testtype = 0;
    }
    else if (!strcmp(argv[0], "./testing") ||
             !strcmp(argv[0], "./testing-dbg") ||
             !strcmp(argv[0], "./testing-prf")) {
        testtype = 1;
    }
    else {
        printf("Illegal command %s\n", argv[0]);
        return 1;
    }

    LNXPROC_MODULE_T *modules = NULL;

    if (argc < 2) {
        lnxproc_init(&modules, LNXPROC_ALL, NULL);
        test_module(modules,"All");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_CGROUPS, NULL);
        test_module(modules,"proc_cgroups");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_DISKSTATS, NULL);
        test_module(modules,"proc_diskstats");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_DOMAINNAME, NULL);
        test_module(modules,"proc_domainname");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_HOSTNAME, NULL);
        test_module(modules,"proc_hostname");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_OSRELEASE, NULL);
        test_module(modules,"proc_osrelease");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_SYS_CPUFREQ, NULL);
        test_module(modules,"sys_cpufreq");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_SYS_DISKSECTORS, NULL);
        test_module(modules,"sys_disksectors");
        LNXPROC_FREE(modules);
        lnxproc_init(&modules, LNXPROC_PROC_PID_STAT, NULL);
        test_module(modules,"proc_pid_stat");
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        lnxproc_init(&modules, LNXPROC_PROC_CGROUPS, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        lnxproc_init(&modules, LNXPROC_PROC_DISKSTATS, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        lnxproc_init(&modules, LNXPROC_PROC_DOMAINNAME, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        lnxproc_init(&modules, LNXPROC_PROC_HOSTNAME, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1],"proc_osrelease")) {
        lnxproc_init(&modules, LNXPROC_PROC_OSRELEASE, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        lnxproc_init(&modules, LNXPROC_SYS_CPUFREQ, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        lnxproc_init(&modules, LNXPROC_SYS_DISKSECTORS, NULL);
        test_module(modules,argv[1]);
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {
        lnxproc_init(&modules, LNXPROC_PROC_PID_STAT, NULL);
        test_module(modules,argv[1]);
    }
    LNXPROC_FREE(modules);
    return 0;
}
