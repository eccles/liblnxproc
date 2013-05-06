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
static void
test_module(LNXPROC_MODULE_T *modules, LNXPROC_MODULE_TYPE_T type)
{
    if( modules) {
        int i;
        for( i = 0; i < ntimes; i++ ) {
            lnxproc_read(modules,type);
        }
    }
    LNXPROC_FREE(modules);
}
/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    LNXPROC_MODULE_T *modules = NULL;
    lnxproc_init(&modules);

    if (argc < 2) {
        test_module(modules,LNXPROC_ALL);
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_module(modules,LNXPROC_PROC_CGROUPS);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        test_module(modules,LNXPROC_PROC_DISKSTATS);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        test_module(modules,LNXPROC_PROC_DOMAINNAME);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        test_module(modules,LNXPROC_PROC_HOSTNAME);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        test_module(modules,LNXPROC_PROC_OSRELEASE);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        test_module(modules,LNXPROC_SYS_CPUFREQ);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        test_module(modules,LNXPROC_SYS_DISKSECTORS);
    }
    return 0;
}
