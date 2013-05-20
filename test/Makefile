
RELDIR=../
TSKS=timing timing-dbg timing-prf
SRCS=timing.c

CC=gcc
CCFLAGS=-O2 -I../include -Wall -Winline -Werror
CCFLAGSDBG=-g -I../include -Wall -Winline -Werror
CCFLAGSPRF=-pg -g -I../include -Wall -Winline -Werror
LDFLAGS=-L../src -llnxproc
LDFLAGSDBG=-L../src -llnxproc-dbg
LDFLAGSPRF=../src/liblnxproc-prf.a -pg

all: $(TSKS)

timing: timing.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

timing-dbg: timing.c
	$(CC) $(CCFLAGSDBG) -o $@ $^ $(LDFLAGSDBG)

timing-prf: timing.c 
	$(CC) $(CCFLAGSPRF) -o $@ $^ $(LDFLAGSPRF)

check: $(SRCS)
	$(RELDIR)/check.sh $(SRCS)

clean:
	rm -f $(TSKS) *~ *.orig core* vgcore* x y z

