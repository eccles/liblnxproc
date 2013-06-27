package ?= topiary
BINDIR=$(prefix)/bin
TSKS=$(package) $(package)-dbg $(package)-prf
SRCS=$(package).c
SHS=$(package).sh

CC=gcc
CCFLAGS=-O2 -I../include -Wall -Winline -Werror
CCFLAGSDBG=-g -I../include -Wall -Winline -Werror
CCFLAGSPRF=-pg -g -I../include -Wall -Winline -Werror
LDFLAGS=-L../src -l$(package)
LDFLAGSDBG=-L../src -l$(package)-dbg
LDFLAGSPRF=../src/lib$(package)-prf.a -pg

all: $(TSKS)

install:
	install -d $(BINDIR)
	install -m 0755 $(TSKS) $(BINDIR)

uninstall:
	-cd $(BINDIR) && rm -f $(TSKS)

$(package): $(package).c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

$(package)-dbg: $(package).c
	$(CC) $(CCFLAGSDBG) -o $@ $^ $(LDFLAGSDBG)

$(package)-prf: $(package).c 
	$(CC) $(CCFLAGSPRF) -o $@ $^ $(LDFLAGSPRF)

clean:
	rm -f $(TSKS) gmon.out *~ *.orig core* vgcore* x y z

.PHONY: all clean install uninstall

# vim: noexpandtab

