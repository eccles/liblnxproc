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

        if (ret) {
            printf("Error %s\n", lnxproc_strerror(ret, buf, sizeof buf));
        }
        ret = lnxproc_read(modules);
        if (ret) {
            printf("Error %s\n", lnxproc_strerror(ret, buf, sizeof buf));
        }
        ret = lnxproc_read(modules);
        if (ret) {
            printf("Error %s\n", lnxproc_strerror(ret, buf, sizeof buf));
        }

        int i;

        if (testtype == 0) {
            long rawread_time;
            long map_time;
            long normalize_time;
            long hash_time;
            size_t size;
            float sum_rawread_time = 0;
            float sum_map_time = 0;
            float sum_normalize_time = 0;
            float sum_hash_time = 0;
            struct timeval start = lnxproc_timeval();

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
            lnxproc_size(modules, &size);

            struct timeval end = lnxproc_timeval();
            long timediff = lnxproc_timeval_diff(&start, &end);

            printf("%s:Elapsed time = %.1f usecs (%.1f,%.1f,%.1f,%.1f,%zd)\n",
                   str, (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
                   sum_map_time / ntimes, sum_normalize_time / ntimes,
                   sum_hash_time / ntimes, size);
        }
        else if (testtype == 1) {
            for (i = 0; i < 2; i++) {
                lnxproc_read(modules);
            }
            lnxproc_print(modules, STDOUT_FILENO, LNXPROC_PRINT_ALL);
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
#define TEST_MODULE(type) do {\
    LNXPROC_MODULE_T *modules = NULL; \
    lnxproc_new(&modules, 1); \
    lnxproc_set(modules, 0, (type), NULL ); \
\
    test_module(modules, #type); \
    LNXPROC_FREE(modules); \
} while(0)

#define TEST_GLOB_MODULE(type,option) do {\
    LNXPROC_OPT_T *opt = NULL; \
    lnxproc_opt_new(&opt); \
    lnxproc_opt_set_fileglob(opt,(option)); \
\
    LNXPROC_MODULE_T *modules = NULL; \
    lnxproc_new(&modules, 1); \
    lnxproc_set(modules, 0, (type), opt ); \
    LNXPROC_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    LNXPROC_FREE(modules); \
} while(0)

#define TEST_PID_MODULE(type) do {\
    TEST_GLOB_MODULE(type,pid);\
} while(0)

#define TEST_MASTER_MODULE(type,task) do {\
    LNXPROC_OPT_T *opt = NULL; \
    lnxproc_opt_new(&opt); \
    lnxproc_opt_set_master(opt,(task)); \
\
    LNXPROC_MODULE_T *modules = NULL; \
    lnxproc_new(&modules, 1); \
    lnxproc_set(modules, 0, (type), opt ); \
    LNXPROC_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    LNXPROC_FREE(modules); \
} while(0)

#define TEST_MOD_MODULE(type,task,subtype) do {\
    LNXPROC_MODULE_T *submodule = NULL; \
    lnxproc_new(&submodule, 1); \
    lnxproc_set(submodule, 0, (subtype), NULL ); \
    LNXPROC_FREE(submodule); \
\
    LNXPROC_OPT_T *opt = NULL; \
    lnxproc_opt_new(&opt); \
    lnxproc_opt_set_module(opt,submodule); \
    lnxproc_opt_set_master(opt,(task)); \
