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

#include <string.h>             //memcpy

#include "allocate.h"
#include "io.h"
#include "memdup.h"
#include "strlcpy.h"
#include "error_private.h"
#include "print_private.h"
#include "base_private.h"
#include "opt_private.h"
#include "interface_private.h"
#include "modules.h"

/*
 * must be in same order as enum in interface.h
 * only add to end to preserve consistent API
 */
static _TOPIARY_MODULE_ROW_T mymodules[] = {
    {.tag = "proc_cgroups",.type = TOPIARY_PROC_CGROUPS,.new =
     _topiary_proc_cgroups_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_diskstats",.type = TOPIARY_PROC_DISKSTATS,.new =
     _topiary_proc_diskstats_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_domainname",.type = TOPIARY_PROC_DOMAINNAME,.new =
     _topiary_proc_domainname_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_hostname",.type = TOPIARY_PROC_HOSTNAME,.new =
     _topiary_proc_hostname_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_osrelease",.type = TOPIARY_PROC_OSRELEASE,.new =
     _topiary_proc_osrelease_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_environ",.type = TOPIARY_PROC_PID_ENVIRON,.new =
     _topiary_proc_pid_environ_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_stat",.type = TOPIARY_PROC_PID_STAT,.new =
     _topiary_proc_pid_stat_new,.base = NULL,.optional = NULL,},
    {.tag = "sys_cpufreq",.type = TOPIARY_SYS_CPUFREQ,.new =
     _topiary_sys_cpufreq_new,.base = NULL,.optional = NULL,},
    {.tag = "sys_disksectors",.type = TOPIARY_SYS_DISKSECTORS,.new =
     _topiary_sys_disksectors_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_buddyinfo",.type = TOPIARY_PROC_BUDDYINFO,.new =
     _topiary_proc_buddyinfo_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_cmdline",.type = TOPIARY_PROC_CMDLINE,.new =
     _topiary_proc_cmdline_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_cpuinfo",.type = TOPIARY_PROC_CPUINFO,.new =
     _topiary_proc_cpuinfo_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_interrupts",.type = TOPIARY_PROC_INTERRUPTS,.new =
     _topiary_proc_interrupts_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_loadavg",.type = TOPIARY_PROC_LOADAVG,.new =
     _topiary_proc_loadavg_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_meminfo",.type = TOPIARY_PROC_MEMINFO,.new =
     _topiary_proc_meminfo_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_mounts",.type = TOPIARY_PROC_MOUNTS,.new =
     _topiary_proc_mounts_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_partitions",.type = TOPIARY_PROC_PARTITIONS,.new =
     _topiary_proc_partitions_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_softirqs",.type = TOPIARY_PROC_SOFTIRQS,.new =
     _topiary_proc_softirqs_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_stat",.type = TOPIARY_PROC_STAT,.new =
     _topiary_proc_stat_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_uptime",.type = TOPIARY_PROC_UPTIME,.new =
     _topiary_proc_uptime_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_vmstat",.type = TOPIARY_PROC_VMSTAT,.new =
     _topiary_proc_vmstat_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_io",.type = TOPIARY_PROC_PID_IO,.new =
     _topiary_proc_pid_io_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_status",.type = TOPIARY_PROC_PID_STATUS,.new =
     _topiary_proc_pid_status_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_statm",.type = TOPIARY_PROC_PID_STATM,.new =
     _topiary_proc_pid_statm_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_dev",.type = TOPIARY_PROC_NET_DEV,.new =
     _topiary_proc_net_dev_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_rpc_nfs",.type = TOPIARY_PROC_NET_RPC_NFS,.new =
     _topiary_proc_net_rpc_nfs_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_rpc_nfsd",.type = TOPIARY_PROC_NET_RPC_NFSD,.new =
     _topiary_proc_net_rpc_nfsd_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_snmp",.type = TOPIARY_PROC_NET_SNMP,.new =
     _topiary_proc_net_snmp_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_netstat",.type = TOPIARY_PROC_NET_NETSTAT,.new =
     _topiary_proc_net_netstat_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_snmp6",.type = TOPIARY_PROC_NET_SNMP6,.new =
     _topiary_proc_net_snmp6_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_net_sockstat",.type = TOPIARY_PROC_NET_SOCKSTAT,.new =
     _topiary_proc_net_sockstat_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_sys_fs_file_nr",.type = TOPIARY_PROC_SYS_FS_FILE_NR,.new =
     _topiary_proc_sys_fs_file_nr_new,.base = NULL,.optional = NULL,},
    {.tag = "proc_pid_smaps",.type = TOPIARY_PROC_PID_SMAPS,.new =
     _topiary_proc_pid_smaps_new,.base = NULL,.optional = NULL,},
    {.tag = "ntp_drift",.type = TOPIARY_NTP_DRIFT,.new =
     _topiary_ntp_drift_new,.base = NULL,.optional = NULL,},
};

static size_t nmodules = sizeof(mymodules) / sizeof(mymodules[0]);

int
topiary_new(TOPIARY_MODULE_T **moduleptr, size_t nmodule)
{
    if (!moduleptr) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module address");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    TOPIARY_MODULE_T *p = *moduleptr;

    if (p) {
        Acquire(p, 0);
        return TOPIARY_OK;
    }

    if (nmodules < 0) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module: nmodules < 0");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (nmodule == 0) {
        p = Acquire(NULL,
                    sizeof(TOPIARY_MODULE_T) +
                    (nmodules * sizeof(_TOPIARY_MODULE_ROW_T)));
    }
    else {
        p = Acquire(NULL,
                    sizeof(TOPIARY_MODULE_T) +
                    (nmodule * sizeof(_TOPIARY_MODULE_ROW_T)));
    }

    if (!p) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_MALLOC, "Module");
        return TOPIARY_ERROR_MALLOC;
    }
    if (nmodule == 0) {
        p->nmodules = nmodules;
        memcpy(p->row, mymodules, nmodules * sizeof(_TOPIARY_MODULE_ROW_T));
    }
    else {
        p->nmodules = nmodule;
    }
    *moduleptr = p;
    return TOPIARY_OK;
}

int
topiary_size(TOPIARY_MODULE_T *modules, size_t * size)
{
    if (!size) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Size");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size = 0;
    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *size += sizeof(TOPIARY_MODULE_T) +
        (modules->nmodules * sizeof(_TOPIARY_MODULE_ROW_T));
    int i;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            if (row->base) {
                size_t s;

                _topiary_base_size(row->base, &s);
                *size += s;
            }
            if (row->optional) {
                *size += sizeof(*row->optional);
                if (row->optional->fileglob) {
                    *size += 1 + strlen(row->optional->fileglob);
                }
                if (row->optional->master) {
                    *size += 1 + strlen(row->optional->master);
                }
                if (row->optional->module) {
                    size_t s;

                    topiary_size(row->optional->module, &s);
                    *size += s;
                }
            }
        }
    }
    return TOPIARY_OK;
}

int
topiary_tag(TOPIARY_MODULE_T *module, size_t pos, const char **tag)
{
    if (!tag) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Tag");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *tag = NULL;
    if (!module) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (pos < 0 || pos >= module->nmodules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Pos = %zd", pos);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    _TOPIARY_MODULE_ROW_T *row = module->row + pos;

    *tag = row->tag;
    return TOPIARY_OK;
}

int
topiary_set(TOPIARY_MODULE_T *module, size_t pos,
            TOPIARY_MODULE_TYPE_T type, TOPIARY_OPT_T *optional)
{
    if (!module) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (pos < 0 || pos >= module->nmodules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Pos = %zd", pos);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (type < 1 || type >= TOPIARY_LAST) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Type = %d", type);
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    _TOPIARY_MODULE_ROW_T *row = module->row + pos;

    if (row->base) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Module already used");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    if (row->optional) {
        topiary_opt_free(&row->optional);
    }

    memcpy(row, mymodules + type - 1, sizeof(_TOPIARY_MODULE_ROW_T));

    if (optional) {
        topiary_opt_new(&optional);
        row->optional = optional;
    }
    return TOPIARY_OK;
}

static void
topiary_release(void *arg)
{
    if (arg) {
        TOPIARY_MODULE_T *modules = arg;
        int i;

        for (i = 0; i < modules->nmodules; i++) {
            _TOPIARY_MODULE_ROW_T *row = modules->row + i;

            _TOPIARY_BASE_FREE(row->base);
            topiary_opt_free(&row->optional);
        }
    }
}

int
topiary_free(TOPIARY_MODULE_T **modulesptr)
{
    if (modulesptr && *modulesptr) {
        TOPIARY_MODULE_T *modules = *modulesptr;

        RELEASE(modules, topiary_release);
        *modulesptr = NULL;
    }
    return TOPIARY_OK;
}

int
_topiary_create(TOPIARY_MODULE_T *modules)
{

    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int i;
    int ret;
    int ret1 = TOPIARY_OK;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            if (!row->base) {
                ret = row->new(&row->base, row->optional);

                if (ret) {
                    _TOPIARY_ERROR_DEBUG(ret, "Module %d Type %d", i,
                                         row->type);
                    ret1 = ret;
                }
            }
        }
    }
    return ret1;
}

int
_topiary_set_fileglob(TOPIARY_MODULE_T *modules, char *fileglob)
{

    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int i;
    int ret;
    int ret1 = TOPIARY_OK;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            if (row->base) {
                ret = _topiary_base_set_fileglob(row->base, fileglob);

                if (ret) {
                    _TOPIARY_ERROR_DEBUG(ret, "Module %d Type %d", i,
                                         row->type);
                    ret1 = ret;
                }
            }
        }
    }
    return ret1;
}

