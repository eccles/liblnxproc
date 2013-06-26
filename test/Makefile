
BINDIR=$(prefix)/bin
TSKS=topiary topiary-dbg topiary-prf
SRCS=topiary.c
SHS=topiary.sh

CC=gcc
CCFLAGS=-O2 -I../include -Wall -Winline -Werror
CCFLAGSDBG=-g -I../include -Wall -Winline -Werror
CCFLAGSPRF=-pg -g -I../include -Wall -Winline -Werror
LDFLAGS=-L../src -llnxproc
LDFLAGSDBG=-L../src -llnxproc-dbg
LDFLAGSPRF=../src/liblnxproc-prf.a -pg

all: $(TSKS)

install:
	install -d $(BINDIR)
	install -m 0755 $(TSKS) $(BINDIR)

uninstall:
	-cd $(BINDIR) && rm -f $(TSKS)

topiary: topiary.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

topiary-dbg: topiary.c
	$(CC) $(CCFLAGSDBG) -o $@ $^ $(LDFLAGSDBG)

topiary-prf: topiary.c 
	$(CC) $(CCFLAGSPRF) -o $@ $^ $(LDFLAGSPRF)

clean:
	rm -f $(TSKS) gmon.out *~ *.orig core* vgcore* x y z

.PHONY: all clean install uninstall

# vim: noexpandtab

