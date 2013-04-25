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
/*----------------------------------------------------------------------------*/
static void
execute_base(LNXPROC_BASE_T *base)
{
    if (base) {
        int i;
        for( i = 0; i < ntimes; i++ ) {
            LNXPROC_RESULTS_T *res = lnxproc_base_read(base);

            if (!res) {
                printf("Failure reading base\n");
            }
            else {
                LNXPROC_RESULTS_FREE(res);
            }
        }
        LNXPROC_BASE_FREE(base);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_cgroups(void)
{
    LNXPROC_BASE_T *proc_cgroups = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_cgroups_new(&proc_cgroups);

    if (ret == LNXPROC_OK) {
        execute_base(proc_cgroups);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_diskstats(void)
{
    LNXPROC_BASE_T *proc_diskstats = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_diskstats_new(&proc_diskstats);

    if (ret == LNXPROC_OK) {
        execute_base(proc_diskstats);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_domainname(void)
{
    LNXPROC_BASE_T *proc_domainname = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_domainname_new(&proc_domainname);

    if (ret == LNXPROC_OK) {
        execute_base(proc_domainname);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_hostname(void)
{
    LNXPROC_BASE_T *proc_hostname = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_hostname_new(&proc_hostname);

    if (ret == LNXPROC_OK) {
        execute_base(proc_hostname);
        LNXPROC_BASE_FREE(proc_hostname);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_proc_osrelease(void)
{
    LNXPROC_BASE_T *proc_osrelease = NULL;
    LNXPROC_ERROR_T ret = lnxproc_proc_osrelease_new(&proc_osrelease);

    if (ret == LNXPROC_OK) {
        execute_base(proc_osrelease);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_sys_cpufreq(void)
{
    LNXPROC_BASE_T *sys_cpufreq = NULL;
    LNXPROC_ERROR_T ret = lnxproc_sys_cpufreq_new(&sys_cpufreq);

    if (ret == LNXPROC_OK) {
        execute_base(sys_cpufreq);
    }
}

/*----------------------------------------------------------------------------*/
static void
test_sys_disksectors(void)
{
    LNXPROC_BASE_T *sys_disksectors = NULL;
    LNXPROC_ERROR_T ret = lnxproc_sys_disksectors_new(&sys_disksectors);

    if (ret == LNXPROC_OK) {
        execute_base(sys_disksectors);
    }
}

/*----------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    if (argc < 2) {
        test_proc_cgroups();
        test_proc_diskstats();
        test_proc_domainname();
        test_proc_hostname();
        test_proc_osrelease();
        test_sys_cpufreq();
        test_sys_disksectors();
    }
    else if (!strcmp(argv[1], "proc_cgroups")) {
        test_proc_cgroups();
    }
    else if (!strcmp(argv[1], "proc_diskstats")) {
        test_proc_diskstats();
    }
    else if (!strcmp(argv[1], "proc_domainname")) {
        test_proc_domainname();
    }
    else if (!strcmp(argv[1], "proc_hostname")) {
        test_proc_hostname();
    }
    else if (!strcmp(argv[1], "proc_osrelease")) {
        test_proc_osrelease();
    }
    else if (!strcmp(argv[1], "sys_cpufreq")) {
        test_sys_cpufreq();
    }
    else if (!strcmp(argv[1], "sys_disksectors")) {
        test_sys_disksectors();
    }
    return 0;
}
