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

#include "testing.h"

static const int ntimes = 1000;
static int testtype = 0;        // timing test

static void
_test_module(TOPIARY_MODULE_T *modules, const char *tag)
{
    if (modules) {
        char buf[96];
        int ret = topiary_read(modules);

        if (ret) {
            printf("%s:Error %s\n", tag, topiary_strerror(ret, buf, sizeof buf));
        }
        ret = topiary_read(modules);
        if (ret) {
            printf("%s:Error %s\n", tag, topiary_strerror(ret, buf, sizeof buf));
        }
        ret = topiary_read(modules);
        if (ret) {
            printf("%s:Error %s\n", tag, topiary_strerror(ret, buf, sizeof buf));
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
                    printf("%s:Error %s\n", tag,
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
                   tag, (timediff * 1.0) / ntimes, sum_rawread_time / ntimes,
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
    _test_module(modules, "All");
    topiary_free(&modules);

}

/*----------------------------------------------------------------------------*/
static void 
test_module(TOPIARY_MODULE_TYPE_T type) {
    TOPIARY_MODULE_T *modules = NULL;
    topiary_new(&modules, 1);
    topiary_set(modules, 0, type, NULL );
    const char *tag;
    topiary_tag(modules, 0, &tag);
    _test_module(modules, tag);
    topiary_free(&modules);
}

/*----------------------------------------------------------------------------*/
static void 
test_glob_module(TOPIARY_MODULE_TYPE_T type, char *glob) {
    TOPIARY_OPT_T *opt = NULL;
    topiary_opt_new(&opt);
    topiary_opt_set_fileglob(opt,glob);

    TOPIARY_MODULE_T *modules = NULL; 
    topiary_new(&modules, 1);
    topiary_set(modules, 0, type, opt );
    topiary_opt_free(&opt);

    const char *tag;
    topiary_tag(modules, 0, &tag);

    _test_module(modules, tag);
    topiary_free(&modules);
}

/*----------------------------------------------------------------------------*/
static void 
test_pid_module(TOPIARY_MODULE_TYPE_T type, char *pid) {
    printf("%s:Pid is %s\n",__func__, pid);
    if( pid && pid[0] ) {
        test_glob_module(type,pid);
    }
}

/*----------------------------------------------------------------------------*/
static void 
test_master_module(TOPIARY_MODULE_TYPE_T type, char *master) {
    TOPIARY_OPT_T *opt = NULL;
    topiary_opt_new(&opt);
    topiary_opt_set_master(opt,master);

    TOPIARY_MODULE_T *modules = NULL;
    topiary_new(&modules, 1);
    topiary_set(modules, 0, type, opt );
    topiary_opt_free(&opt);

    const char *tag;
    topiary_tag(modules, 0, &tag);

    _test_module(modules, tag);

    topiary_free(&modules);
}

/*----------------------------------------------------------------------------*/
static void 
test_mod_module(TOPIARY_MODULE_TYPE_T type, char *master, TOPIARY_MODULE_TYPE_T subtype) {
    TOPIARY_MODULE_T *submodule = NULL;
    topiary_new(&submodule, 1);
    topiary_set(submodule, 0, subtype, NULL );
    topiary_free(&submodule);

    TOPIARY_OPT_T *opt = NULL;
    topiary_opt_new(&opt);
    topiary_opt_set_module(opt,submodule);
    topiary_opt_set_master(opt,master);

    TOPIARY_MODULE_T *modules = NULL;
    topiary_new(&modules, 1);
    topiary_set(modules, 0, (type), opt );
    topiary_opt_free(&opt);

    const char *tag;
    topiary_tag(modules, 0, &tag);

    _test_module(modules, tag);
    topiary_free(&modules);
}

/*----------------------------------------------------------------------------*/
int
testing(int mytesttype, char *module)
{
    testtype = mytesttype;

    char pid[32] = "";

    if( testtype == 0 ) {
        snprintf(pid, sizeof pid, "%d", getpid());
    }
    printf("Pid is %s\n",pid);

    if (!module) {

        test_all();
        test_module(TOPIARY_NTP_DRIFT);
        test_module(TOPIARY_PROC_BUDDYINFO);
        test_module(TOPIARY_PROC_CGROUPS);
        test_module(TOPIARY_PROC_CMDLINE);
        test_module(TOPIARY_PROC_CPUINFO);
        test_module(TOPIARY_PROC_DISKSTATS);
        test_module(TOPIARY_PROC_DOMAINNAME);
        test_module(TOPIARY_PROC_HOSTNAME);
        test_module(TOPIARY_PROC_INTERRUPTS);
        test_module(TOPIARY_PROC_LOADAVG);
        test_module(TOPIARY_PROC_MEMINFO);
        test_module(TOPIARY_PROC_MOUNTS);
        test_module(TOPIARY_PROC_NET_DEV);
        test_module(TOPIARY_PROC_NET_NETSTAT);
        test_module(TOPIARY_PROC_NET_RPC_NFS);
        test_module(TOPIARY_PROC_NET_RPC_NFSD);
        test_module(TOPIARY_PROC_NET_SNMP);
        test_module(TOPIARY_PROC_NET_SNMP6);
        test_module(TOPIARY_PROC_NET_SOCKSTAT);
        test_module(TOPIARY_PROC_OSRELEASE);
        test_module(TOPIARY_PROC_PARTITIONS);
        test_module(TOPIARY_PROC_PID_ENVIRON);
        test_pid_module(TOPIARY_PROC_PID_ENVIRON,pid);
        test_module(TOPIARY_PROC_PID_IO);
        test_pid_module(TOPIARY_PROC_PID_IO,pid);
        test_module(TOPIARY_PROC_PID_SMAPS);
        test_pid_module(TOPIARY_PROC_PID_SMAPS,pid);
        test_module(TOPIARY_PROC_PID_STAT);
        test_pid_module(TOPIARY_PROC_PID_STAT,pid);
        test_master_module(TOPIARY_PROC_PID_STAT, "chrome");
        test_mod_module(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
        test_module(TOPIARY_PROC_PID_STATM);
        test_pid_module(TOPIARY_PROC_PID_STATM,pid);
        test_module(TOPIARY_PROC_PID_STATUS);
        test_pid_module(TOPIARY_PROC_PID_STATUS,pid);
        test_module(TOPIARY_PROC_SOFTIRQS);
        test_module(TOPIARY_PROC_STAT);
        test_module(TOPIARY_PROC_SYS_FS_FILE_NR);
        test_module(TOPIARY_PROC_UPTIME);
        test_module(TOPIARY_PROC_VMSTAT);
/* later kernels (3.8?) do not have this....
 *      test_module(TOPIARY_SYS_CPUFREQ);
 */
        test_module(TOPIARY_SYS_DISKSECTORS);
        test_glob_module(TOPIARY_SYS_DISKSECTORS, "sd*");

    }
    else if (!strcmp(module, "all")) {
        test_all();
    }
    else if (!strcmp(module, "ntp_drift")) {
        test_module(TOPIARY_NTP_DRIFT);
    }
    else if (!strcmp(module, "proc_buddyinfo")) {
        test_module(TOPIARY_PROC_BUDDYINFO);
    }
    else if (!strcmp(module, "proc_cgroups")) {
        test_module(TOPIARY_PROC_CGROUPS);
    }
    else if (!strcmp(module, "proc_cmdline")) {
        test_module(TOPIARY_PROC_CMDLINE);
    }
    else if (!strcmp(module, "proc_cpuinfo")) {
        test_module(TOPIARY_PROC_CPUINFO);
    }
    else if (!strcmp(module, "proc_diskstats")) {
        test_module(TOPIARY_PROC_DISKSTATS);
    }
    else if (!strcmp(module, "proc_domainname")) {
        test_module(TOPIARY_PROC_DOMAINNAME);
    }
    else if (!strcmp(module, "proc_hostname")) {
        test_module(TOPIARY_PROC_HOSTNAME);
    }
    else if (!strcmp(module, "proc_interrupts")) {
        test_module(TOPIARY_PROC_INTERRUPTS);
    }
    else if (!strcmp(module, "proc_loadavg")) {
        test_module(TOPIARY_PROC_LOADAVG);
    }
    else if (!strcmp(module, "proc_meminfo")) {
        test_module(TOPIARY_PROC_MEMINFO);
    }
    else if (!strcmp(module, "proc_mounts")) {
        test_module(TOPIARY_PROC_MOUNTS);
    }
    else if (!strcmp(module, "proc_net_dev")) {
        test_module(TOPIARY_PROC_NET_DEV);
    }
    else if (!strcmp(module, "proc_net_netstat")) {
        test_module(TOPIARY_PROC_NET_NETSTAT);
    }
    else if (!strcmp(module, "proc_net_rpc_nfs")) {
        test_module(TOPIARY_PROC_NET_RPC_NFS);
    }
    else if (!strcmp(module, "proc_net_rpc_nfsd")) {
        test_module(TOPIARY_PROC_NET_RPC_NFSD);
    }
    else if (!strcmp(module, "proc_net_snmp")) {
        test_module(TOPIARY_PROC_NET_SNMP);
    }
    else if (!strcmp(module, "proc_net_snmp6")) {
        test_module(TOPIARY_PROC_NET_SNMP6);
    }
    else if (!strcmp(module, "proc_net_sockstat")) {
        test_module(TOPIARY_PROC_NET_SOCKSTAT);
    }
    else if (!strcmp(module, "proc_osrelease")) {
        test_module(TOPIARY_PROC_OSRELEASE);
    }
    else if (!strcmp(module, "proc_partitions")) {
        test_module(TOPIARY_PROC_PARTITIONS);
    }
    else if (!strcmp(module, "proc_pid_environ")) {
        test_module(TOPIARY_PROC_PID_ENVIRON);
        test_pid_module(TOPIARY_PROC_PID_ENVIRON,pid);
    }
    else if (!strcmp(module, "proc_pid_io")) {
        test_module(TOPIARY_PROC_PID_IO);
        test_pid_module(TOPIARY_PROC_PID_IO,pid);
    }
    else if (!strcmp(module, "proc_pid_smaps")) {
        test_module(TOPIARY_PROC_PID_SMAPS);
        test_pid_module(TOPIARY_PROC_PID_SMAPS,pid);
    }
    else if (!strcmp(module, "proc_pid_stat")) {
        test_module(TOPIARY_PROC_PID_STAT);
        test_pid_module(TOPIARY_PROC_PID_STAT,pid);
        test_master_module(TOPIARY_PROC_PID_STAT, "chrome");
        test_mod_module(TOPIARY_PROC_PID_STAT, "chrome",
                        TOPIARY_PROC_PID_STATM);
    }
    else if (!strcmp(module, "proc_pid_statm")) {
        test_module(TOPIARY_PROC_PID_STATM);
        test_pid_module(TOPIARY_PROC_PID_STATM,pid);
    }
    else if (!strcmp(module, "proc_pid_status")) {
        test_module(TOPIARY_PROC_PID_STATUS);
        test_pid_module(TOPIARY_PROC_PID_STATUS,pid);
    }
    else if (!strcmp(module, "proc_softirqs")) {
        test_module(TOPIARY_PROC_SOFTIRQS);
    }
    else if (!strcmp(module, "proc_stat")) {
        test_module(TOPIARY_PROC_STAT);
    }
    else if (!strcmp(module, "proc_sys_fs_file_nr")) {
        test_module(TOPIARY_PROC_SYS_FS_FILE_NR);
    }
    else if (!strcmp(module, "proc_uptime")) {
        test_module(TOPIARY_PROC_UPTIME);
    }
    else if (!strcmp(module, "proc_vmstat")) {
        test_module(TOPIARY_PROC_VMSTAT);
    }
    else if (!strcmp(module, "sys_cpufreq")) {
        test_module(TOPIARY_SYS_CPUFREQ);
    }
    else if (!strcmp(module, "sys_disksectors")) {
        test_module(TOPIARY_SYS_DISKSECTORS);
        test_glob_module(TOPIARY_SYS_DISKSECTORS, "sd*");
    }
    return 0;
}
