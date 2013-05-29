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

#ifndef LIBLNXPROC_INTERFACE_H
#define LIBLNXPROC_INTERFACE_H 1

#include <lnxproc/util.h>
#include <lnxproc/error.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum lnxproc_module_type_t {
        LNXPROC_ALL = 0,
        LNXPROC_PROC_CGROUPS,
        LNXPROC_PROC_DISKSTATS,
        LNXPROC_PROC_DOMAINNAME,
        LNXPROC_PROC_HOSTNAME,
        LNXPROC_PROC_OSRELEASE,
        LNXPROC_PROC_PID_ENVIRON,
        LNXPROC_PROC_PID_STAT,
        LNXPROC_SYS_CPUFREQ,
        LNXPROC_SYS_DISKSECTORS,
        LNXPROC_PROC_BUDDYINFO,
        LNXPROC_PROC_CMDLINE,
        LNXPROC_PROC_CPUINFO,
        LNXPROC_PROC_INTERRUPTS,
        LNXPROC_PROC_LOADAVG,
        LNXPROC_PROC_MEMINFO,
        LNXPROC_PROC_MOUNTS,
        LNXPROC_PROC_PARTITIONS,
        LNXPROC_PROC_SOFTIRQS,
        LNXPROC_LAST,           // must be last
    };
    typedef enum lnxproc_module_type_t LNXPROC_MODULE_TYPE_T;

    typedef struct lnxproc_module_t LNXPROC_MODULE_T;

    int lnxproc_new(LNXPROC_MODULE_T ** moduleptr, size_t nmodule);
    int lnxproc_set(LNXPROC_MODULE_T * module, size_t pos,
                    LNXPROC_MODULE_TYPE_T type, void *optional, size_t optlen);
    int lnxproc_free(LNXPROC_MODULE_T ** modulesptr);

#define LNXPROC_FREE(b) lnxproc_free(&b)

    int lnxproc_read(LNXPROC_MODULE_T * modules);

    int lnxproc_print(LNXPROC_MODULE_T * modules);

    int lnxproc_performance(LNXPROC_MODULE_T * modules,
                            long *rawread_time,
                            long *map_time,
                            long *normalize_time, long *hash_time);

    typedef int (*LNXPROC_INTERFACE_METHOD) (char *mod, char *key, char *value,
                                             void *data);
    int lnxproc_iterate(LNXPROC_MODULE_T * modules,
                        LNXPROC_INTERFACE_METHOD func, void *data);

    int lnxproc_fetch(LNXPROC_MODULE_T * modules, LNXPROC_MODULE_TYPE_T type,
                      char *key, char *value, size_t valuelen);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
