RELDIR=..
PUBHDIR=../src/lnxproc
PUBBASEHDRS=lnxproc.h db.h error.h results.h limits.h base.h vector.h array.h
PUBPROCHDRS=proc_cgroups.h proc_domainname.h proc_hostname.h proc_osrelease.h
PUBHDRS=$(PUBBASEHDRS) $(PUBPROCHDRS)
HDRS=base_private.h vector_private.h array_private.h
LIB=liblnxproc
BASESRCS=error.c results.c limits.c vector.c array.c base.c
PROCSRCS=proc_cgroups.c proc_domainname.c proc_hostname.c proc_osrelease.c
SRCS=$(BASESRCS) $(PROCSRCS)
DIRS=
BIN=test-lnxproc

include $(RELDIR)/Makefile.generic

