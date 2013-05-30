RELDIR=..
PUBHDIR=$(RELDIR)/include/lnxproc
FUNCHDRS=limit_chr.h strlcpy.h memdup.h val2str.h allocate.h reference.h
PUBBASEHDRS=lnxproc.h util.h error.h interface.h
PROCHDRS=modules.h
PUBHDRS=$(PUBBASEHDRS)
PRIVHDRS=util_private.h base_private.h error_private.h interface_private.h limits_private.h vector_private.h array_private.h results_private.h 
HDRS=$(PRIVHDRS) $(PROCHDRS) $(FUNCHDRS)
LIB=liblnxproc
BASESRCS=util.c error.c results.c limits.c vector.c array.c base.c interface.c
PROCSRCS=proc_buddyinfo.c proc_cgroups.c proc_cmdline.c proc_cpuinfo.c proc_diskstats.c \
proc_domainname.c proc_hostname.c proc_interrupts.c proc_loadavg.c proc_meminfo.c \
proc_mounts.c proc_osrelease.c proc_partitions.c proc_softirqs.c proc_stat.c
PROCPIDSRCS=proc_pid_environ.c proc_pid_stat.c
SYSSRCS=sys_cpufreq.c sys_disksectors.c
SRCS=$(BASESRCS) $(PROCSRCS) $(PROCPIDSRCS) $(SYSSRCS)
DIRS=
BIN=test-lnxproc

include $(RELDIR)/Makefile.generic

