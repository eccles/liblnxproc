#ifndef TOPIARY_INTERFACE_H
#define TOPIARY_INTERFACE_H 1

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

#include <topiary/util.h>
#include <topiary/error.h>
#include <topiary/print.h>
#include <topiary/opt.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup topiary The topiary API
 *
 * topiary is an interface to the /proc,/sys and other system files that reads
 * parameters and calculates (if appropriate) rates and usage statistics of a
 * Linux system.
 *
 * @section topiary_interface Interface
 *
 * Similar in design to the pthreads library. That is all structures are 
 * opaque and must be created by suitable function calls.
 *
 * @{
 */

/**
 * @brief Indicates which topiary module is selected
 *
 * WARN: must be in same order as table in interface.c 
 * only add new modules to end of enum to preserve a consistent API
 *
 * TOPIARY_ALL is used *only* for testing.
 *
 * TOPIARY_LAST is used *only* as an end marker.
 *
 * @see topiary_new()
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

/** A convenient typedef for the enum topiary_module_type_t */
    typedef enum topiary_module_type_t TOPIARY_MODULE_TYPE_T;

/** A convenient typedef for the struct topiary_module_t */
    typedef struct topiary_module_t TOPIARY_MODULE_T;

/**
 * @brief Create a module.
 *
 * @param[in]  modulesptr  Address of pointer to module - must contain NULL if
 *                         a new structure is required.
 *
 * @param[in]  nmodule The number of submodules required.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_new(TOPIARY_MODULE_T **modulesptr, size_t nmodule);

/**
 * @brief Free a module.
 *
 * @param[in]  modulesptr   Address of pointer to module - modules reference
 *                         count is decremented\. If reference count is 0 the 
 *                         module structure is freed.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_free(TOPIARY_MODULE_T **modulesptr);

/**
 * @brief Set attribute for one row of the module.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[in]  pos   Position - from 0 to nmodule-1.
 *
 * @param[in]  type Indicates which submodule to map to this row of the
 *                  module.
 *
 * @see topiary_module_type_t
 *
 * @param[in]  optional Address of optional structure.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 *
 */
    int topiary_set(TOPIARY_MODULE_T *modules, size_t pos,
                    TOPIARY_MODULE_TYPE_T type, TOPIARY_OPT_T *optional);
/**
 * @brief Gets tag for one row of the module.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[in]  pos   Position - from 0 to nmodule-1.
 *
 * @param[out]  tag Address of character string that tags row.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_tag(TOPIARY_MODULE_T *modules, size_t pos, const char **tag);

/**
 * @brief Read data for all submodules.
 *
 * @param[in]  modules   Address of modules.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_read(TOPIARY_MODULE_T *modules);

/**
 * @brief Print data for all submodules from last read operation.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[in]  fd   File descriptor to output results.
 *
 * @param[in]  print   Output type - VANILLA or JSON.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_print(TOPIARY_MODULE_T *modules, int fd, TOPIARY_PRINT_T print);

/**
 * @brief Print performance.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[out]  rawread_time   Address of where to put time to read data
 *
 * @param[out]  map_time   Address of where to put time to map data to array.
 *
 * @param[out]  normalize_time   Address of where to put time to normalize 
 *                              data i.e. convert to standard units and 
 *                              calculate rates and usage.
 *
 * @param[out]  hash_time   Address of where to put time to create hash map.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_performance(TOPIARY_MODULE_T *modules,
                            long *rawread_time,
                            long *map_time,
                            long *normalize_time, long *hash_time);

/**
 * @brief Print memory consumption.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[out]  size   Address of where to put used RAM.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_size(TOPIARY_MODULE_T *modules, size_t * size );

/**
 * @brief Callback function type.
 *
 * Functions of this type are called for every data type read from the
 * source when executing the topiary_iterate() function.
 *
 * @param[in]  mod  ?.
 *
 * @param[in]  key  Hash map key of data value.
 *
 * @param[in]  value Value of the data as a string.
 *
 * @param[in]  data  Optional data structure that is passed to the 
 *                   callback function unmodified.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    typedef int (*TOPIARY_INTERFACE_METHOD) (char *mod, char *key, char *value,
                                             void *data);

/**
 * @brief Iterate over all values from last read operation.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[in]  func  Callback function.
 *
 * @param[in]  data  Optional data structure that is passed to the 
 *                   callback function unmodified.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_iterate(TOPIARY_MODULE_T *modules,
                        TOPIARY_INTERFACE_METHOD func, void *data);

/**
 * @brief Fetch data value.
 *
 * @param[in]  modules   Address of modules.
 *
 * @param[in]  type Indicates which submodule.
 *
 * @param[in]  key  Data hash key.
 *
 * @param[out]  value  Location to store value.
 *
 * @param[in]  valuelen  Length of value buffer.
 *
 * @param[out]  pbuf  Contains pointer to string containing value.
 *
 * @return              A TOPIARY_ERROR value - 0 is success.
 */
    int topiary_fetch(TOPIARY_MODULE_T *modules, TOPIARY_MODULE_TYPE_T type,
                      char *key, char *value, size_t valuelen, char **pbuf);

/** @} ******************************************************************/

#ifdef __cplusplus
}                               // extern "C"
#endif
#endif                          // TOPIARY_INTERFACE_H
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