int
topiary_read(TOPIARY_MODULE_T *modules)
{

    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int i;
    int ret;
    int ret1 = TOPIARY_OK;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            if (!row->base) {
                ret = row->new(&row->base, row->optional);

                if (ret) {
                    _TOPIARY_ERROR_DEBUG(ret, "Module %d Type %d", i,
                                         row->type);
                    ret1 = ret;
                }
            }
            if (row->base) {
                _TOPIARY_BASE_T *base = row->base;

                ret = _topiary_base_rawread(base);
                if (ret) {
                    _TOPIARY_ERROR_DEBUG(ret, "Module %d Type %d", i,
                                         row->type);
                    ret1 = ret;
                }
            }
        }
    }
    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            _TOPIARY_BASE_T *base = row->base;

            if (base) {

                ret = _topiary_base_normalize(base);
                if (ret) {
                    _TOPIARY_ERROR_DEBUG(ret, "Module %d Type %d", i,
                                         row->type);
                    ret1 = ret;
                }
            }
        }
    }
    return ret1;
}

int
topiary_performance(TOPIARY_MODULE_T *modules,
                    long *rawread_time, long *map_time,
                    long *normalize_time, long *hash_time)
{

    *rawread_time = 0;
    *map_time = 0;
    *normalize_time = 0;
    *hash_time = 0;

    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int i;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            _TOPIARY_BASE_T *base = row->base;

            if (base) {
                _TOPIARY_BASE_DATA_T *base_data = base->current;

                if (base_data) {
                    *rawread_time += base_data->rawread_time;
                    *map_time += base_data->map_time;
                    *normalize_time += base_data->normalize_time;
                    *hash_time += base_data->hash_time;
                }
            }
        }
    }
    return TOPIARY_OK;
}

struct interface_env_t {
    TOPIARY_INTERFACE_METHOD func;
    void *data;
};
static int
results_iterate(_TOPIARY_RESULTS_T *results,
                _TOPIARY_RESULTS_TABLE_T *entry, void *data)
{
    struct interface_env_t *env = data;
    TOPIARY_INTERFACE_METHOD func = env->func;
    char buf[32];
    char *pbuf;

    _topiary_results_table_valuestr(entry, buf, sizeof buf, &pbuf);

    func(results->tag, entry->key, pbuf, env->data);
    return TOPIARY_OK;
}

int
topiary_iterate(TOPIARY_MODULE_T *modules, TOPIARY_INTERFACE_METHOD func,
                void *data)
{
    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    int i;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            _TOPIARY_BASE_T *base = row->base;

            if (base) {
                _TOPIARY_BASE_DATA_T *base_data = base->current;

                struct interface_env_t env = {
                    .func = func,
                    .data = data,
                };
                if (base_data) {
                    _topiary_results_iterate(base_data->results,
                                             results_iterate, &env);
                }
                if (row->optional) {
                    if (row->optional->module) {
                        topiary_iterate(row->optional->module, func, data);
                    }
                }
            }
        }
    }
    return TOPIARY_OK;
}

int
topiary_print(TOPIARY_MODULE_T *modules, int fd, TOPIARY_PRINT_T print)
{
    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    int i;

    if (print == TOPIARY_PRINT_JSON) {
        writen(fd, "\n{\n", 3);
    }
    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new) {
            _TOPIARY_BASE_T *base = row->base;

            if (base) {
                _TOPIARY_BASE_DATA_T *base_data = base->current;

                if (base_data) {
                    _topiary_results_print(base_data->results, fd, print);
                }
                if (row->optional) {
                    if (row->optional->module) {
                        topiary_print(row->optional->module, fd, print);
                    }
                }
            }
        }
    }
    if (print == TOPIARY_PRINT_JSON) {
        writen(fd, "}\n", 2);
    }
    return TOPIARY_OK;
}

int
topiary_fetch(TOPIARY_MODULE_T *modules, TOPIARY_MODULE_TYPE_T type,
              char *key, char *value, size_t valuelen, char **pbuf)
{
    if (!modules) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    if (!pbuf) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Modules");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }
    *pbuf = NULL;
    if (!value || valuelen < 2) {
        _TOPIARY_ERROR_DEBUG(TOPIARY_ERROR_ILLEGAL_ARG, "Fetch buffer");
        return TOPIARY_ERROR_ILLEGAL_ARG;
    }

    int i, ret;

    for (i = 0; i < modules->nmodules; i++) {
        _TOPIARY_MODULE_ROW_T *row = modules->row + i;

        if (row->new && ((type == TOPIARY_ALL) || (row->type == type))) {
            _TOPIARY_BASE_T *base = row->base;

            if (base) {
                _TOPIARY_BASE_DATA_T *base_data = base->current;
                _TOPIARY_RESULTS_TABLE_T *entry = NULL;

                if (base_data) {

                    ret = _topiary_results_fetch(base_data->results, key,
                                                 &entry);

                    if (!ret) {
                        _topiary_results_table_valuestr(entry, value, valuelen,
                                                        pbuf);
                        return TOPIARY_OK;
                    }
                }
                if (row->optional && row->optional->module) {
                    ret = topiary_fetch(row->optional->module, TOPIARY_ALL,
                                        key, value, valuelen, pbuf);
                    if (!ret) {
                        return TOPIARY_OK;
                    }
                }
            }
        }
    }
    value[0] = '\0';
    return TOPIARY_ERROR_NOT_FOUND;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
