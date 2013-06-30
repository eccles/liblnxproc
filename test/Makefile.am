bin_PROGRAMS = topiary
topiary_SOURCES = topiary.c
topiary_CFLAGS = -I$(top_srcdir)/include
topiary_LDADD = ../src/libtopiary.la
dist_noinst_SCRIPTS = topiary.sh

check_SCRIPTS = check-topiary.sh

check-topiary.sh:
	echo './topiary.sh test nodbg' > check-topiary.sh
	chmod +x check-topiary.sh

CLEANFILES = check-topiary.sh

# vim: noexpandtab

