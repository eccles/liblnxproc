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

#include <stdio.h>              // printf
#include <string.h>             // strrchr()
#include <sys/types.h>          // getpid()
#include <unistd.h>             // getpid()

#include <topiary/topiary.h>

static const int ntimes = 1000;
static int testtype = 0;        // timing test

static void
test_module(TOPIARY_MODULE_T *modules, char *str)
{
    if (modules) {
        char buf[96];
        int ret = topiary_read(modules);

        if (ret) {
            printf("Error %s\n", topiary_strerror(ret, buf, sizeof buf));
        }
        ret = topiary_read(modules);
        if (ret) {
            printf("Error %s\n", topiary_strerror(ret, buf, sizeof buf));
        }
        ret = topiary_read(modules);
        if (ret) {
            printf("Error %s\n", topiary_strerror(ret, buf, sizeof buf));
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
            struct timeval start = topiary_timeval();

            for (i = 0; i < ntimes; i++) {
                ret = topiary_read(modules);
                if (ret) {
                    printf("%s:Error %s\n", str,
                           topiary_strerror(ret, buf, sizeof buf));
                    break;
                }
                topiary_performance(modules, &rawread_time, &map_time,
                                    &normalize_time, &hash_time);
                sum_rawread_time += rawread_time;
                sum_map_time += map_time;
                sum_normalize_time += normalize_time;
                sum_hash_time += hash_time;
            }
            topiary_size(modules, &size);

            struct timeval end = topiary_timeval();
            long timediff = topiary_timeval_diff(&start, &end);

            printf("%s:Elapsed time = %.1f usecs (%.1f,%.1f,%.1f,%.1f,%zd)\n",
                   str, (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
                   sum_map_time / ntimes, sum_normalize_time / ntimes,
                   sum_hash_time / ntimes, size);
        }
        else if (testtype == 1) {
            for (i = 0; i < 2; i++) {
                topiary_read(modules);
            }
            topiary_print(modules, STDOUT_FILENO, TOPIARY_PRINT_ALL);
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
test_all(void)
{
    TOPIARY_MODULE_T *modules = NULL;

    topiary_new(&modules, 0);
    test_module(modules, "All");
    topiary_free(&modules);

}

/*----------------------------------------------------------------------------*/
#define TEST_MODULE(type) do {\
    TOPIARY_MODULE_T *modules = NULL; \
    topiary_new(&modules, 1); \
    topiary_set(modules, 0, (type), NULL ); \
\
    test_module(modules, #type); \
    topiary_free(&modules); \
} while(0)

#define TEST_GLOB_MODULE(type,option) do {\
    TOPIARY_OPT_T *opt = NULL; \
    topiary_opt_new(&opt); \
    topiary_opt_set_fileglob(opt,(option)); \
\
    TOPIARY_MODULE_T *modules = NULL; \
    topiary_new(&modules, 1); \
    topiary_set(modules, 0, (type), opt ); \
    TOPIARY_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    topiary_free(&modules); \
} while(0)

#define TEST_PID_MODULE(type) do {\
    TEST_GLOB_MODULE(type,pid);\
} while(0)

#define TEST_MASTER_MODULE(type,task) do {\
    TOPIARY_OPT_T *opt = NULL; \
    topiary_opt_new(&opt); \
    topiary_opt_set_master(opt,(task)); \
\
    TOPIARY_MODULE_T *modules = NULL; \
    topiary_new(&modules, 1); \
    topiary_set(modules, 0, (type), opt ); \
    TOPIARY_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    topiary_free(&modules); \
} while(0)

#define TEST_MOD_MODULE(type,task,subtype) do {\
    TOPIARY_MODULE_T *submodule = NULL; \
    topiary_new(&submodule, 1); \
    topiary_set(submodule, 0, (subtype), NULL ); \
    topiary_free(&submodule); \
\
    TOPIARY_OPT_T *opt = NULL; \
    topiary_opt_new(&opt); \
    topiary_opt_set_module(opt,submodule); \
    topiary_opt_set_master(opt,(task)); \
\
    TOPIARY_MODULE_T *modules = NULL; \
    topiary_new(&modules, 1); \
    topiary_set(modules, 0, (type), opt ); \
    TOPIARY_OPT_FREE(opt); \
\
    test_module(modules, #type); \
    topiary_free(&modules); \
} while(0)

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    char pid[32];

    snprintf(pid, sizeof pid, "%d", getpid());

    if (argc < 2) {
        printf("First option is unspecified\n");
        return 1;
    }
    else if (!strcmp(argv[1], "time")) {
        testtype = 0;
    }
    else if (!strcmp(argv[1], "test")) {
        testtype = 1;
    }
    else {
        printf("Illegal option %s\n", argv[1]);
        return 1;
    }

    char *module = NULL;

    if (argc > 2) {
        module = argv[2];
    }

    if (argc < 3) {

        test_all();
        TEST_MODULE(TOPIARY_NTP_DRIFT);
        TEST_MODULE(TOPIARY_PROC_BUDDYINFO);
        TEST_MODULE(TOPIARY_PROC_CGROUPS);
        TEST_MODULE(TOPIARY_PROC_CMDLINE);
        TEST_MODULE(TOPIARY_PROC_CPUINFO);
        TEST_MODULE(TOPIARY_PROC_DISKSTATS);
        TEST_MODULE(TOPIARY_PROC_DOMAINNAME);
        TEST_MODULE(TOPIARY_PROC_HOSTNAME);
        TEST_MODULE(TOPIARY_PROC_INTERRUPTS);
        TEST_MODULE(TOPIARY_PROC_LOADAVG);
        TEST_MODULE(TOPIARY_PROC_MEMINFO);
        TEST_MODULE(TOPIARY_PROC_MOUNTS);
        TEST_MODULE(TOPIARY_PROC_NET_DEV);
        TEST_MODULE(TOPIARY_PROC_NET_NETSTAT);
        TEST_MODULE(TOPIARY_PROC_NET_RPC_NFS);
        TEST_MODULE(TOPIARY_PROC_NET_RPC_NFSD);
        TEST_MODULE(TOPIARY_PROC_NET_SNMP);
        TEST_MODULE(TOPIARY_PROC_NET_SNMP6);
        TEST_MODULE(TOPIARY_PROC_NET_SOCKSTAT);
        TEST_MODULE(TOPIARY_PROC_OSRELEASE);
        TEST_MODULE(TOPIARY_PROC_PARTITIONS);
        TEST_MODULE(TOPIARY_PROC_PID_ENVIRON);
        TEST_PID_MODULE(TOPIARY_PROC_PID_ENVIRON);
        TEST_MODULE(TOPIARY_PROC_PID_IO);
        TEST_PID_MODULE(TOPIARY_PROC_PID_IO);
        TEST_MODULE(TOPIARY_PROC_PID_SMAPS);
        TEST_PID_MODULE(TOPIARY_PROC_PID_SMAPS);
        TEST_MODULE(TOPIARY_PROC_PID_STAT);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STAT);
        TEST_MASTER_MODULE(TOPIARY_PROC_PID_STAT, "chrome");
        TEST_MOD_MODULE(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
        TEST_MODULE(TOPIARY_PROC_PID_STATM);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STATM);
        TEST_MODULE(TOPIARY_PROC_PID_STATUS);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STATUS);
        TEST_MODULE(TOPIARY_PROC_SOFTIRQS);
        TEST_MODULE(TOPIARY_PROC_STAT);
        TEST_MODULE(TOPIARY_PROC_SYS_FS_FILE_NR);
        TEST_MODULE(TOPIARY_PROC_UPTIME);
        TEST_MODULE(TOPIARY_PROC_VMSTAT);
        TEST_MODULE(TOPIARY_SYS_CPUFREQ);
        TEST_MODULE(TOPIARY_SYS_DISKSECTORS);
        TEST_GLOB_MODULE(TOPIARY_SYS_DISKSECTORS, "sd*");

    }
    else if (!strcmp(module, "ntp_drift")) {
        TEST_MODULE(TOPIARY_NTP_DRIFT);
    }
    else if (!strcmp(module, "proc_buddyinfo")) {
        TEST_MODULE(TOPIARY_PROC_BUDDYINFO);
    }
    else if (!strcmp(module, "proc_cgroups")) {
        TEST_MODULE(TOPIARY_PROC_CGROUPS);
    }
    else if (!strcmp(module, "proc_cmdline")) {
        TEST_MODULE(TOPIARY_PROC_CMDLINE);
    }
    else if (!strcmp(module, "proc_cpuinfo")) {
        TEST_MODULE(TOPIARY_PROC_CPUINFO);
    }
    else if (!strcmp(module, "proc_diskstats")) {
        TEST_MODULE(TOPIARY_PROC_DISKSTATS);
    }
    else if (!strcmp(module, "proc_domainname")) {
        TEST_MODULE(TOPIARY_PROC_DOMAINNAME);
    }
    else if (!strcmp(module, "proc_hostname")) {
        TEST_MODULE(TOPIARY_PROC_HOSTNAME);
    }
    else if (!strcmp(module, "proc_interrupts")) {
        TEST_MODULE(TOPIARY_PROC_INTERRUPTS);
    }
    else if (!strcmp(module, "proc_loadavg")) {
        TEST_MODULE(TOPIARY_PROC_LOADAVG);
    }
    else if (!strcmp(module, "proc_meminfo")) {
        TEST_MODULE(TOPIARY_PROC_MEMINFO);
    }
    else if (!strcmp(module, "proc_mounts")) {
        TEST_MODULE(TOPIARY_PROC_MOUNTS);
    }
    else if (!strcmp(module, "proc_net_dev")) {
        TEST_MODULE(TOPIARY_PROC_NET_DEV);
    }
    else if (!strcmp(module, "proc_net_netstat")) {
        TEST_MODULE(TOPIARY_PROC_NET_NETSTAT);
    }
    else if (!strcmp(module, "proc_net_rpc_nfs")) {
        TEST_MODULE(TOPIARY_PROC_NET_RPC_NFS);
    }
    else if (!strcmp(module, "proc_net_rpc_nfsd")) {
        TEST_MODULE(TOPIARY_PROC_NET_RPC_NFSD);
    }
    else if (!strcmp(module, "proc_net_snmp")) {
        TEST_MODULE(TOPIARY_PROC_NET_SNMP);
    }
    else if (!strcmp(module, "proc_net_snmp6")) {
        TEST_MODULE(TOPIARY_PROC_NET_SNMP6);
    }
    else if (!strcmp(module, "proc_net_sockstat")) {
        TEST_MODULE(TOPIARY_PROC_NET_SOCKSTAT);
    }
    else if (!strcmp(module, "proc_osrelease")) {
        TEST_MODULE(TOPIARY_PROC_OSRELEASE);
    }
    else if (!strcmp(module, "proc_partitions")) {
        TEST_MODULE(TOPIARY_PROC_PARTITIONS);
    }
    else if (!strcmp(module, "proc_pid_environ")) {
        TEST_MODULE(TOPIARY_PROC_PID_ENVIRON);
        TEST_PID_MODULE(TOPIARY_PROC_PID_ENVIRON);
    }
    else if (!strcmp(module, "proc_pid_io")) {
        TEST_MODULE(TOPIARY_PROC_PID_IO);
        TEST_PID_MODULE(TOPIARY_PROC_PID_IO);
    }
    else if (!strcmp(module, "proc_pid_smaps")) {
        TEST_MODULE(TOPIARY_PROC_PID_SMAPS);
        TEST_PID_MODULE(TOPIARY_PROC_PID_SMAPS);
    }
    else if (!strcmp(module, "proc_pid_stat")) {
        TEST_MODULE(TOPIARY_PROC_PID_STAT);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STAT);
        TEST_MASTER_MODULE(TOPIARY_PROC_PID_STAT, "chrome");
        TEST_MOD_MODULE(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
    }
    else if (!strcmp(module, "proc_pid_statm")) {
        TEST_MODULE(TOPIARY_PROC_PID_STATM);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STATM);
    }
    else if (!strcmp(module, "proc_pid_status")) {
        TEST_MODULE(TOPIARY_PROC_PID_STATUS);
        TEST_PID_MODULE(TOPIARY_PROC_PID_STATUS);
    }
    else if (!strcmp(module, "proc_softirqs")) {
        TEST_MODULE(TOPIARY_PROC_SOFTIRQS);
    }
    else if (!strcmp(module, "proc_stat")) {
        TEST_MODULE(TOPIARY_PROC_STAT);
    }
    else if (!strcmp(module, "proc_sys_fs_file_nr")) {
        TEST_MODULE(TOPIARY_PROC_SYS_FS_FILE_NR);
    }
    else if (!strcmp(module, "proc_uptime")) {
        TEST_MODULE(TOPIARY_PROC_UPTIME);
    }
    else if (!strcmp(module, "proc_vmstat")) {
        TEST_MODULE(TOPIARY_PROC_VMSTAT);
    }
    else if (!strcmp(module, "sys_cpufreq")) {
        TEST_MODULE(TOPIARY_SYS_CPUFREQ);
    }
    else if (!strcmp(module, "sys_disksectors")) {
        TEST_MODULE(TOPIARY_SYS_DISKSECTORS);
        TEST_GLOB_MODULE(TOPIARY_SYS_DISKSECTORS, "sd*");
    }
    return 0;
}
