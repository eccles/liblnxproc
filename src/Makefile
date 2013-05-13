RELDIR=..
PUBHDIR=../src/lnxproc
FUNCHDRS=limit_chr.h 
PUBBASEHDRS=lnxproc.h util.h base.h error.h interface.h
PROCHDRS=proc_cgroups.h proc_diskstats.h proc_domainname.h proc_hostname.h proc_osrelease.h
PROCPIDHDRS=proc_pid_stat.h
SYSHDRS=sys_cpufreq.h sys_disksectors.h
PUBHDRS=$(PUBBASEHDRS)
PRIVHDRS=util_private.h base_private.h error_private.h interface_private.h limits_private.h vector_private.h array_private.h results_private.h 
HDRS=$(PRIVHDRS) $(PROCHDRS) $(PROCPIDHDRS) $(SYSHDRS) $(FUNCHDRS)
LIB=liblnxproc
BASESRCS=util.c error.c results.c limits.c vector.c array.c base.c interface.c
PROCSRCS=proc_cgroups.c proc_diskstats.c proc_domainname.c proc_hostname.c proc_osrelease.c
PROCPIDSRCS=proc_pid_stat.c
SYSSRCS=sys_cpufreq.c sys_disksectors.c
SRCS=$(BASESRCS) $(PROCSRCS) $(PROCPIDSRCS) $(SYSSRCS)
DIRS=
BIN=test-lnxproc

include $(RELDIR)/Makefile.generic

