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

#ifndef TOPIARY_MODULES_H
#define TOPIARY_MODULES_H 1

#include "interface_private.h"
#include "base_private.h"

int _topiary_ntp_drift_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_buddyinfo_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_cgroups_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_cmdline_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_cpuinfo_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_diskstats_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_domainname_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional);
int _topiary_proc_hostname_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_interrupts_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional);
int _topiary_proc_loadavg_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_meminfo_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_mounts_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_net_dev_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_net_netstat_new(_TOPIARY_BASE_T **base,
                                  TOPIARY_OPT_T *optional);
int _topiary_proc_net_rpc_nfs_new(_TOPIARY_BASE_T **base,
                                  TOPIARY_OPT_T *optional);
int _topiary_proc_net_rpc_nfsd_new(_TOPIARY_BASE_T **base,
                                   TOPIARY_OPT_T *optional);
int _topiary_proc_net_snmp_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_net_snmp6_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_net_sockstat_new(_TOPIARY_BASE_T **base,
                                   TOPIARY_OPT_T *optional);
int _topiary_proc_osrelease_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_partitions_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional);
int _topiary_proc_pid_environ_new(_TOPIARY_BASE_T **base,
                                  TOPIARY_OPT_T *optional);
int _topiary_proc_pid_io_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_pid_smaps_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_pid_stat_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_pid_statm_new(_TOPIARY_BASE_T **base,
                                TOPIARY_OPT_T *optional);
int _topiary_proc_pid_status_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional);
int _topiary_proc_softirqs_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_stat_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_sys_fs_file_nr_new(_TOPIARY_BASE_T **base,
                                     TOPIARY_OPT_T *optional);
int _topiary_proc_uptime_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_proc_vmstat_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_sys_cpufreq_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional);
int _topiary_sys_disksectors_new(_TOPIARY_BASE_T **base,
                                 TOPIARY_OPT_T *optional);

#endif                          // TOPIARY_MODULES_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
