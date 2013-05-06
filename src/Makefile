RELDIR=..
PUBHDIR=../src/lnxproc
PUBBASEHDRS=lnxproc.h util.h db.h base.h error.h interface.h
PROCHDRS=proc_cgroups.h proc_diskstats.h proc_domainname.h proc_hostname.h proc_osrelease.h
SYSHDRS=sys_cpufreq.h sys_disksectors.h
PUBHDRS=$(PUBBASEHDRS)
HDRS=util_private.h base_private.h db_private.h error_private.h interface_private.h limits_private.h vector_private.h array_private.h results_private.h $(PROCHDRS) $(SYSHDRS)
LIB=liblnxproc
BASESRCS=error.c results.c limits.c vector.c array.c base.c interface.c
PROCSRCS=proc_cgroups.c proc_diskstats.c proc_domainname.c proc_hostname.c proc_osrelease.c
SYSSRCS=sys_cpufreq.c sys_disksectors.c
SRCS=$(BASESRCS) $(PROCSRCS) $(SYSSRCS)
DIRS=
BIN=test-lnxproc

include $(RELDIR)/Makefile.generic

