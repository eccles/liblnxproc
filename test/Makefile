
TSKS=timing timing-dbg
SRCS=timing.c

CC=gcc
CCFLAGS=-O2 -I../src -Wall
CCFLAGSDBG=-g -I../src -Wall
LDFLAGS=-L../src -llnxproc
LDFLAGSDBG=-L../src -llnxproc-dbg

all: $(TSKS)

timing: timing.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

timing-dbg: timing.c
	$(CC) $(CCFLAGSDBG) -o $@ $^ $(LDFLAGSDBG)

check: $(SRCS)
	$(RELDIR)/check.sh $(SRCS)

clean:
	rm -f $(TSKS) *~ *.orig core* vgcore* x y z

