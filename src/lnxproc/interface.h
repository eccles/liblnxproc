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

#ifdef __cplusplus
extern "C" {
#endif

#include <lnxproc/util.h>
#include <lnxproc/error.h>
#include <lnxproc/base.h>
#include <lnxproc/results.h>

    enum lnxproc_module_type_t {
        LNXPROC_ALL = 0,
        LNXPROC_PROC_CGROUPS,
        LNXPROC_PROC_DISKSTATS,
        LNXPROC_PROC_DOMAINNAME,
        LNXPROC_PROC_HOSTNAME,
        LNXPROC_PROC_OSRELEASE,
        LNXPROC_SYS_CPUFREQ,
        LNXPROC_SYS_DISKSECTORS,
    };
    typedef enum lnxproc_module_type_t LNXPROC_MODULE_TYPE_T;

    typedef LNXPROC_ERROR_T (*LNXPROC_METHOD) (LNXPROC_BASE_T **base);

    typedef struct lnxproc_module_t LNXPROC_MODULE_T;

    LNXPROC_ERROR_T lnxproc_init(LNXPROC_MODULE_T ** modules);

    LNXPROC_MODULE_T *lnxproc_free(LNXPROC_MODULE_T * module) WARN_UNUSED;

#define LNXPROC_FREE(b) {\
    b = lnxproc_free(b);\
}

    LNXPROC_RESULTS_T *lnxproc_read(LNXPROC_MODULE_T * modules,
                                    LNXPROC_MODULE_TYPE_T type);

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // LIBLNXPROC_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
