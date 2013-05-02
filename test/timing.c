/*
This file is part of liblnxproc.

    liblnxproc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    liblnxproc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2013 Paul Hewlett, phewlett76@gmail.com

*/

#include <stdio.h>
#include <string.h>
#include <lnxproc/lnxproc.h>

static const int ntimes = 1000;
struct lnxproc_module_t {
    LNXPROC_INTERFACE_METHOD new;
    LNXPROC_INTERFACE_T *interface;
};
typedef struct lnxproc_module_t LNXPROC_MODULE_T;
/*----------------------------------------------------------------------------*/
static void
execute_interface(LNXPROC_INTERFACE_T *interface)
{
    if (interface) {
        int i;
        for( i = 0; i < ntimes; i++ ) {
            LNXPROC_RESULTS_T *res = interface->read(interface->base);

            if (!res) {
                printf("Failure reading base\n");
            }
            else {
                LNXPROC_RESULTS_FREE(res);
            }
        }
        LNXPROC_INTERFACE_FREE(interface);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_module(LNXPROC_MODULE_T *module)
{
    if( module) {
        LNXPROC_ERROR_T ret = module->new(&module->interface);
        if (ret == LNXPROC_OK) {
            execute_interface(module->interface);
        }
    }
}
/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    LNXPROC_MODULE_T modules[] = {
        { .new = lnxproc_proc_cgroups_new, .interface = NULL, },
        { .new = lnxproc_proc_diskstats_new, .interface = NULL, },
        { .new = lnxproc_proc_domainname_new, .interface = NULL, },
        { .new = lnxproc_proc_hostname_new, .interface = NULL, },
        { .new = lnxproc_proc_osrelease_new, .interface = NULL, },
        { .new = lnxproc_sys_cpufreq_new, .interface = NULL, },
        { .new = lnxproc_sys_disksectors_new, .interface = NULL, },
    };
    size_t nmodules = sizeof(modules)/sizeof(modules[0]);

    if (argc < 2) {
        int i;
        for( i = 0 ; i < nmodules ; i++ ) {
            test_module(modules+i);
        }
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_module(modules+0);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        test_module(modules+1);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        test_module(modules+2);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        test_module(modules+3);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        test_module(modules+4);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        test_module(modules+5);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        test_module(modules+6);
    }
    return 0;
}
