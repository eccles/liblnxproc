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
        LNXPROC_SYS_CPUFREQ,
        LNXPROC_SYS_DISKSECTORS,
        LNXPROC_PROC_PID_STAT,
    };
    typedef enum lnxproc_module_type_t LNXPROC_MODULE_TYPE_T;

    typedef struct lnxproc_module_t LNXPROC_MODULE_T;

    int lnxproc_new(LNXPROC_MODULE_T ** moduleptr, size_t nmodule);
    int lnxproc_set(LNXPROC_MODULE_T * module, size_t pos,
                    LNXPROC_MODULE_TYPE_T type, void *optional, size_t optlen);
    int lnxproc_free(LNXPROC_MODULE_T ** modulesptr);

#define LNXPROC_FREE(b) {\
    lnxproc_free(&b);\
}

    int lnxproc_read(LNXPROC_MODULE_T * modules);

    int lnxproc_print(LNXPROC_MODULE_T * modules);

    int lnxproc_performance(LNXPROC_MODULE_T * modules,
                            long *rawread_time,
                            long *map_time, long *hash_time,
                            long *normalize_time);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
