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
#include <string.h>             // strrchr()
#include <sys/time.h>
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
                    printf("%s:Error %s\n", str,
                           lnxproc_strerror(ret, buf, sizeof buf));
                    break;
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

            printf("%s:Elapsed time = %.1f usecs (%.1f,%.1f,%.1f,%.1f)\n", str,
                   (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
                   sum_map_time / ntimes, sum_hash_time / ntimes,
                   sum_normalize_time / ntimes);
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

    LNXPROC_MODULE_T *modules = NULL;

    if (argc < 2) {
        lnxproc_new(&modules, 0);
        test_module(modules, "All");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_CGROUPS, NULL, 0);
        test_module(modules, "proc_cgroups");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_DISKSTATS, NULL, 0);
        test_module(modules, "proc_diskstats");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_DOMAINNAME, NULL, 0);
        test_module(modules, "proc_domainname");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_HOSTNAME, NULL, 0);
        test_module(modules, "proc_hostname");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_OSRELEASE, NULL, 0);
        test_module(modules, "proc_osrelease");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_CPUFREQ, NULL, 0);
        test_module(modules, "sys_cpufreq");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, NULL, 0);
        test_module(modules, "sys_disksectors");
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, "sd*", 4);
        test_module(modules, "sys_sd_disksectors");
        LNXPROC_FREE(modules);

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

    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_CGROUPS, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_DISKSTATS, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_DOMAINNAME, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_HOSTNAME, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_OSRELEASE, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_CPUFREQ, NULL, 0);
        test_module(modules, argv[1]);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, NULL, 0);
        test_module(modules, argv[1]);
        LNXPROC_FREE(modules);

        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_SYS_DISKSECTORS, "sd*", 4);
        test_module(modules, "sys_sd_disksectors");
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_PID_STAT, NULL, 0);
        test_module(modules, argv[1]);
        LNXPROC_FREE(modules);

        char buf[32];

        snprintf(buf, sizeof buf, "%d", getpid());
        lnxproc_new(&modules, 1);
        lnxproc_set(modules, 0, LNXPROC_PROC_PID_STAT, buf, 1 + strlen(buf));
        snprintf(buf, sizeof buf, "proc_%d_stat", getpid());
        test_module(modules, buf);
    }
    LNXPROC_FREE(modules);
    return 0;
}
