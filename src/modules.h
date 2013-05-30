/*
 * This file is part of liblnxproc.
 *
 *  liblnxproc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  liblnxproc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#ifndef LIBLNXPROC_MODULES_H
#define LIBLNXPROC_MODULES_H 1

#include "base_private.h"

int _lnxproc_proc_buddyinfo_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_cgroups_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_cmdline_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_cpuinfo_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_diskstats_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_domainname_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_hostname_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_interrupts_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_loadavg_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_meminfo_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_mounts_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_osrelease_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_partitions_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_pid_environ_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_pid_io_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_pid_stat_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_pid_statm_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_pid_status_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_softirqs_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_stat_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_uptime_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_proc_vmstat_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_sys_cpufreq_new(_LNXPROC_BASE_T ** base, void *optional);
int _lnxproc_sys_disksectors_new(_LNXPROC_BASE_T ** base, void *optional);

#endif                          // LIBLNXPROC_MODULES_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
