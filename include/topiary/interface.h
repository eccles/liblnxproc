/*
 * This file is part of topiary.
 *
 *  topiary is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  topiary is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with topiary.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#ifndef TOPIARY_INTERFACE_H
#define TOPIARY_INTERFACE_H 1

#include <topiary/util.h>
#include <topiary/error.h>
#include <topiary/print.h>
#include <topiary/opt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * must be in same order as table in interface.c 
 * only add to end of enum to preserve a consistent API
 */

    enum topiary_module_type_t {
        TOPIARY_ALL = 0,
        TOPIARY_PROC_CGROUPS,
        TOPIARY_PROC_DISKSTATS,
        TOPIARY_PROC_DOMAINNAME,
        TOPIARY_PROC_HOSTNAME,
        TOPIARY_PROC_OSRELEASE,
        TOPIARY_PROC_PID_ENVIRON,
        TOPIARY_PROC_PID_STAT,
        TOPIARY_SYS_CPUFREQ,
        TOPIARY_SYS_DISKSECTORS,
        TOPIARY_PROC_BUDDYINFO,
        TOPIARY_PROC_CMDLINE,
        TOPIARY_PROC_CPUINFO,
        TOPIARY_PROC_INTERRUPTS,
        TOPIARY_PROC_LOADAVG,
        TOPIARY_PROC_MEMINFO,
        TOPIARY_PROC_MOUNTS,
        TOPIARY_PROC_PARTITIONS,
        TOPIARY_PROC_SOFTIRQS,
        TOPIARY_PROC_STAT,
        TOPIARY_PROC_UPTIME,
        TOPIARY_PROC_VMSTAT,
        TOPIARY_PROC_PID_IO,
        TOPIARY_PROC_PID_STATUS,
        TOPIARY_PROC_PID_STATM,
        TOPIARY_PROC_NET_DEV,
        TOPIARY_PROC_NET_RPC_NFS,
        TOPIARY_PROC_NET_RPC_NFSD,
        TOPIARY_PROC_NET_SNMP,
        TOPIARY_PROC_NET_NETSTAT,
        TOPIARY_PROC_NET_SNMP6,
        TOPIARY_PROC_NET_SOCKSTAT,
        TOPIARY_PROC_SYS_FS_FILE_NR,
        TOPIARY_PROC_PID_SMAPS,
        TOPIARY_NTP_DRIFT,
        TOPIARY_LAST,           // must be last
    };
    typedef enum topiary_module_type_t TOPIARY_MODULE_TYPE_T;

    typedef struct topiary_module_t TOPIARY_MODULE_T;

    int topiary_new(TOPIARY_MODULE_T **moduleptr, size_t nmodule);
    int topiary_size(TOPIARY_MODULE_T *module, size_t * size);
    int topiary_set(TOPIARY_MODULE_T *module, size_t pos,
                    TOPIARY_MODULE_TYPE_T type, TOPIARY_OPT_T *optional);
    int topiary_tag(TOPIARY_MODULE_T *module, size_t pos, const char **tag);
    int topiary_free(TOPIARY_MODULE_T **modulesptr);

#define TOPIARY_FREE(b) topiary_free(&b)

    int topiary_read(TOPIARY_MODULE_T *modules);

    int topiary_print(TOPIARY_MODULE_T *modules, int fd, TOPIARY_PRINT_T print);

    int topiary_performance(TOPIARY_MODULE_T *modules,
                            long *rawread_time,
                            long *map_time,
                            long *normalize_time, long *hash_time);

    typedef int (*TOPIARY_INTERFACE_METHOD) (char *mod, char *key, char *value,
                                             void *data);
    int topiary_iterate(TOPIARY_MODULE_T *modules,
                        TOPIARY_INTERFACE_METHOD func, void *data);

    int topiary_fetch(TOPIARY_MODULE_T *modules, TOPIARY_MODULE_TYPE_T type,
                      char *key, char *value, size_t valuelen, char **pbuf);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
