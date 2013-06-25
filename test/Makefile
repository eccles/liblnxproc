
TSKS=timing timing-dbg timing-prf
LTSKS=testing testing-dbg testing-prf
SRCS=timing.c
SHS=testing.sh

CC=gcc
CCFLAGS=-O2 -I../include -Wall -Winline -Werror
CCFLAGSDBG=-g -I../include -Wall -Winline -Werror
CCFLAGSPRF=-pg -g -I../include -Wall -Winline -Werror
LDFLAGS=-L../src -llnxproc
LDFLAGSDBG=-L../src -llnxproc-dbg
LDFLAGSPRF=../src/liblnxproc-prf.a -pg

all: $(TSKS) $(LTSKS) $(SHS)

install:
	@echo "*** $@ not implemented yet ***"

timing: timing.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

testing.sh: timing.sh
	ln -f $^ $@

testing: timing
	ln -f $^ $@

timing-dbg: timing.c
	$(CC) $(CCFLAGSDBG) -o $@ $^ $(LDFLAGSDBG)

testing-dbg: timing-dbg
	ln -f $^ $@

timing-prf: timing.c 
	$(CC) $(CCFLAGSPRF) -o $@ $^ $(LDFLAGSPRF)

testing-prf: timing-prf
	ln -f $^ $@

clean:
	rm -f $(TSKS) $(LTSKS) $(SHS) *~ *.orig core* vgcore* x y z

# vim: noexpandtab

