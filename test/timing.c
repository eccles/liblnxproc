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
#include <sys/time.h>
#include <lnxproc/lnxproc.h>

static const int ntimes = 1000;
static int testtype = 0; // timing test
#define TEST_MODULE(m,t)  time_module(m,t, #t)
static void
time_module(LNXPROC_MODULE_T *modules, LNXPROC_MODULE_TYPE_T type, char *str)
{
    if( modules) {

        lnxproc_read(modules,type);

        if( testtype == 0 ) {
            struct timeval start = lnxproc_timeval();

            int i;
            for( i = 0; i < ntimes; i++ ) {
                lnxproc_read(modules,type);
            }

            struct timeval end = lnxproc_timeval();
            long timediff = lnxproc_timeval_diff(&start,&end);
            printf("%d:%s:Elapsed time = %ld usecs\n", type, str, timediff/ntimes);
        }
        else if( testtype == 1 ) {
            lnxproc_read(modules,type);
            printf("%d:%s:Results\n", type, str);
            lnxproc_print(modules,type);
        }
    }
}
/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    printf("Command %s\n", argv[0]);

    if( !strcmp(argv[0],"./timing") ||
        !strcmp(argv[0],"./timing-dbg")) {
        testtype = 0;
    }
    else if( !strcmp(argv[0],"./testing") ||
             !strcmp(argv[0],"./testing-dbg")) {
        testtype = 1;
    }
    else {
        printf("Illegal command %s\n", argv[0]);
        return 1;
    }

    LNXPROC_MODULE_T *modules = NULL;
    lnxproc_init(&modules);

    if (argc < 2) {
        TEST_MODULE(modules,LNXPROC_ALL);
        TEST_MODULE(modules,LNXPROC_PROC_CGROUPS);
        TEST_MODULE(modules,LNXPROC_PROC_DISKSTATS);
        TEST_MODULE(modules,LNXPROC_PROC_DOMAINNAME);
        TEST_MODULE(modules,LNXPROC_PROC_HOSTNAME);
        TEST_MODULE(modules,LNXPROC_PROC_OSRELEASE);
        TEST_MODULE(modules,LNXPROC_SYS_CPUFREQ);
        TEST_MODULE(modules,LNXPROC_SYS_DISKSECTORS);
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        TEST_MODULE(modules,LNXPROC_PROC_CGROUPS);
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        TEST_MODULE(modules,LNXPROC_PROC_DISKSTATS);
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        TEST_MODULE(modules,LNXPROC_PROC_DOMAINNAME);
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        TEST_MODULE(modules,LNXPROC_PROC_HOSTNAME);
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        TEST_MODULE(modules,LNXPROC_PROC_OSRELEASE);
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        TEST_MODULE(modules,LNXPROC_SYS_CPUFREQ);
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        TEST_MODULE(modules,LNXPROC_SYS_DISKSECTORS);
    }
    LNXPROC_FREE(modules);
    return 0;
}
