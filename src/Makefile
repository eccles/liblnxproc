RELDIR=..
PUBHDIR=../src/lnxproc
PUBBASEHDRS=lnxproc.h db.h error.h results.h limits.h base.h vector.h array.h
PUBPROCHDRS=proc_cgroups.h proc_diskstats.h proc_domainname.h proc_hostname.h proc_osrelease.h
PUBSYSHDRS=sys_cpufreq.h sys_disksectors.h
PUBHDRS=$(PUBBASEHDRS) $(PUBPROCHDRS) $(PUBSYSHDRS)
HDRS=base_private.h vector_private.h array_private.h
LIB=liblnxproc
BASESRCS=error.c results.c limits.c vector.c array.c base.c
PROCSRCS=proc_cgroups.c proc_diskstats.c proc_domainname.c proc_hostname.c proc_osrelease.c
SYSSRCS=sys_cpufreq.c sys_disksectors.c
SRCS=$(BASESRCS) $(PROCSRCS) $(SYSSRCS)
DIRS=
BIN=test-lnxproc

include $(RELDIR)/Makefile.generic

