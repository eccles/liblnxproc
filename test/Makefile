
SRCS=test-lnxproc.c
TSKS=test-lnxproc

CC=gcc
CCFLAGS=-I../src -Wall
LDFLAGS=-L../src -llnxproc

all: $(TSKS)

test-lnxproc: test-lnxproc.c
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

check: $(SRCS)
	$(RELDIR)/check.sh $(SRCS)

clean:
	rm -f $(TSKS)

