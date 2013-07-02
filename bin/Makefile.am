bin_PROGRAMS = topiary
noinst_PROGRAMS = test-topiary

topiary_SOURCES = topiary.c
topiary_CFLAGS = -I$(top_srcdir)/include
topiary_LDADD = ../lib/libtopiary.la

test_topiary_SOURCES = test-topiary.c
test_topiary_CFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/lib
test_topiary_LDADD = ../lib/libtopiary.la

install-exec-hook:
	if test "$$USER" = "root"; then \
	groupadd -f topiary;\
	chgrp topiary $(DESTDIR)/$(bindir)/topiary;\
	type setcap; \
	if test $$? -eq 0; then \
	setcap cap_sys_ptrace=eip $(DESTDIR)/$(bindir)/topiary;\
	fi; \
	fi;
	chmod 750 $(DESTDIR)$(bindir)/topiary

# vim: noexpandtab

