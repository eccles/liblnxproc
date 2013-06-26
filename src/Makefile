LIBDIR=$(prefix)/lib
PUBHDIR=../include/lnxproc
FUNCHDRS=limit_chr.h strlcpy.h memdup.h val2str.h allocate.h reference.h io.h
PUBBASEHDRS=lnxproc.h util.h error.h interface.h opt.h print.h
PROCHDRS=modules.h
PUBHDRS=$(PUBBASEHDRS)
PRIVHDRS=util_private.h base_private.h error_private.h interface_private.h \
limits_private.h vector_private.h array_private.h results_private.h \
opt_private.h print_private.h
HDRS=$(PRIVHDRS) $(PROCHDRS) $(FUNCHDRS)
LIB=liblnxproc
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
SRCS=$(BASESRCS) $(PROCSRCS) $(SYSSRCS) $(MISCSRCS)
DIRS=
BIN=test-lnxproc

.SUFFIXES: .c .o .so

CC=gcc
CCFLAGSD=-DDEBUG=1 -g
CCFLAGSP=-pg -g
CCFLAGSO=-O2
CCFLAGS1=-I../include -Wall -Werror
CCFLAGS2=$(CCFLAGS1) -fpic
#CCFLAGS=-g -fpic -I. -Wall
LDFLAGS1=-L. -llnxproc
LDFLAGS2=-L. -llnxproc-dbg
LDFLAGS3=-pg 
SOFLAGS=-shared
OBJS=$(SRCS:.c=.o)
OBJSDBG=$(SRCS:.c=-dbg.o)
OBJSPRF=$(SRCS:.c=-prf.o)
#TGT1=$(patsubst %,$(BIN)/%,$(SRCS))
#TGT2=$(patsubst %,$(BIN)/%,$(OBJS))
#TARGETS=$(TGT1) $(TGT2)

%.o: %.c $(HDRS)
	$(CC) $(CCFLAGSO) $(CCFLAGS2) -c $< -o $@

%-prf.o: %.c $(HDRS)
	$(CC) $(CCFLAGSP) $(CCFLAGS2) -c $< -o $@

%-dbg.o: %.c $(HDRS)
	$(CC) $(CCFLAGSD) $(CCFLAGS2) -c $< -o $@

all: $(BIN) $(BIN)-dbg $(BIN)-prf
	for d in $(DIRS); do (cd $$d; $(MAKE) $@ ); done

install:
	install -d $(LIBDIR)
	install -m 0755 $(LIB).so $(LIBDIR)
	install -m 0755 $(LIB)-dbg.so $(LIBDIR)
	install -m 0755 $(LIB)-prf.a $(LIBDIR)

uninstall:
	-cd $(LIBDIR) && rm -f $(LIB).so $(LIB)-dbg.so $(LIB)-prf.a

$(LIB).so: $(OBJS)
	$(CC) $(SOFLAGS) -o $@ $^

$(LIB)-dbg.so: $(OBJSDBG)
	$(CC) $(SOFLAGS) -o $@ $^

$(LIB)-prf.a: $(OBJSPRF)
	ar rcs $@ $^ 

$(BIN): $(BIN).c $(LIB).so
	$(CC) $(CCFLAGSO) $(CCFLAGS1) -o $@ $^ $(LDFLAGS1) 

$(BIN)-dbg: $(BIN).c $(LIB)-dbg.so
	$(CC) $(CCFLAGSD) $(CCFLAGS1) -o $@ $^ $(LDFLAGS2)

$(BIN)-prf: $(BIN).c $(LIB)-prf.a
	$(CC) $(CCFLAGSP) $(CCFLAGS1) -o $@ $^ $(LDFLAGS3)

clean:
	for d in $(PUBHDIR); do (cd $$d; rm -f *~ ); done
	rm -f $(BIN) $(BIN)-dbg $(BIN)-prf *.a *.o *.so *~ *.orig core* vgcore* x y z X Y

.PHONY: all clean install uninstall

# vim: set noexpandtab:


