
TSKS=timing
SRCS=timing.c

CC=gcc
CCFLAGS=-O2 -I../src -Wall
LDFLAGS=-L../src -llnxproc

all: $(TSKS)

timing: timing.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

check: $(SRCS)
	$(RELDIR)/check.sh $(SRCS)

clean:
	rm -f $(TSKS) *~ *.orig core* vgcore* x y z