\
    LNXPROC_MODULE_T *modules = NULL; \
    lnxproc_new(&modules, 1); \
    lnxproc_set(modules, 0, (type), opt ); \
    LNXPROC_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    LNXPROC_FREE(modules); \
} while(0)

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    char pid[32];

    snprintf(pid, sizeof pid, "%d", getpid());

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
        TEST_MODULE(LNXPROC_PROC_BUDDYINFO);
        TEST_MODULE(LNXPROC_PROC_CGROUPS);
        TEST_MODULE(LNXPROC_PROC_CMDLINE);
        TEST_MODULE(LNXPROC_PROC_CPUINFO);
        TEST_MODULE(LNXPROC_PROC_DISKSTATS);
        TEST_MODULE(LNXPROC_PROC_DOMAINNAME);
        TEST_MODULE(LNXPROC_PROC_HOSTNAME);
        TEST_MODULE(LNXPROC_PROC_INTERRUPTS);
        TEST_MODULE(LNXPROC_PROC_LOADAVG);
        TEST_MODULE(LNXPROC_PROC_MEMINFO);
        TEST_MODULE(LNXPROC_PROC_MOUNTS);
        TEST_MODULE(LNXPROC_PROC_NET_DEV);
        TEST_MODULE(LNXPROC_PROC_NET_NETSTAT);
        TEST_MODULE(LNXPROC_PROC_NET_RPC_NFS);
        TEST_MODULE(LNXPROC_PROC_NET_RPC_NFSD);
        TEST_MODULE(LNXPROC_PROC_NET_SNMP);
        TEST_MODULE(LNXPROC_PROC_NET_SNMP6);
        TEST_MODULE(LNXPROC_PROC_NET_SOCKSTAT);
        TEST_MODULE(LNXPROC_PROC_OSRELEASE);
        TEST_MODULE(LNXPROC_PROC_PARTITIONS);
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON);
        TEST_PID_MODULE(LNXPROC_PROC_PID_ENVIRON);
        TEST_MODULE(LNXPROC_PROC_PID_IO);
        TEST_PID_MODULE(LNXPROC_PROC_PID_IO);
        TEST_MODULE(LNXPROC_PROC_PID_SMAPS);
        TEST_PID_MODULE(LNXPROC_PROC_PID_SMAPS);
        TEST_MODULE(LNXPROC_PROC_PID_STAT);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STAT);
        TEST_MASTER_MODULE(LNXPROC_PROC_PID_STAT, "chrome");
        TEST_MOD_MODULE(LNXPROC_PROC_PID_STAT, "chrome",
                        LNXPROC_PROC_PID_STATM);
        TEST_MODULE(LNXPROC_PROC_PID_STATM);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STATM);
        TEST_MODULE(LNXPROC_PROC_PID_STATUS);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STATUS);
        TEST_MODULE(LNXPROC_PROC_SOFTIRQS);
        TEST_MODULE(LNXPROC_PROC_STAT);
        TEST_MODULE(LNXPROC_PROC_SYS_FS_FILE_NR);
        TEST_MODULE(LNXPROC_PROC_UPTIME);
        TEST_MODULE(LNXPROC_PROC_VMSTAT);
        TEST_MODULE(LNXPROC_SYS_CPUFREQ);
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS);
        TEST_GLOB_MODULE(LNXPROC_SYS_DISKSECTORS, "sd*");

    }
    else if (!strcmp(argv[1], "proc_buddyinfo")) {
        TEST_MODULE(LNXPROC_PROC_BUDDYINFO);
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        TEST_MODULE(LNXPROC_PROC_CGROUPS);
    }
    else if (!strcmp(argv[1], "proc_cmdline")) {
        TEST_MODULE(LNXPROC_PROC_CMDLINE);
    }
    else if (!strcmp(argv[1], "proc_cpuinfo")) {
        TEST_MODULE(LNXPROC_PROC_CPUINFO);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        TEST_MODULE(LNXPROC_PROC_DISKSTATS);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        TEST_MODULE(LNXPROC_PROC_DOMAINNAME);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        TEST_MODULE(LNXPROC_PROC_HOSTNAME);
    }
    else if (!strcmp(argv[1], "proc_interrupts")) {
        TEST_MODULE(LNXPROC_PROC_INTERRUPTS);
    }
    else if (!strcmp(argv[1], "proc_loadavg")) {
        TEST_MODULE(LNXPROC_PROC_LOADAVG);
    }
    else if (!strcmp(argv[1], "proc_meminfo")) {
        TEST_MODULE(LNXPROC_PROC_MEMINFO);
    }
    else if (!strcmp(argv[1], "proc_mounts")) {
        TEST_MODULE(LNXPROC_PROC_MOUNTS);
    }
    else if (!strcmp(argv[1], "proc_net_dev")) {
        TEST_MODULE(LNXPROC_PROC_NET_DEV);
    }
    else if (!strcmp(argv[1], "proc_net_netstat")) {
        TEST_MODULE(LNXPROC_PROC_NET_NETSTAT);
    }
    else if (!strcmp(argv[1], "proc_net_rpc_nfs")) {
        TEST_MODULE(LNXPROC_PROC_NET_RPC_NFS);
    }
    else if (!strcmp(argv[1], "proc_net_rpc_nfsd")) {
        TEST_MODULE(LNXPROC_PROC_NET_RPC_NFSD);
    }
    else if (!strcmp(argv[1], "proc_net_snmp")) {
        TEST_MODULE(LNXPROC_PROC_NET_SNMP);
    }
    else if (!strcmp(argv[1], "proc_net_snmp6")) {
        TEST_MODULE(LNXPROC_PROC_NET_SNMP6);
    }
    else if (!strcmp(argv[1], "proc_net_sockstat")) {
        TEST_MODULE(LNXPROC_PROC_NET_SOCKSTAT);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        TEST_MODULE(LNXPROC_PROC_OSRELEASE);
    }
    else if (!strcmp(argv[1], "proc_partitions")) {
        TEST_MODULE(LNXPROC_PROC_PARTITIONS);
    }
    else if (!strcmp(argv[1], "proc_pid_environ")) {
        TEST_MODULE(LNXPROC_PROC_PID_ENVIRON);
        TEST_PID_MODULE(LNXPROC_PROC_PID_ENVIRON);
    }
    else if (!strcmp(argv[1], "proc_pid_io")) {
        TEST_MODULE(LNXPROC_PROC_PID_IO);
        TEST_PID_MODULE(LNXPROC_PROC_PID_IO);
    }
    else if (!strcmp(argv[1], "proc_pid_smaps")) {
        TEST_MODULE(LNXPROC_PROC_PID_SMAPS);
        TEST_PID_MODULE(LNXPROC_PROC_PID_SMAPS);
    }
    else if (!strcmp(argv[1], "proc_pid_stat")) {
        TEST_MODULE(LNXPROC_PROC_PID_STAT);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STAT);
        TEST_MASTER_MODULE(LNXPROC_PROC_PID_STAT, "chrome");
        TEST_MOD_MODULE(LNXPROC_PROC_PID_STAT, "chrome",
                        LNXPROC_PROC_PID_STATM);
    }
    else if (!strcmp(argv[1], "proc_pid_statm")) {
        TEST_MODULE(LNXPROC_PROC_PID_STATM);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STATM);
    }
    else if (!strcmp(argv[1], "proc_pid_status")) {
        TEST_MODULE(LNXPROC_PROC_PID_STATUS);
        TEST_PID_MODULE(LNXPROC_PROC_PID_STATUS);
    }
    else if (!strcmp(argv[1], "proc_softirqs")) {
        TEST_MODULE(LNXPROC_PROC_SOFTIRQS);
    }
    else if (!strcmp(argv[1], "proc_stat")) {
        TEST_MODULE(LNXPROC_PROC_STAT);
    }
    else if (!strcmp(argv[1], "proc_sys_fs_file_nr")) {
        TEST_MODULE(LNXPROC_PROC_SYS_FS_FILE_NR);
    }
    else if (!strcmp(argv[1], "proc_uptime")) {
        TEST_MODULE(LNXPROC_PROC_UPTIME);
    }
    else if (!strcmp(argv[1], "proc_vmstat")) {
        TEST_MODULE(LNXPROC_PROC_VMSTAT);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        TEST_MODULE(LNXPROC_SYS_CPUFREQ);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        TEST_MODULE(LNXPROC_SYS_DISKSECTORS);
        TEST_GLOB_MODULE(LNXPROC_SYS_DISKSECTORS, "sd*");
    }
    return 0;
}
