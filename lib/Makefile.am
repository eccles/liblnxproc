#-----------------------------------------------------------------------
SYSHDRS=topiary_stdlib.h topiary_uthash.h
FUNCHDRS=limit_chr.h strlcpy.h memdup.h val2str.h allocate.h reference.h io.h
PUBBASEHDRS=$(package).h util.h error.h interface.h opt.h print.h
PROCHDRS=modules.h
PUBHDRS=$(PUBBASEHDRS)
PRIVHDRS=config.h util_private.h base_private.h error_private.h interface_private.h \
limits_private.h vector_private.h array_private.h results_private.h \
opt_private.h print_private.h
LIB=lib$(package)
BASESRCS=util.c error.c results.c limits.c vector.c array.c base.c interface.c \
opt.c
MISCSRCS=ntp_drift.c
PROCPIDSRCS=proc_pid_environ.c proc_pid_io.c proc_pid_stat.c proc_pid_statm.c \
proc_pid_status.c proc_pid_smaps.c 
PROCNETRPCSRCS=proc_net_rpc_nfs.c proc_net_rpc_nfsd.c
PROCNETSRCS=proc_net_dev.c proc_net_netstat.c $(PROCNETRPCSRCS) proc_net_snmp.c proc_net_snmp6.c proc_net_sockstat.c
PROCSYSFSSRCS=proc_sys_fs_file-nr.c
PROCSYSSRCS=$(PROCSYSFSSRCS)
PROCSRCS=proc_buddyinfo.c proc_cgroups.c proc_cmdline.c proc_cpuinfo.c \
proc_diskstats.c proc_domainname.c proc_hostname.c proc_interrupts.c \
proc_loadavg.c proc_meminfo.c proc_mounts.c proc_osrelease.c proc_partitions.c \
proc_softirqs.c proc_stat.c proc_uptime.c proc_vmstat.c \
$(PROCNETSRCS) $(PROCPIDSRCS) $(PROCSYSSRCS)
SYSSRCS=sys_cpufreq.c sys_disksectors.c

#-----------------------------------------------------------------------
lib_LTLIBRARIES = libtopiary.la
libtopiary_la_SOURCES = $(BASESRCS) $(PROCSRCS) $(SYSSRCS) $(MISCSRCS) \
$(SYSHDRS) $(PRIVHDRS) $(PROCHDRS) $(FUNCHDRS)
libtopiary_la_CFLAGS = -I$(top_srcdir)/include
libtopiary_la_LDFLAGS = -release 0.0.1 -version-info 0:0:0

#install-exec-hook:
#	if test "$$USER" = "root"; then \
#	groupadd -f topiary;\
#	chgrp topiary $(DESTDIR)/$(libdir)/libtopiary;\
#	fi;
#	chmod 750 $(DESTDIR)$(libdir)/libtopiary
#
CLEANFILES = *.gcno *.gcda *.gcov libtopiary*

# vim: noexpandtab

